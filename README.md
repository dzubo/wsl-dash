# wsl-dash

*Data produced inside WSL, on the Windows desktop.*

A daemon runs commands inside WSL on a schedule, caches the JSON they print,
and serves it over HTTP where a Windows widget can read it. Ships with a
Rainmeter skin that shows remaining AI provider limits via
[`fumes`](https://github.com/dzubo/fumes).

Rainmeter's WebParser can't parse JSON, so each producer is also served as flat
`key=value` lines — the format a widget can actually read. See
[REFERENCE.md](REFERENCE.md) for the endpoint spec and the hard-won Rainmeter
gotchas.

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
re-copy on every edit. In Rainmeter, refresh, then load `WslDash\Fumes`.

### As a service

```bash
cp systemd/wsl-dash.service ~/.config/systemd/user/
systemctl --user daemon-reload
systemctl --user enable --now wsl-dash
loginctl enable-linger "$USER"     # so it survives with no login session
```

## Status

v0.1. One producer, one skin, deliberately. A plugin API, extra transports, a
widget library, and `.rmskin` packaging all wait for a real second use case.

## License

MIT
