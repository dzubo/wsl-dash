#!/usr/bin/env python3
"""
wsl-dash — put data produced inside WSL onto the Windows desktop.

A daemon that runs commands inside WSL on a schedule, caches whatever JSON they
print, and serves it over HTTP where a Windows-side widget can read it. The
widget shipped here is a Rainmeter skin, but nothing below is Rainmeter-specific
except the flat `.txt` rendering — and that exists for a concrete reason:

    Rainmeter's WebParser has no JSON parser. It is a regex engine with capture
    groups. Every "read JSON in Rainmeter" recipe therefore ends in a brittle
    pattern that breaks the moment a field moves or an array grows.

So each producer is served twice. `/p/<name>.json` is the exact output for
normal consumers; `/p/<name>.txt` is the same data flattened to `key=value`
lines, which a widget can read with a two-line regex that cannot care about
nesting. The flat form also carries derived fields a regex engine could never
compute for itself — notably a countdown for every timestamp.

Producers run on their own timer, not on request, so a widget refresh is always
an instant read of cache rather than a blocking shell-out.

Usage:
    ./wsl_dash.py serve                 # run the daemon
    ./wsl_dash.py run <producer>        # run one producer once, print both forms
    ./wsl_dash.py --config path serve   # non-default config location
"""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
import threading
import time
import tomllib
from dataclasses import dataclass, field
from datetime import datetime, timezone
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from typing import Any

VERSION = "0.1.0"
DEFAULT_CONFIG = Path(__file__).with_name("wsl-dash.toml")

# 0.0.0.0, not 127.0.0.1, and this is load-bearing: WSL2's localhost relay only
# forwards a Windows-side connection to a listener bound on all interfaces. A
# loopback-only bind is invisible from Windows and the widget silently shows
# nothing.
DEFAULT_HOST = "0.0.0.0"
DEFAULT_PORT = 8781
DEFAULT_INTERVAL = 300  # generous by default; tighten per-producer in config
DEFAULT_TIMEOUT = 30


# --------------------------------------------------------------------------- #
# Config
# --------------------------------------------------------------------------- #


@dataclass
class Producer:
    name: str
    command: str
    interval: int = DEFAULT_INTERVAL
    timeout: int = DEFAULT_TIMEOUT
    # Parsed from the `index_by = "list_key:field"` config option: the dotted
    # path to a list under the data root, and the field of each item whose value
    # becomes the key. None when the producer isn't indexed.
    index_by: tuple[str, str] | None = None

    # Latest result, guarded by `lock`.
    lock: threading.Lock = field(default_factory=threading.Lock, repr=False)
    ran_at: float | None = None
    ok: bool = False
    exit_code: int | None = None
    error: str = ""
    data: Any = None
    raw: str = ""

    def snapshot(self) -> dict:
        with self.lock:
            return {
                "ran_at": self.ran_at,
                "ok": self.ok,
                "exit_code": self.exit_code,
                "error": self.error,
                "data": self.data,
                "raw": self.raw,
            }


@dataclass
class Config:
    host: str = DEFAULT_HOST
    port: int = DEFAULT_PORT
    producers: dict[str, Producer] = field(default_factory=dict)


def load_config(path: Path) -> Config:
    if not path.exists():
        sys.exit(
            f"wsl-dash: no config at {path}\n"
            f"Copy wsl-dash.example.toml to {path.name} and edit it."
        )
    raw = tomllib.loads(path.read_text())
    cfg = Config(
        host=raw.get("host", DEFAULT_HOST),
        port=int(raw.get("port", DEFAULT_PORT)),
    )
    entries = raw.get("producer", [])
    if not entries:
        sys.exit(f"wsl-dash: {path} defines no [[producer]] blocks")
    for entry in entries:
        try:
            name = entry["name"]
            command = entry["command"]
        except KeyError as exc:
            sys.exit(f"wsl-dash: a [[producer]] block is missing {exc}")
        if name in cfg.producers:
            sys.exit(f"wsl-dash: duplicate producer name {name!r}")
        cfg.producers[name] = Producer(
            name=name,
            command=command,
            interval=int(entry.get("interval", DEFAULT_INTERVAL)),
            timeout=int(entry.get("timeout", DEFAULT_TIMEOUT)),
            index_by=parse_index_by(name, entry.get("index_by")),
        )
    return cfg


