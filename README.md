# wsl-dash

*Data produced inside WSL, on the Windows desktop.*

WSL users have plenty of Linux-side state worth watching — job progress, API
quotas, service health, queue depth — and no pleasant way to see it on the host
without alt-tabbing into a terminal. Rainmeter already solves the rendering.
`wsl-dash` solves the plumbing.

```
+------------------------------+
| AI LIMITS                    |
|                              |
| opencode  go 5-hour   0%  4h |
| opencode  go week     0%  1d |
| claude    5-hour     73%  2h |   <- amber past 70%
| claude    7-day      34%  3d |
| claude-a  5-hour     19%  2h |
| updated 8s ago               |
+------------------------------+
```

A daemon runs commands inside WSL on a schedule, caches whatever JSON they
print, and serves it over HTTP where a Windows widget can read it. The widget
shipped here is a Rainmeter skin fed by [`fumes`](https://github.com/dzubo/fumes),
which reports remaining AI provider limits.

## Why not just point Rainmeter at a JSON endpoint?

Because **Rainmeter's WebParser has no JSON parser.** It is a regex engine with
capture groups. Every "read JSON in Rainmeter" recipe ends in a brittle pattern
that breaks when a field moves or an array grows.

So `wsl-dash` serves every producer twice:

| Endpoint | For |
|---|---|
| `/p/<name>.json` | Normal consumers. Exact producer output, wrapped with status. |
| `/p/<name>.txt` | Widgets. The same data flattened to `key=value` lines. |

The flat form turns a nested document into something a two-line regex can read:

```
#wsl-dash
producer=fumes
ok=1
age_seconds=8
data.records.count=8
data.records.0.account=opencode
data.records.0.label=go 5-hour
data.records.0.pct=0.09855576670077473
data.records.0.resets_at=2026-08-16T01:26:16.927000+00:00
data.records.0.resets_at_in_seconds=16347
data.records.0.resets_at_in=4h 32m
```

Note the last two lines. Any value that parses as an ISO-8601 timestamp gains a
`_in_seconds` and a humanized `_in` countdown, computed fresh on every request —
a countdown is the most common thing a dashboard wants and the one thing a regex
engine cannot derive for itself. Lists gain a `.count` so a skin can hide the
rows it has no data for.

## Install

Python 3.12+, no dependencies.

```bash
git clone git@github.com:dzubo/wsl-dash.git ~/projects/wsl-dash
cd ~/projects/wsl-dash
cp wsl-dash.example.toml wsl-dash.toml   # then edit
./wsl_dash.py run fumes                  # one-shot: see both output forms
./wsl_dash.py serve                      # run the daemon
```

Then, from Windows, confirm the relay works:

```
curl.exe http://localhost:8781/p/fumes.txt
```

Deploy the skin and load it:

```bash
./deploy.sh          # copies skin/ into your Rainmeter Skins folder
```

`deploy.sh` reads the real skins path out of `Rainmeter.ini` rather than
guessing — Windows often redirects `Documents` into OneDrive. Use `--watch` to
re-copy on every edit. In Rainmeter, refresh, then load `WslDash\Fumes`.

### As a service

```bash
cp systemd/wsl-dash.service ~/.config/systemd/user/
systemctl --user daemon-reload
systemctl --user enable --now wsl-dash
loginctl enable-linger "$USER"     # so it survives with no login session
```

## Configuration

```toml
host = "0.0.0.0"
port = 8781

[[producer]]
name = "fumes"
command = "uv run --project ~/projects/fumes ~/projects/fumes/fumes.py --json"
interval = 300
timeout = 30
```

A producer is any command that prints JSON to stdout. It runs on its own timer,
so a widget refresh reads cache and never waits for the command.

## Things that will bite you

Collected the hard way; all verified against Rainmeter 4.5 on Windows 10 22H2
with WSL 2.7.11.

**Bind `0.0.0.0`, not `127.0.0.1`.** WSL2's localhost relay only forwards
Windows-side connections to a listener bound on all interfaces. A loopback-only
bind is simply invisible from Windows.

**A systemd `--user` unit does not inherit your shell's PATH.** Anything in
`~/.local/bin` — `uv`, pipx shims, cargo binaries — is missing, and your
producer dies with `not found` even though the same command works in your
terminal. The shipped unit sets PATH explicitly. `/p/<name>.json` reports the
failure in `ok`, `exit_code`, and `error`, so check there first.

**WSL shuts an idle distro down** about 8 seconds after its last process exits,
and your widget goes blank with it. Add a Task Scheduler entry at logon running
`wsl.exe -d <distro> -e /bin/true` to boot the distro at login.

**In a child WebParser measure, `StringIndex=1` returns the whole match, not the
first capture group** (and `StringIndex=2` returns nothing). Rather than fight
it, write patterns with no capture group at all and let a fixed-length
lookbehind do the work:

```ini
RegExp=(?<=\ndata\.records\.0\.pct=)[^\n]*
StringIndex=1
```

This is why the flat format opens with a `#wsl-dash` comment line: it guarantees
every key line is preceded by a newline, so `\nkey=` anchors work on the first
line as safely as on any other.

**The parent measure's regex must contain a capture group.** `RegExp=(?s).*`
leaves the parent with zero substrings and every child silently reads nothing —
producing a skin that draws its header and no data. Use `RegExp=(?s)(.*)`.

**Rainmeter reads `.ini` as ANSI unless the file has a BOM.** A UTF-8 `·`
renders as `Â·`. Keep skin files ASCII.

**New skin folders need `!RefreshApp`** before `!ActivateConfig` can find them.

**`Rainmeter.exe` has no `--version`.** An unrecognized argument is treated as a
settings path, and it opens a modal error dialog — which will hang any script
waiting on the process.

## Writing your own widget

Add a producer to `wsl-dash.toml`, restart, and check `/p/<name>.txt` to see
your key names. Then copy `skin/WslDash/Fumes/Fumes.ini` and change the
lookbehinds. The row blocks are unrolled because Rainmeter has no loops; to show
more rows, copy the last block and bump every index in it.

## Status

v0.1. One producer, one skin, deliberately. A plugin API, extra transports, a
widget library, and `.rmskin` packaging all wait for a real second use case.

## License

MIT
