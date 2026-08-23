#!/usr/bin/env python3
"""Boundary-condition checks for the flat-form helpers.

Run directly, no test framework and no dependencies:

    python3 test_wsl_dash.py

These are the functions that regress silently — an unsafe key that slips
through `index_key`, a malformed `index_by` spec that `parse_index_by` accepts,
or an adaptive schedule that stretches when it should not — so they get a few
plain asserts each. The scheduler is in here because its failure mode is a
dashboard that is quietly minutes out of date, which looks exactly like a
dashboard that is fine.
"""

from __future__ import annotations

import contextlib
import io
from datetime import datetime, timezone

from wsl_dash import (
    Producer,
    flatten_indexed,
    group_errors,
    group_indexed,
    index_key,
    parse_backoff,
    parse_index_by,
    parse_max_interval,
    parse_watch,
    reschedule,
)


def _expect_exit(fn, *args) -> None:
    """Assert `fn(*args)` raises SystemExit, silencing its stderr message."""
    with contextlib.redirect_stderr(io.StringIO()):
        try:
            fn(*args)
        except SystemExit:
            return
    raise AssertionError(f"{fn.__name__}{args!r} did not exit")


def check_index_key() -> None:
    ok = [
        "claude",
        "claude_max",
        "claude-max",
        "2fast",
        61,
    ]
    for value in ok:
        assert index_key(value) == str(value), value

    bad = [
        "",
        "claude.max",
        "claude max",
        "a=b",
        "claude+max",
        "team#1",
        "claude(pro)",
        "_claude",
        "-claude",
        None,
        True,
        False,
        61.0,
        [],
        {},
    ]
    for value in bad:
        assert index_key(value) is None, repr(value)


def check_parse_index_by() -> None:
    assert parse_index_by("p", "records:account") == ("records", "account")
    assert parse_index_by("p", "records.sub:account") == ("records.sub", "account")
    assert parse_index_by("p", None) is None

    bad = [
        "records",
        "records:",
        ":account",
        "records:a:b",
        "records:ac=count",
        "records:meta.account",
        "records:ac count",
        "re cords:account",
        "re=ords:account",
    ]
    for spec in bad:
        _expect_exit(parse_index_by, "p", spec)


def check_group_indexed() -> None:
    p = Producer(name="x", command="c", index_by=("records", "account"))
    data = {
        "records": [
            {"account": "claude", "pct": 1},
            {"account": "opencode", "pct": 2},
            {"account": "claude", "pct": 3},
            {"pct": 4},  # missing field
            {"account": "bad.dot", "pct": 5},  # unsafe key
        ]
    }
    buckets, problems = group_indexed(p, data)
    assert set(buckets) == {"claude", "opencode"}, buckets
    assert [i["pct"] for i in buckets["claude"]] == [1, 3]
    assert problems, "skips should be reported once, not silently dropped"

    assert group_indexed(p, {})[0] is None
    assert group_indexed(p, {"records": "nope"})[0] is None


def check_index_errors() -> None:
    p = Producer(name="x", command="c", index_by=("records", "account"))
    data = {
        "records": [{"account": "claude", "pct": 61.0}],
        "errors": [
            {"account": "claude-a", "provider": "claude",
             "message": "HTTP 429 from https://api.anthropic.com"},
        ],
    }
    pairs: list[tuple[str, str]] = []
    flatten_indexed(p, data, pairs, datetime.now(timezone.utc))
    flat = dict(pairs)
    assert flat["data.by_account.claude.records.count"] == "1"
    assert flat["data.by_account.claude-a.errors.count"] == "1"
    assert (
        flat["data.by_account.claude-a.errors.0.message"]
        == "HTTP 429 from https://api.anthropic.com"
    )
    # No errors list -> only the records index is emitted.
    pairs = []
    flatten_indexed(p, {"records": [{"account": "claude", "pct": 1}]},
                    pairs, datetime.now(timezone.utc))
    assert not any(".errors." in k for k in dict(pairs))


def check_group_errors() -> None:
    p = Producer(name="x", command="c", index_by=("records", "account"))
    data = {
        "records": [{"account": "claude", "pct": 1}],
        "errors": [
            {"account": "claude-a", "message": "HTTP 429"},
            {"account": "bad.dot", "message": "x"},  # unsafe key
            {"message": "x"},                        # missing field
        ],
    }
    buckets, problems = group_errors(p, data)
    assert set(buckets) == {"claude-a"}, buckets
    assert problems, "skipped errors should be reported, not silently dropped"

    assert group_errors(p, {"records": []})[0] is None
    assert group_errors(p, {"errors": "nope"})[0] is None


def check_errors_not_double_indexed() -> None:
    # An index that already targets "errors" must not emit the errors index a
    # second time, or the flat form breaks its one-key-per-line contract.
    p = Producer(name="x", command="c", index_by=("errors", "account"))
    data = {"errors": [{"account": "claude", "message": "HTTP 429"}]}
    pairs: list[tuple[str, str]] = []
    flatten_indexed(p, data, pairs, datetime.now(timezone.utc))
    keys = [k for k, _ in pairs if k == "data.by_account.claude.errors.count"]
    assert keys == ["data.by_account.claude.errors.count"], keys