def parse_index_by(name: str, spec: object) -> tuple[str, str] | None:
    """Turn the `index_by` config value into a (list_key, field) pair.

    The format is "<list key>:<field>", e.g. "records:account". The list key is
    a dotted path under the data root; the field is the name of a scalar field on
    each list item. Malformed specs are config errors and stop startup, because
    a bad spec can never produce data and would otherwise fail silently on every
    run.
    """
    if spec is None:
        return None
    if not isinstance(spec, str) or ":" not in spec:
        sys.exit(
            f"wsl-dash: producer {name!r} index_by must be \"list_key:field\", "
            f"got {spec!r}"
        )
    list_key, _, field = spec.partition(":")
    if not list_key or not field or ":" in field:
        sys.exit(
            f"wsl-dash: producer {name!r} index_by must be \"list_key:field\" "
            f"with a non-empty key and field, got {spec!r}"
        )
    # Both halves become key-path segments, so reject the characters that would
    # break the flat form's `\\nkey=` anchoring before the producer ever runs.
    # The list key is a dotted path (`.` is its separator, so it is allowed
    # there); the field is a single name, so a `.` in it would instead be
    # accepted and then silently index nothing on every run.
    if any(ch.isspace() or ch == "=" for ch in list_key):
        sys.exit(
            f"wsl-dash: producer {name!r} index_by list key must not contain "
            f"whitespace or '=', got {list_key!r}"
        )
    if any(ch.isspace() or ch in ".=" for ch in field):
        sys.exit(
            f"wsl-dash: producer {name!r} index_by field must not contain "
            f"'.', '=' or whitespace, got {field!r}"
        )
    return list_key, field


# --------------------------------------------------------------------------- #
# Running producers
# --------------------------------------------------------------------------- #


def run_producer(p: Producer) -> None:
    """Run a producer once and store the result. Never raises."""
    started = time.time()
    # shell=True on purpose: commands come from the user's own config file and
    # are expected to look like `uv run --project ~/x ~/x/y.py --json`, which
    # wants ~ expansion and a PATH lookup. This is not a network-facing input.
    try:
        proc = subprocess.run(
            p.command,
            shell=True,
            capture_output=True,
            text=True,
            timeout=p.timeout,
        )
        stdout, stderr, code = proc.stdout, proc.stderr, proc.returncode
    except subprocess.TimeoutExpired:
        stdout, stderr, code = "", f"timed out after {p.timeout}s", -1
    except Exception as exc:  # a bad command shouldn't take the daemon down
        stdout, stderr, code = "", f"{type(exc).__name__}: {exc}", -1

    data: Any = None
    error = ""
    if code != 0:
        error = (stderr or stdout or "command failed").strip()[:500]
    else:
        try:
            data = json.loads(stdout)
        except json.JSONDecodeError as exc:
            error = f"stdout is not JSON: {exc}"

    with p.lock:
        p.ran_at = started
        p.ok = code == 0 and not error
        p.exit_code = code
        p.error = error
        p.data = data
        p.raw = stdout

    # An indexed producer's problems are a property of the data, so they are
    # logged here — once per run — not on every `.txt` request, which would fill
    # the journal the way the request logger is deliberately silent to avoid.
    if p.index_by is not None and data is not None:
        for problem in group_indexed(p, data)[1]:
            log(problem)
        # The errors index shares the same field; its problems are logged here
        # too rather than silently dropped. Skip it when the index already
        # targets `errors` directly, which would report each problem twice.
        if p.index_by[0] != "errors":
            for problem in group_errors(p, data)[1]:
                log(problem)

    status = "ok" if p.ok else f"FAIL ({error[:80]})"
    log(f"{p.name}: {status} in {time.time() - started:.2f}s")


def producer_loop(p: Producer, stop: threading.Event) -> None:
    while not stop.is_set():
        run_producer(p)
        stop.wait(p.interval)


# --------------------------------------------------------------------------- #
# Flattening — the part that makes a regex-only widget engine bearable
# --------------------------------------------------------------------------- #


def parse_iso(value: str) -> datetime | None:
    """Parse an ISO-8601 timestamp, tolerating a trailing Z. None if it isn't one."""
    if not (10 <= len(value) <= 40) or value[4] != "-":
        return None
    try:
        dt = datetime.fromisoformat(value.replace("Z", "+00:00"))
    except ValueError:
        return None
    return dt if dt.tzinfo else dt.replace(tzinfo=timezone.utc)


def humanize(seconds: float) -> str:
    """Seconds until something, in the same register fumes itself uses."""
    if seconds <= 0:
        return "now"
    s = int(seconds)
    d, rem = divmod(s, 86400)
    h, rem = divmod(rem, 3600)
    m, _ = divmod(rem, 60)
    if d:
        return f"{d}d {h}h"
    if h:
        return f"{h}h {m}m"
    if m:
        return f"{m}m"
    return f"{s}s"


