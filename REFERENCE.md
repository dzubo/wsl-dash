# wsl-dash — Reference

Technical documentation for wsl-dash. For what it is and how to install it,
see [README.md](README.md).

## Why not just point Rainmeter at a JSON endpoint?

Because **Rainmeter's WebParser has no JSON parser.** It is a regex engine with
capture groups. Every "read JSON in Rainmeter" recipe ends in a brittle pattern
that breaks when a field moves or an array grows.

So `wsl-dash` serves every producer twice:

| Endpoint | For |
|---|---|
| `/p/<name>.json` | Normal consumers. Exact producer output, wrapped with status. |
| `/p/<name>.txt` | Widgets. The same data flattened to `key=value` lines. |

A producer is any command that prints JSON to stdout. It runs on its own timer,
so a widget refresh reads cache and never waits for the command.

## The flat format

The flat form turns a nested document into something a two-line regex can read:

```
#wsl-dash 1
producer=fumes
ok=1
age_seconds=8
age=8s
interval=120
base_interval=120
quiet_runs=0
next_run_in_seconds=112
next_run_in=1m
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

The same pairing applies to the header's own durations: `age` and `next_run_in`
are the humanized forms of `age_seconds` and `next_run_in_seconds` (`-` when
there is no next run, as with a one-shot `wsl-dash run`). A widget should
generally render those and reserve the raw seconds for arithmetic — with
adaptive scheduling a quiet producer is legitimately 1750 seconds old, and four
digits of seconds in a header reads like a fault rather than a nap.

The file opens with a `#wsl-dash 1` comment line so every key line is preceded
by a newline — `\nkey=` anchors work on the first line as safely as on any
other. The `1` is a **contract marker**: it bumps only when a change breaks the
flattening rules (renaming the `data.` prefix, changing how lists flatten) —
never on additions like `index_by` or new derived fields. A widget can read the
first line to tell "the shape changed, update me" from "the network failed".

## `index_by`

A producer can also index a list of objects by one of its fields:

```toml
[[producer]]
name = "fumes"
command = "..."
index_by = "records:account"   # <list key>:<field>
```

For a `records` list whose items carry an `account` field, the `.txt` form gains
a sibling map alongside the positional `data.records.N.*` keys (which are left
untouched):

```
data.by_account.claude.records.count=2
data.by_account.claude.records.0.label=5-hour session
data.by_account.claude.records.0.pct=61.0
data.by_account.opencode.records.count=1
data.by_account.opencode.records.0.label=go 5-hour
```

The same index covers the producer's `errors` list when it has one, so a widget
can read its own failure instead of searching the positional `data.errors.N.*`
keys for the one that names it:

```
data.by_account.claude.errors.count=1
data.by_account.claude.errors.0.message=HTTP 429 from https://api.anthropic.com
```

A failed account leaves no records, so this `errors` bucket is its only trace in
the flat form — the thing that distinguishes "HTTP 429" from "no limits
configured". Two caveats:

- **The producer must still exit 0.** A non-zero exit means "producer failed":
  wsl-dash serves only `ok`, `exit_code` and `error`, and none of the `data.*`
  keys — including the errors index — are emitted. `fumes` currently exits 1
  when *every* account fails, so a total outage collapses the per-account skins
  to a bare header; a single-account failure (the common case) still renders.
- **An account is expected to have records *or* an error, never both.** The
  per-account skin sizes its panel for one or the other, so a producer that
  emits both for one account would draw them on top of each other. Today's
  `fumes` raises before emitting both.

The skins also read three fields this section does not otherwise name:
`records[].unit` and `records[].used` (the spend line; shown only when `unit` is
`usd`) and `errors[].account`/`errors[].message` (the error line). If a producer
does not emit those, the two lines silently never appear.

A widget then reads a **static** path with the key as a plain `#Variable#` — no
`DynamicVariables`, no computed indices:

```ini
[Variables]
Account=claude

[mR0Pct]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.0\.pct=)[^\n]*
StringIndex=1
```

`StringIndex=1` is not optional — without it a child WebParser measure returns
nothing (see below). This exists because a regex engine cannot select "the rows
where `account=claude`" any more than it can subtract two timestamps. Records
whose key field is missing, or whose value isn't a plain identifier (an
alphanumeric followed by alphanumerics, `_` or `-`, matching the account-name
rule in `fumes`) are skipped and logged once, not silently dropped — the
consumer's failure mode is an empty panel, which is indistinguishable from "no
data".

`index_by` affects `/p/<name>.txt` only. `/p/<name>.json` stays a faithful
passthrough of the producer, exactly as the existing derived fields
(`_in_seconds`, `_in`) are flat-form-only.

## Architecture

A daemon reads `wsl-dash.toml`, runs each producer command on its own timer, and
caches the JSON. Widgets read cache, so a refresh never blocks on a shell-out.
A failed producer run is reported in `/p/<name>.json` via `ok`, `exit_code`,
and `error` — check there first when a widget goes blank.

## Configuration

