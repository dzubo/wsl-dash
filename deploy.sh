#!/usr/bin/env bash
# deploy.sh — copy the skin from this repo into Rainmeter's skins folder.
#
# Rainmeter loads skins from exactly one directory: the SkinPath recorded in
# Rainmeter.ini. That directory is often inside OneDrive (Documents gets
# redirected), which is a poor place for source — it syncs, it locks files
# mid-write, and it makes conflict copies. So the repo stays the source of
# truth and this script pushes a copy.
#
# Usage:
#   ./deploy.sh                 # copy once
#   ./deploy.sh --watch         # copy, then re-copy whenever a file changes
#   ./deploy.sh --to <dir>      # override the destination
#
# After a copy, refresh Rainmeter to pick up structural changes:
#   "/mnt/c/Program Files/Rainmeter/Rainmeter.exe" !RefreshApp

set -euo pipefail

repo_skin="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/skin"
watch=0
dest=""

while [[ $# -gt 0 ]]; do
    case "$1" in
        --watch) watch=1; shift ;;
        --to)    dest="${2:?--to needs a directory}"; shift 2 ;;
        -h|--help) sed -n '2,18p' "${BASH_SOURCE[0]}"; exit 0 ;;
        *) echo "deploy.sh: unknown argument $1" >&2; exit 2 ;;
    esac
done

# Find SkinPath in Rainmeter.ini rather than guessing. The file is UTF-16 with a
# BOM, and its paths are Windows-style with backslashes.
find_skin_path() {
    local ini="/mnt/c/Users/$(detect_user)/AppData/Roaming/Rainmeter/Rainmeter.ini"
    [[ -f "$ini" ]] || return 1
    local line
    line=$(iconv -f UTF-16 -t UTF-8 "$ini" 2>/dev/null | grep -i '^SkinPath=' | head -1) || return 1
    line="${line#*=}"
    line="${line%$'\r'}"
    winpath_to_wsl "$line"
}

detect_user() {
    # cmd.exe knows the Windows username; fall back to scanning /mnt/c/Users.
    local u
    u=$(cmd.exe /c "echo %USERNAME%" 2>/dev/null | tr -d '\r\n') || true
    if [[ -n "$u" && -d "/mnt/c/Users/$u" ]]; then
        echo "$u"; return
    fi
    for d in /mnt/c/Users/*/; do
        case "$(basename "$d")" in
            Public|Default|"Default User"|"All Users") continue ;;
        esac
        basename "$d"; return
    done
    return 1
}

winpath_to_wsl() {
    local p="${1//\\//}"                       # backslashes to slashes
    local drive="${p:0:1}"
    echo "/mnt/${drive,,}/${p:3}"
}

if [[ -z "$dest" ]]; then
    dest=$(find_skin_path) || {
        echo "deploy.sh: could not read SkinPath from Rainmeter.ini; pass --to <dir>" >&2
        exit 1
    }
fi

[[ -d "$dest" ]] || { echo "deploy.sh: destination does not exist: $dest" >&2; exit 1; }

copy_once() {
    cp -r "$repo_skin"/. "$dest"/
    echo "deployed $(find "$repo_skin" -type f | wc -l) file(s) -> $dest"
}

copy_once

if [[ $watch -eq 1 ]]; then
    command -v inotifywait >/dev/null || {
        echo "deploy.sh: --watch needs inotify-tools (sudo apt install inotify-tools)" >&2
        exit 1
    }
    echo "watching $repo_skin ..."
    while inotifywait -qq -r -e modify,create,delete,move "$repo_skin"; do
        copy_once
    done
fi