def scalar(value: Any) -> str:
    """Render one value for the flat form, preserving precision."""
    if value is None:
        return ""
    if value is True:
        return "1"
    if value is False:
        return "0"
    if isinstance(value, float):
        return repr(value)
    if isinstance(value, (int, str)):
        # A stray newline would break the one-key-per-line contract.
        return str(value).replace("\r", " ").replace("\n", " ")
    return json.dumps(value)


# The only characters a bucket key may contain. This is an allowlist, not a
# denylist: a key must be writable literally into the static regex a widget uses
# to read it, so anything regex- or Rainmeter-special (`+`, `#`, `(`, `)`, ...)
# is excluded rather than enumerated. It mirrors the account-name rule upstream
# in dzubo/fumes#1, so the same value indexes the same way in both projects.
SAFE_INDEX_KEY = re.compile(r"^[A-Za-z0-9][A-Za-z0-9_-]*$")


def index_key(value: Any) -> str | None:
    """Render an indexed field's value as a key-path segment, or None if unsafe.

    The value becomes a key path segment a widget must write literally into a
    static regex, so it is held to `SAFE_INDEX_KEY`: an initial alphanumeric
    followed by alphanumerics, `_` or `-`. Anything else — `.`, `=`, whitespace,
    or a regex/Rainmeter metacharacter — either reads as another level or can
    never be matched by the documented pattern, so it is rejected. A missing,
    boolean, or non-scalar value can't be a key either; the caller logs and skips
    those rather than emitting a key that silently can't be matched.
    """
    if value is None or isinstance(value, bool):
        return None
    if isinstance(value, (int, float)):
        s = str(value)
    elif isinstance(value, str):
        s = value
    else:
        return None
    return s if SAFE_INDEX_KEY.match(s) else None


def flatten(value: Any, prefix: str, out: list[tuple[str, str]], now: datetime) -> None:
    """
    Flatten nested JSON to key=value pairs.

    Dicts become `parent.key`, lists become `parent.0` plus a `parent.count` so a
    widget can hide the rows it has no data for. Any value that parses as an
    ISO-8601 timestamp additionally emits `<key>_in_seconds` and a humanized
    `<key>_in` — a countdown is the single most common thing a dashboard wants
    and the single thing a regex cannot derive.
    """
    if isinstance(value, dict):
        for k, v in value.items():
            flatten(v, f"{prefix}.{k}" if prefix else str(k), out, now)
    elif isinstance(value, list):
        out.append((f"{prefix}.count", str(len(value))))
        for i, v in enumerate(value):
            flatten(v, f"{prefix}.{i}", out, now)
    else:
        out.append((prefix, scalar(value)))
        if isinstance(value, str):
            dt = parse_iso(value)
            if dt is not None:
                delta = (dt - now).total_seconds()
                out.append((f"{prefix}_in_seconds", str(int(delta))))
                out.append((f"{prefix}_in", humanize(delta)))


def _group_by_field(
    node: list[Any], field: str
) -> tuple[dict[str, list[Any]], tuple[int, int]]:
    """Group a list of objects by a scalar `field`, skipping ungroupable items.

    Returns the buckets keyed by field value and a (missing, unsafe) count of
    items that were skipped — the field absent, or its value not a safe
    key-path segment (see `index_key`). The counts are returned rather than
    logged so the caller decides when to surface them.
    """
    buckets: dict[str, list[Any]] = {}
    missing = 0
    unsafe = 0
    for item in node:
        if not isinstance(item, dict) or field not in item:
            missing += 1
            continue
        key = index_key(item[field])
        if key is None:
            unsafe += 1
            continue
        buckets.setdefault(key, []).append(item)
    return buckets, (missing, unsafe)


def group_indexed(
    p: Producer, data: Any
) -> tuple[dict[str, list[Any]] | None, list[str]]:
    """
    Group the indexed list's items by their key field.

    Returns the buckets (keyed by field value) and a list of human-readable
    problems — the named list missing or not a list, records missing the field,
    or records whose value can't be a key. Problems are returned rather than
    logged so the caller decides when to surface them: once per producer run,
    not once per HTTP request.
    """
    list_key, field = p.index_by
    node = data
    for part in list_key.split("."):
        if not isinstance(node, dict) or part not in node:
            return None, [
                f"{p.name}: index_by {list_key!r} not found in data; no indexed keys"
            ]
        node = node[part]
    if not isinstance(node, list):
        return None, [
            f"{p.name}: index_by {list_key!r} is not a list; no indexed keys"
        ]

    buckets, (missing, unsafe) = _group_by_field(node, field)
    problems: list[str] = []
    if missing or unsafe:
        problems.append(
            f"{p.name}: index_by {list_key!r}:{field!r} skipped "
            f"{missing} record(s) missing the field and {unsafe} with an unsafe key"
        )
    return buckets, problems