```toml
host = "0.0.0.0"
port = 8781

[[producer]]
name = "fumes"
command = "uv run --project ~/projects/fumes ~/projects/fumes/fumes.py --json"
interval = 300
timeout = 30
index_by = "records:account"      # optional; see "index_by" above
max_interval = 1800               # optional; see "Adaptive scheduling" below
backoff = 2.0
watch = ["records", "errors"]
quiet_on_errors = true
```

`host = "0.0.0.0"` is **required**, not a default worth changing: WSL2's
localhost relay only forwards Windows-side connections to a listener bound on
all interfaces. A `127.0.0.1` bind is invisible from Windows.

`index_by` takes a single `<list key>:<field>` spec. Multiple indexes are out of
scope — make it a list later if it is ever needed.

## Adaptive scheduling

A fixed interval has to be chosen for the busiest case and then paid for around
the clock. `fumes` is the motivating example: one request to an undocumented
`api.anthropic.com` endpoint per run, worth making every two minutes while
you are burning quota, worth making far less often overnight — or while an
account's OAuth token has been expired since lunchtime and its numbers cannot
move by definition.

`max_interval` is the switch. Without it the producer keeps a fixed timer and
nothing below applies. With it, each run is classified:

| Run | Wait |
|---|---|
| watched data changed, no errors | back to `interval` |
| watched data unchanged | `× backoff`, capped at `max_interval` |
| command failed, timed out, or printed non-JSON | `× backoff`, capped |
| producer reported errors (`quiet_on_errors`) | `× backoff`, capped |

`watch` names the dotted paths whose contents count as news — one string or a
list of them. Leave it out and the whole payload is compared, which is right
until a producer stamps every run with a fresh timestamp; `fumes` emits `ts`,
so nothing would ever compare equal and the interval could never stretch. Name
`records` and `errors` and the noise drops out. A watched path that is missing
from the payload is logged once per run and contributes nothing, the same way an
unresolvable `index_by` is.

`quiet_on_errors` (default `true`) exists because a per-item producer's exit
code says nothing about its items. `fumes` exits 0 when only some accounts fail
and reports the rest in `errors`, so an expired token otherwise reads as a
perfectly healthy run. The trade-off is bluntness — the scheduler works per
producer, so one stuck account slows down the rows that are still moving. Set it
`false` to buy fast rows at the cost of hammering the broken one.

A failed run clears the stored fingerprint, so the run that recovers always
counts as news: a producer that comes back does not have to earn its way down
from the cap.

Both output forms carry the state, so a consumer can tell a slow poll from a
dead one: `interval` is the wait currently in effect, `base_interval` the
configured floor, `quiet_runs` the number of consecutive newsless runs, and
`next_run_in_seconds` the countdown to the next one (`-1` in the flat form when
there is no next run, as with a one-shot `wsl-dash run`). `interval` keeping its
name and changing to mean "in effect" is deliberate: on a fixed timer the two
are identical, so no existing consumer sees a difference.

## Versioning

The three version numbers in play are independent, and that is a decision
rather than an oversight:

| Version | What it measures |
|---|---|
| `wsl_dash.py` `VERSION` | the daemon and its HTTP surface |
| a producer's own `VERSION` (e.g. `fumes`) | that producer's releases, opaque to wsl-dash |
| skin `[Metadata] Version` | one widget's presentation |

They measure different things, and coupling them would mean bumping one project
because another shipped. A fumes release that adds a field must not oblige a
wsl-dash bump, and a skin redesign must not oblige either. wsl-dash never
imports a producer — it runs a shell string from the user's config and knows
nothing about the data's meaning by design — so there is no version constraint
to introduce between them.

What replaces a pin is the `#wsl-dash 1` contract marker plus feature detection:
a consumer should read the key it needs and fall back if it is empty, exactly as
the skin substitutes `"^$"` for absent values. Any minimum-version statement
(e.g. "this skin needs wsl-dash ≥ X") belongs in prose in a README, addressed to
a human deciding whether to upgrade — never in a machine-checked constraint,
because nothing here resolves one.

## Things that will bite you

Collected the hard way; all verified against Rainmeter 4.5 on Windows 10 22H2
with WSL 2.7.11.

**A systemd `--user` unit does not inherit your shell's PATH.** Anything in
`~/.local/bin` — `uv`, pipx shims, cargo binaries — is missing, and your
producer dies with `not found` even though the same command works in your
terminal. The shipped unit sets PATH explicitly.

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

**The parent measure's regex must contain a capture group.** `RegExp=(?s).*`
leaves the parent with zero substrings and every child silently reads nothing —
producing a skin that draws its header and no data. Use `RegExp=(?s)(.*)`.

**Rainmeter reads `.ini` as ANSI unless the file has a BOM.** A UTF-8 `·`
renders as `Â·`. Keep skin files ASCII.

**New skin folders need `!RefreshApp`** before `!ActivateConfig` can find them.
Rainmeter caches the list of available skins.

