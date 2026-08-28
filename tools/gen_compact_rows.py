#!/usr/bin/env python3
"""Generator for CompactRows.inc (the merged panel's meter blocks).

The compact panel's parsing lives in compact.lua (one Lua script reading the
single [mData] WebParser parent); this file is presentation only: per account
a muted prefix line, an error line, and MaxRows one-line rows whose text,
colours and fill width are pushed in by the script. Regenerate rather than
hand-editing the 18 row blocks.

Run from the repo root:  python3 tools/gen_compact_rows.py
"""

import os

OUT = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "skin", "WslDash", "@Resources", "CompactRows.inc")
ACCOUNTS = ["opencode", "claude", "claude-a"]  # A1..A3, matches the .ini
MAXROWS = 6

HEADER = """; CompactRows.inc -- the merged panel's meter blocks.
;
; Included by Fumes-compact.ini after Compact.inc. Three blocks, one per
; account named A1..A3 in the skin's [Variables]; each block is a muted
; account prefix line, an error line shown only when that account failed, and
; MaxRows one-line rows. There are no measures here: compact.lua parses the
; one download and pushes text, colours and fill widths into these meters
; with bangs, so nothing in this file needs to know the flat-form keys.
;
; A row is one 20px line: a cTrack track as the row background, a fill whose
; width and colour the script sets, the composed "label | $spend | countdown"
; text on the track, and the percentage right-aligned.
;
; Row Y positions hang off the Top1..Top3 skin variables, which the script
; recomputes from the live row counts.
;
; Colour tiers, shifted up from the classic skin so green dominates longer:
; green under 80, amber 80-94, red at 95 and up. Keep ASCII: Rainmeter reads
; .inc as ANSI unless there is a BOM.

"""


def meter_prologue() -> str:
    return """; The panel and header furniture. The script replaces the Panel shape, the
; Dot colour and the Age text as data lands; the values below are the load
; state before the first fetch.

[Panel]
Meter=Shape
X=0
Y=0
Shape=Rectangle 0.5,0.5,(#W# - 1),(#HeadH# + #Bottom# - 1),10 | Fill Color #cPanel# | StrokeWidth 1 | Stroke Color #cEdge#

[Dot]
Meter=Image
X=#PAD#
Y=16
W=6
H=6
SolidColor=#cGreen#

[Title]
Meter=String
X=10R
Y=10
FontFace=#Face#
FontSize=10
FontWeight=600
FontColor=#cMuted#
AntiAlias=1
Text=Usage

[Age]
Meter=String
X=(#W# - #PAD#)
Y=13
StringAlign=Right
FontFace=#Face#
FontSize=8
FontWeight=600
FontColor=#cMuted#
AntiAlias=1
Text=-

[Rule]
Meter=Image
X=#PAD#
Y=#HeadH#
W=#BarW#
H=1
SolidColor=#cRule#

"""


def acc_meters(i: int) -> str:
    a = f"A{i}"
    av = f"#{a}#"
    out = f"""; ---- account {i} meters: {av} ----

[A{i}Prefix]
Meter=String
Group=A{i}Head
X=#PAD#
Y=#Top{i}#
FontFace=#Face#
FontSize=8
FontWeight=600
FontColor=#cFaint#
AntiAlias=1
Text={av}
DynamicVariables=1

[A{i}ErrRail]
Meter=Image
Group=E{i}
X=#PAD#
Y=(#Top{i}# + #PrefixH#)
W=3
H=#ErrH#
SolidColor=#cRed#
DynamicVariables=1

; The account's error (e.g. "HTTP 429"), clipped to the panel and carrying
; the whole message in its tooltip -- both pushed in by the script. The
; alpha-1 backdrop makes the whole box a hover target, not just the glyphs.
[A{i}ErrText]
Meter=String
Group=E{i}
X=10R
Y=((#Top{i}# + #PrefixH#) - 1)
FontFace=#Face#
FontSize=9
FontColor=#cRed#
AntiAlias=1
W=#ErrW#
H=#ErrClipH#
ClipString=1
SolidColor=0,0,0,1
DynamicVariables=1
Text=

"""
    for k in range(MAXROWS):
        top = f"(#Top{i}# + #PrefixH#) + ({k} * #RowH#)"
        out += f"""
[A{i}R{k}Track]
Meter=Image
Group=A{i}R{k}
X=0
Y={top}
W=#W#
H=#RowH#
SolidColor=#cTrack#
DynamicVariables=1

[A{i}R{k}Fill]
Meter=Image
Group=A{i}R{k}
X=0
Y={top}
W=0
H=#RowH#
SolidColor=#cFillGreen#
DynamicVariables=1

[A{i}R{k}Text]
Meter=String
Group=A{i}R{k}
X=#PAD#
Y=(({top}) + 4)
W=(#W# - #PAD# - 36)
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A{i}R{k}Pct]
Meter=String
Group=A{i}R{k}
X=(#W# - #PAD#)
Y=(({top}) + 3)
StringAlign=Right
FontFace=#Face#
FontSize=9
FontWeight=600
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1
"""
    return out


def main() -> None:
    parts = [HEADER]
    parts.append(meter_prologue())
    for i in range(1, len(ACCOUNTS) + 1):
        parts.append(acc_meters(i))
    with open(OUT, "w", newline="\n") as f:
        f.write("".join(parts))
    print(f"wrote {OUT}")


if __name__ == "__main__":
    main()