def group_errors(
    p: Producer, data: Any
) -> tuple[dict[str, list[Any]] | None, list[str]]:
    """Index the producer's `errors` list by the index field.

    A failed account leaves no records, so its only trace in the flat form is
    this list. Returns (buckets, problems) like `group_indexed`: `None` when
    there is no `errors` list to index, and an empty dict when the list exists
    but none of its items can be keyed. Problems are returned, not logged, so
    the caller surfaces them once per run rather than on every request.
    """
    field = p.index_by[1]
    errors = data.get("errors") if isinstance(data, dict) else None
    if not isinstance(errors, list):
        return None, []
    buckets, (missing, unsafe) = _group_by_field(errors, field)
    problems: list[str] = []
    if missing or unsafe:
        problems.append(
            f"{p.name}: index_by errors:{field!r} skipped {missing} error(s) "
            f"missing the field and {unsafe} with an unsafe key"
        )
    return buckets, problems


def flatten_indexed(
    p: Producer, data: Any, out: list[tuple[str, str]], now: datetime
) -> None:
    """
    Emit the `data.by_<field>.<value>...` sibling keys for an indexed producer.

    Each list item is re-flattened under `data.by_<field>.<value>.<list key>.<i>`
    so a widget can read one bucket with a static regex and a `#Value#` variable,
    instead of having to compute where a given value's rows start. The original
    positional `data.<list key>.N.*` keys are untouched — this is purely additive.

    A failed item leaves no rows at all, so the producer's `errors` list (when
    present) is indexed under the same field too. That gives a widget a static
    path to its own failure — `data.by_<field>.<value>.errors.0.message` — instead
    of having to search the positional `data.errors.N.*` keys for the one naming
    it. Skipped when the index already targets `errors` directly, which would
    emit every key twice.
    """
    list_key, field = p.index_by
    buckets, _ = group_indexed(p, data)
    if buckets is not None:
        for key, items in buckets.items():
            prefix = f"data.by_{field}.{key}.{list_key}"
            out.append((f"{prefix}.count", str(len(items))))
            for i, item in enumerate(items):
                flatten(item, f"{prefix}.{i}", out, now)

    if list_key != "errors":
        error_buckets, _ = group_errors(p, data)
        if error_buckets is not None:
            for key, items in error_buckets.items():
                prefix = f"data.by_{field}.{key}.errors"
                out.append((f"{prefix}.count", str(len(items))))
                for i, item in enumerate(items):
                    flatten(item, f"{prefix}.{i}", out, now)


def render_txt(p: Producer, snap: dict, now: datetime) -> str:
    """
    The flat form.

    Opens with a `#wsl-dash 1` comment line so that every key line is preceded by
    a newline — which lets a widget anchor its regex on `\\nkey=` and match the
    first line as safely as any other. The `1` is a contract marker: it bumps
    only when a change breaks the flattening rules (say, renaming the `data.`
    prefix or changing how lists flatten), never on additions like `index_by`.
    A widget can read it to tell "the shape changed" from "the network failed".
    """
    age = -1 if snap["ran_at"] is None else int(time.time() - snap["ran_at"])
    pairs: list[tuple[str, str]] = [
        ("producer", p.name),
        ("ok", "1" if snap["ok"] else "0"),
        ("age_seconds", str(age)),
        ("interval", str(p.interval)),
        ("exit_code", "" if snap["exit_code"] is None else str(snap["exit_code"])),
        ("error", scalar(snap["error"])),
        ("served_at", now.isoformat(timespec="seconds")),
        ("served_at_epoch", str(int(now.timestamp()))),
    ]
    if snap["data"] is not None:
        flatten(snap["data"], "data", pairs, now)
        if p.index_by is not None:
            flatten_indexed(p, snap["data"], pairs, now)
    lines = ["#wsl-dash 1"] + [f"{k}={v}" for k, v in pairs]
    return "\n".join(lines) + "\n"