**`Rainmeter.exe` has no `--version`.** An unrecognized argument is treated as
a settings path, and it opens a modal error dialog — which will hang any script
waiting on the process.

**At 125% DPI scaling, Rainmeter positions in logical pixels while a PowerShell
screen capture returns physical ones.** Multiply by 1.25 when hunting for a
widget in a screenshot.

**`IfCondition` actions fire only when the condition changes state.** A row that
loads holding a sentinel value fires its sentinel action once, and when real
data arrives nothing fires to undo it — the row stays stuck. Set
`IfConditionMode=1` to evaluate every update whenever an initial value can
legitimately match one of the conditions.

**`ClipString` with only a width wraps instead of truncating.** Give a String
meter a width and no height and a long line does not end in an ellipsis — it
flows onto a second line, and on a panel sized for one line it flows out
through the bottom edge. The ellipsis appears only when the height is one a
second line would break: clip to a box, not to a width.

**`AlwaysOnTop=1` ("Topmost") is not enough — use `2` ("Stay topmost").** With
`1`, Windows lets any other topmost window sit above the skin and silently
demotes it; `2` makes Rainmeter re-assert topmost on a timer, so it wins the
position back. The full scale is `-2` on desktop, `-1` bottom, `0` normal, `1`
topmost, `2` stay topmost. No z-order value beats an exclusive-fullscreen app;
that is a Windows limitation, not a skin setting.

**Loading a layout replaces `Rainmeter.ini` wholesale — it does not merge.**
Treat Manage → Layouts → Load as a factory reset. Rainmeter writes a full
pre-load snapshot to `Layouts/@Backup/Rainmeter.ini` first, which is both the
recovery path and a dated record of the previous config. Separately, Rainmeter
rewrites `Rainmeter.ini` from memory when it exits, so hand-edits made while it
runs are discarded — change settings with bangs, which update memory and disk
together. A skin's live z-order is the `WS_EX_TOPMOST` bit (`0x8`) in its
`GetWindowLong(hwnd, GWL_EXSTYLE)`, enumerating windows of class
`RainmeterMeterWindow`.

**Two transparency layers multiply, and only one of them ships.** Window alpha
is `AlphaValue` in `Rainmeter.ini` (local, never in the repo); panel alpha is
the fourth channel of `cPanel` in the skin (shipped). If a panel looks washed
out, check `AlphaValue` before editing the skin — the two compose, and a local
`AlphaValue` below the shipped `cPanel` alpha quietly undoes the skin's
intended opacity. If a translucent panel is ever wanted, lower `cPanel`'s alpha
in the skin instead, so the choice ships and stays visible next to its
rationale.

## Visual design notes

The first cut was correct and ugly. Three causes, worth remembering:

- **Opacity, not colour, was the main problem.** At 84% alpha over a light
  terminal, the backdrop bled through and muddied every row. The panel is now
  near-opaque (`248`). Rainmeter showcase skins are always photographed over
  dark wallpapers, which hides this.
- **Primitive meters only.** `String`/`Bar`/`Image` give square corners and no
  edge. The panel is now a `Meter=Shape` rounded rectangle with a hairline
  stroke — the one thing that makes a Rainmeter skin look designed rather than
  assembled.
- **No hierarchy.** Everything was 9pt in two greys. Rows are now two lines:
  label and a 13pt semibold percentage on top, account and countdown small and
  muted beneath, bar underneath.

Numbers are also honest now: `<1%` for a live-but-tiny value rather than a flat
`0%`, and `--` for uncapped pay-as-you-go rows, which have a null percentage
rather than a zero one.

## Writing your own widget

Add a producer to `wsl-dash.toml`, restart, and check `/p/<name>.txt` to see
your key names. Then copy a `Fumes-<account>` skin folder and change the
lookbehinds. The shared foundation is `@Resources/Common.inc` (palette, geometry,
the download) and `@Resources/Rows.inc` (the unrolled row blocks, keyed on the
skin's `#Account#` variable); a per-account skin is just a few variables and two
`@Include` lines. A third, `@Resources/Glue.inc`, is optional and included only
by a skin that has another one beneath it: it watches this skin's own position
and panel height and `!Move`s the config named in `GlueNext` to sit under it,
`GlueGap` pixels down, which is what keeps the three windows reading as a
single panel. It stacks on `[mHeight]` rather than `#CURRENTCONFIGHEIGHT#`
deliberately — the *window* stays as tall as the full `MaxRows` block because
the hidden row meters still extend it, so a short panel trails a stretch of
transparent window that would otherwise show up as a gap in the stack. The chain is
one hop per skin because `DynamicWindowSize=1` makes a panel's height depend on
its account's row count, and a skin can only read its own height — so each skin
positions its immediate follower rather than one skin placing all the rest. The row blocks are unrolled because Rainmeter has no loops;
`Rows.inc` is six copies of the same block, so edit row 0 and propagate the edit
to rows 1-5. To show more rows, copy the last block, bump every index in it, and
raise `MaxRows` in `Common.inc`.