def check_parse_schedule() -> None:
    assert parse_max_interval("x", None, 120) is None
    assert parse_max_interval("x", 900, 120) == 900
    assert parse_max_interval("x", 120, 120) == 120
    # A cap under the base would make the first quiet run poll faster.
    _expect_exit(parse_max_interval, "x", 60, 120)
    _expect_exit(parse_max_interval, "x", "900", 120)
    # TOML booleans are ints in Python; they are not intervals.
    _expect_exit(parse_max_interval, "x", True, 120)

    assert parse_backoff("x", None) == 2.0
    assert parse_backoff("x", 1.5) == 1.5
    # 1 reads like a configured multiplier but never moves the interval.
    _expect_exit(parse_backoff, "x", 1)
    _expect_exit(parse_backoff, "x", 0.5)
    _expect_exit(parse_backoff, "x", True)

    assert parse_watch("x", None) is None
    assert parse_watch("x", "records") == ("records",)
    assert parse_watch("x", ["records", "errors"]) == ("records", "errors")
    _expect_exit(parse_watch, "x", [])
    _expect_exit(parse_watch, "x", ["records", ""])
    _expect_exit(parse_watch, "x", ["a b"])


def _pace(p: Producer, payloads, ok=True) -> list[int]:
    """Feed a producer a series of payloads, returning the interval after each."""
    out = []
    for payload in payloads:
        reschedule(p, payload, ok)
        out.append(p.interval_now)
    return out


def check_schedule_stretches_when_quiet() -> None:
    p = Producer(
        name="x", command="c", interval=120, max_interval=900, watch=("records",)
    )
    same = {"ts": "changes every run", "records": [{"pct": 1}]}
    # The first run is news (nothing to compare against), then the payload sits
    # still. `ts` moving must not count, or nothing ever stretches.
    assert _pace(p, [same, dict(same, ts="b"), dict(same, ts="c"), dict(same, ts="d")]) == [
        120,
        240,
        480,
        900,
    ]
    # Capped, not climbing forever.
    assert _pace(p, [dict(same, ts="e")]) == [900]
    # ...and one real change drops it straight back to the base interval.
    moved = {"ts": "f", "records": [{"pct": 2}]}
    assert _pace(p, [moved]) == [120]


def check_schedule_fixed_without_max_interval() -> None:
    # No max_interval means the pre-adaptive behaviour, unchanged.
    p = Producer(name="x", command="c", interval=120)
    assert _pace(p, [{"a": 1}, {"a": 1}, {"a": 1}]) == [120, 120, 120]
    assert p.quiet_runs == 2  # counted, just not acted on


def check_schedule_quiets_on_errors() -> None:
    # A producer that keeps working for other accounts exits 0 and reports the
    # broken one in `errors`. Changing data must not hide that.
    p = Producer(
        name="x", command="c", interval=120, max_interval=600, watch=("records",)
    )
    broken = [
        {"records": [{"pct": 1}], "errors": [{"message": "token expired"}]},
        {"records": [{"pct": 2}], "errors": [{"message": "token expired"}]},
    ]
    assert _pace(p, broken) == [240, 480]

    # ...unless the operator would rather keep the fast poll for the accounts
    # that still work.
    q = Producer(
        name="x",
        command="c",
        interval=120,
        max_interval=600,
        watch=("records",),
        quiet_on_errors=False,
    )
    assert _pace(q, broken) == [120, 120]


def check_schedule_recovers_from_failure() -> None:
    p = Producer(name="x", command="c", interval=120, max_interval=600)
    payload = {"records": [{"pct": 1}]}
    reschedule(p, payload, True)
    assert p.interval_now == 120
    # A failed run has no data at all: quiet, and it forgets the fingerprint.
    reschedule(p, None, False)
    reschedule(p, None, False)
    assert p.interval_now == 480
    # So the run that recovers is news even though the payload is unchanged —
    # a producer that comes back must not have to earn its way down from the cap.
    reschedule(p, payload, True)
    assert p.interval_now == 120


def check_watch_missing_path_is_reported() -> None:
    p = Producer(name="x", command="c", interval=120, watch=("records", "nope"))
    problems = reschedule(p, {"records": []}, True)
    assert len(problems) == 1 and "nope" in problems[0], problems


if __name__ == "__main__":
    check_index_key()
    check_parse_index_by()
    check_group_indexed()
    check_index_errors()
    check_group_errors()
    check_errors_not_double_indexed()
    check_parse_schedule()
    check_schedule_stretches_when_quiet()
    check_schedule_fixed_without_max_interval()
    check_schedule_quiets_on_errors()
    check_schedule_recovers_from_failure()
    check_watch_missing_path_is_reported()
    print("ok")