def render_json(p: Producer, snap: dict, now: datetime) -> bytes:
    age = None if snap["ran_at"] is None else int(time.time() - snap["ran_at"])
    body = {
        "producer": p.name,
        "ok": snap["ok"],
        "age_seconds": age,
        "interval": p.interval,
        "exit_code": snap["exit_code"],
        "error": snap["error"],
        "served_at": now.isoformat(timespec="seconds"),
        "data": snap["data"],
    }
    return json.dumps(body, indent=2).encode()


# --------------------------------------------------------------------------- #
# HTTP
# --------------------------------------------------------------------------- #


class Handler(BaseHTTPRequestHandler):
    server_version = f"wsl-dash/{VERSION}"
    config: Config  # set on the class before serving
    verbose: bool = False

    def do_GET(self) -> None:  # noqa: N802 (stdlib naming)
        path = self.path.split("?", 1)[0]
        now = datetime.now(timezone.utc)

        if path in ("/", "/index.json"):
            return self._send(
                json.dumps(
                    {
                        "wsl_dash": VERSION,
                        "producers": [
                            {
                                "name": p.name,
                                "interval": p.interval,
                                "json": f"/p/{p.name}.json",
                                "txt": f"/p/{p.name}.txt",
                            }
                            for p in self.config.producers.values()
                        ],
                    },
                    indent=2,
                ).encode(),
                "application/json",
            )

        if path == "/healthz":
            return self._send(b"ok\n", "text/plain")

        if path.startswith("/p/"):
            stem = path[3:]
            for suffix, kind in ((".json", "json"), (".txt", "txt")):
                if stem.endswith(suffix):
                    name = stem[: -len(suffix)]
                    p = self.config.producers.get(name)
                    if p is None:
                        return self._send(b"no such producer\n", "text/plain", 404)
                    snap = p.snapshot()
                    if kind == "json":
                        return self._send(render_json(p, snap, now), "application/json")
                    return self._send(
                        render_txt(p, snap, now).encode(), "text/plain"
                    )

        self._send(b"not found\n", "text/plain", 404)

    def _send(self, body: bytes, ctype: str, status: int = 200) -> None:
        self.send_response(status)
        self.send_header("Content-Type", f"{ctype}; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        # A widget polling on its own schedule must never be handed a cached body.
        self.send_header("Cache-Control", "no-store")
        self.end_headers()
        self.wfile.write(body)

    def log_message(self, fmt: str, *args) -> None:
        # Silent by default — a widget polling every 2 minutes would fill the
        # journal. `serve --verbose` turns it on, which is the fastest way to
        # tell "my skin can't reach the daemon" from "my regex is wrong".
        if self.verbose:
            log(f"{self.address_string()} {fmt % args}")


# --------------------------------------------------------------------------- #
# Entry points
# --------------------------------------------------------------------------- #


def log(msg: str) -> None:
    print(f"[{datetime.now().strftime('%H:%M:%S')}] {msg}", flush=True)


def cmd_serve(cfg: Config, verbose: bool = False) -> None:
    stop = threading.Event()
    for p in cfg.producers.values():
        threading.Thread(target=producer_loop, args=(p, stop), daemon=True).start()
        log(f"producer {p.name!r} every {p.interval}s: {p.command}")

    Handler.config = cfg
    Handler.verbose = verbose
    httpd = ThreadingHTTPServer((cfg.host, cfg.port), Handler)
    log(f"serving on http://{cfg.host}:{cfg.port} (Windows: http://localhost:{cfg.port})")
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        log("stopping")
    finally:
        stop.set()
        httpd.server_close()


def cmd_run(cfg: Config, name: str) -> None:
    p = cfg.producers.get(name)
    if p is None:
        sys.exit(f"wsl-dash: no producer named {name!r}")
    run_producer(p)
    now = datetime.now(timezone.utc)
    snap = p.snapshot()
    print(render_json(p, snap, now).decode())
    print("--- flat ---")
    print(render_txt(p, snap, now), end="")


def main() -> None:
    ap = argparse.ArgumentParser(prog="wsl-dash", description=__doc__)
    ap.add_argument("--config", type=Path, default=DEFAULT_CONFIG)
    ap.add_argument("--version", action="version", version=f"wsl-dash {VERSION}")
    sub = ap.add_subparsers(dest="cmd", required=True)
    serve = sub.add_parser("serve", help="run the daemon")
    serve.add_argument("--verbose", action="store_true", help="log every request")
    run = sub.add_parser("run", help="run one producer once and print both forms")
    run.add_argument("producer")

    args = ap.parse_args()
    cfg = load_config(args.config)
    if args.cmd == "serve":
        cmd_serve(cfg, verbose=args.verbose)
    else:
        cmd_run(cfg, args.producer)


if __name__ == "__main__":
    main()
