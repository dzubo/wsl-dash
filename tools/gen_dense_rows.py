#!/usr/bin/env python3
"""Generator for DenseRows.inc (the Fumes-dense panel's meter blocks).

The dense panel's parsing lives in dense.lua (one Lua script reading the
single [mData] WebParser parent); this file is presentation only. Per account:
a tier-coloured prefix dot and small-caps name, a hairline divider above the
section, an error strip shown only when that account failed, and MaxRows
one-line rows -- label, muted details, right-aligned tier-coloured
percentage, and a 2px hairline track + fill beneath. Everything data-driven
starts Hidden=1; dense.lua shows it as data lands. Regenerate rather than
hand-editing the 18 row blocks.

Run from the repo root:  python3 tools/gen_dense_rows.py
"""

import os

OUT = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "skin", "WslDash", "@Resources", "DenseRows.inc")
ACCOUNTS = ["opencode", "claude", "claude-a"]  # A1..A3, matches the .ini
MAXROWS = 6

HEADER = """; DenseRows.inc -- the Fumes-dense panel's meter blocks.
;
; Included by Fumes-dense.ini after Dense.inc. Three blocks, one per account
; named A1..A3 in the skin's [Variables]: a tier-coloured prefix dot and
; small-caps name, a hairline divider above the section (shown only when both
; neighbouring sections render), an error strip shown only when that account
; failed, and MaxRows one-line rows. There are no measures here: dense.lua
; parses the one download and pushes text, colours and fill widths into these
; meters with bangs, so nothing in this file needs to know the flat-form keys.
;
; A row is a label, a muted details column and a right-aligned percentage on
; one 18px line, with a 2px hairline track + tier-coloured fill beneath it --
; the bar is a line under the text, not the row's background. Sections are
; separated by hairline dividers instead of gaps, so the whole panel reads as
; one card.
;
; Everything data-driven starts Hidden=1; the script shows it when data
; lands. Row Y positions hang off the Top1..Top3 skin variables, which the
; script recomputes from the live row counts. Colour tiers: green under 80,
; amber 80-94, red at 95 and up. Keep ASCII: Rainmeter reads
; .inc as ANSI unless there is a BOM.

"""

PROLOGUE = """; The panel and header furniture. The script replaces the Panel shape, the
; Dot colour and the Age text as data lands; the values below are the load
; state before the first fetch.

[Panel]
Meter=Shape
X=0
Y=0
Shape=Rectangle 0.5,0.5,(#W# - 1),(#HeadH# + #Bottom# - 1),8 | Fill Color #cPanel# | StrokeWidth 1 | Stroke Color #cEdge#

[Dot]
Meter=Image
X=#PAD#
Y=9
W=6
H=6
SolidColor=#cGreen#

[Title]
Meter=String
X=(#PAD# + 11)
Y=6
FontFace=#Face#
FontSize=8
FontWeight=600
FontColor=#cMuted#
StringCase=Upper
AntiAlias=1
Text=Usage

[Age]
Meter=String
X=(#W# - #PAD#)
Y=8
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

[A{i}PDot]
Meter=Shape
Group=A{i}Head
Hidden=1
X=#PAD#
Y=(#Top{i}# + 4)
Shape=Rectangle 0.5,0.5,4,4,1.5 | Fill Color #cGreen#

[A{i}Prefix]
Meter=String
Group=A{i}Head
Hidden=1
X=(#PAD# + 9)
Y=#Top{i}#
FontFace=#Face#
FontSize=8
FontWeight=600
FontColor=#cFaint#
StringCase=Upper
AntiAlias=1
Text={av}
DynamicVariables=1
"""
    if i > 1:
        out += f"""
; Hairline divider above the section; dense.lua shows it only when both this
; section and the one above rendered something.
[A{i}Div]
Meter=Image
Group=Div{i}
Hidden=1
X=#PAD#
Y=(#Top{i}# - #SecPad# - 1)
W=#BarW#
H=1
SolidColor=#cRule#
DynamicVariables=1
"""
    out += f"""
; The account's error as a strip: a red-tinted rounded band with a dot and
; the clipped message (whole message in the tooltip, pushed in by the
; script).
[A{i}ErrBg]
Meter=Shape
Group=E{i}
Hidden=1
X=#PAD#
Y=(#Top{i}# + #PrefixH#)
Shape=Rectangle 0.5,0.5,(#BarW# - 1),(#ErrH# - 1),4 | Fill Color #cErrBg# | StrokeWidth 1 | Stroke Color #cErrEdge#
DynamicVariables=1

[A{i}ErrDot]
Meter=Image
Group=E{i}
Hidden=1
X=(#PAD# + 4)
Y=((#Top{i}# + #PrefixH#) + 6)
W=4
H=4
SolidColor=#cRed#
DynamicVariables=1

[A{i}ErrText]
Meter=String
Group=E{i}
Hidden=1
X=(#PAD# + 13)
Y=((#Top{i}# + #PrefixH#) + 2)
FontFace=#Face#
FontSize=9
FontColor=#cRed#
AntiAlias=1
W=#ErrW#
H=#ErrClipH#
ClipString=1
; Alpha-1 backdrop: the whole box, not just the glyphs, is the hover target.
SolidColor=0,0,0,1
DynamicVariables=1
Text=
"""
    for k in range(MAXROWS):
        top = f"(#Top{i}# + #PrefixH#) + ({k} * #RowH#)"
        out += f"""
[A{i}R{k}Text]
Meter=String
Group=A{i}R{k}
Hidden=1
X=#PAD#
Y=({top} + 1)
W=#LabelW#
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A{i}R{k}Det]
Meter=String
Group=A{i}R{k}
Hidden=1
X=#DetX#
Y=({top} + 1)
W=#DetW#
H=14
ClipString=1
FontFace=#Face#
FontSize=8
FontColor=#cFaint#
AntiAlias=1
Text=
DynamicVariables=1

[A{i}R{k}Pct]
Meter=String
Group=A{i}R{k}
Hidden=1
X=(#W# - #PAD#)
Y=({top} + 1)
StringAlign=Right
FontFace=#Face#
FontSize=9
FontWeight=600
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A{i}R{k}Track]
Meter=Image
Group=A{i}R{k}
Hidden=1
X=#PAD#
Y=({top} + #BarOff#)
W=#BarW#
H=#BarH#
SolidColor=#cTrack#
DynamicVariables=1

[A{i}R{k}Fill]
Meter=Image
Group=A{i}R{k}
Hidden=1
X=#PAD#
Y=({top} + #BarOff#)
W=0
H=#BarH#
SolidColor=#cLineGreen#
DynamicVariables=1
"""
    return out


def main() -> None:
    parts = [HEADER, PROLOGUE]
    for i in range(1, len(ACCOUNTS) + 1):
        parts.append(acc_meters(i))
    with open(OUT, "w", newline="\n") as f:
        f.write("".join(parts))
    print(f"wrote {OUT}")


if __name__ == "__main__":
    main()
