#!/usr/bin/env python3
"""Boundary-condition checks for the flat-form helpers.

Run directly, no test framework and no dependencies:

    python3 test_wsl_dash.py

These are the functions that regress silently — an unsafe key that slips
through `index_key`, or a malformed `index_by` spec that `parse_index_by`
accepts — so they get a few plain asserts each.
"""

from __future__ import annotations

import contextlib
import io
from datetime import datetime, timezone

import wsl_dash as m
from wsl_dash import Producer, group_indexed, index_key, parse_index_by


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


if __name__ == "__main__":
    check_index_key()
    check_parse_index_by()
    check_group_indexed()
    print("ok")
