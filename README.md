# wsl-dash

*Data produced inside WSL, on the Windows desktop.*

A daemon runs commands inside WSL on a schedule, caches the JSON they print,
and serves it over HTTP where a Windows widget can read it. Ships with a
[Rainmeter](https://www.rainmeter.net/) skin that shows remaining AI provider
limits via [`fumes`](https://github.com/dzubo/fumes).

The schedule can adapt. Give a producer a `max_interval` and every run that
brings no news — the data unchanged, the command failing, an account's token
still expired — stretches the wait, while the first run that brings news drops
it straight back. An endpoint worth polling every two minutes while you work is
not worth it overnight (see [REFERENCE.md](REFERENCE.md#adaptive-scheduling)).

Rainmeter's WebParser can't parse JSON, so each producer is also served as flat
`key=value` lines — the format a widget can actually read. See
[REFERENCE.md](REFERENCE.md) for the endpoint spec and the hard-won Rainmeter
gotchas.

![The compact panel — all accounts merged into one window, with the per-account
error rail at the bottom](screenshots/fumes-compact.png)

## Quick start

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
re-copy on every edit. It also prunes `WslDash` skins that no longer exist in
the repo (scoped to `WslDash`, so your other skins are untouched) — if you
still have the retired single-panel `WslDash\Fumes` loaded, it disappears after
the next deploy. In Rainmeter, run `!RefreshApp`, then load one skin per
account — `WslDash\Fumes-opencode`, `WslDash\Fumes-claude`,
`WslDash\Fumes-claude-a`, one window per account. An experimental
single-panel variant that merges all accounts into one compressed window is
also included: `WslDash\Fumes-compact` (see REFERENCE.md — note its deploy
caveat: restart Rainmeter rather than refreshing it).

The three windows behave as one panel. Position the top skin and the other two
follow: each skin parks the next one directly beneath itself, so the stack
re-forms when you drag it or when a panel grows a row (`@Resources/Glue.inc`,
top to bottom: `opencode`, `claude`, `claude-a`; `GlueGap` sets the seam, 5px
by default, 0 for flush). To move the stack by hand
without one skin leading, CTRL+ALT-click any panel — all three are in the same
`DragGroup`, so they select together and drag, or nudge with the arrow keys, as
a unit; click outside them to deselect.

The per-account skins read the `data.by_account.*` keys, so `wsl-dash.toml`
needs `index_by = "records:account"` on the `fumes` producer (the example
already has it) and wsl-dash ≥ 0.2.0, which is when the humanized `age` key
the header reads was added. Each account is a skin folder; after
adding a new account, copy the closest existing skin in the repo's
`skin/WslDash/`, set its `Account` and `Provider` variables, splice it into the
`GlueNext` chain (point the skin above it at the new config, and give the new
skin a `GlueNext` and an `@Include3` unless it is now the bottom one), and run
`!RefreshApp` so Rainmeter discovers the new folder. Add it in the repo, not
in Rainmeter's Skins folder — `deploy.sh` mirrors `skin/` and prunes `WslDash`
folders it doesn't find there.

### As a service

```bash
cp systemd/wsl-dash.service ~/.config/systemd/user/
systemctl --user daemon-reload
systemctl --user enable --now wsl-dash
loginctl enable-linger "$USER"     # so it survives with no login session
```

## Status

v0.1. One producer, three per-account skins, deliberately. A plugin API, extra
transports, a widget library, and `.rmskin` packaging all wait for a real
second use case.

## License

MIT
