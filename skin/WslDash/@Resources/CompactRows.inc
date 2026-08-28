; CompactRows.inc -- the merged panel's meter blocks.
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

; The panel and header furniture. The script replaces the Panel shape, the
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

; ---- account 1 meters: #A1# ----

[A1Prefix]
Meter=String
Group=A1Head
X=#PAD#
Y=#Top1#
FontFace=#Face#
FontSize=8
FontWeight=600
FontColor=#cFaint#
AntiAlias=1
Text=#A1#
DynamicVariables=1

[A1ErrRail]
Meter=Image
Group=E1
X=#PAD#
Y=(#Top1# + #PrefixH#)
W=3
H=#ErrH#
SolidColor=#cRed#
DynamicVariables=1

; The account's error (e.g. "HTTP 429"), clipped to the panel and carrying
; the whole message in its tooltip -- both pushed in by the script. The
; alpha-1 backdrop makes the whole box a hover target, not just the glyphs.
[A1ErrText]
Meter=String
Group=E1
X=10R
Y=((#Top1# + #PrefixH#) - 1)
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


[A1R0Track]
Meter=Image
Group=A1R0
X=0
Y=(#Top1# + #PrefixH#) + (0 * #RowH#)
W=#W#
H=#RowH#
SolidColor=#cTrack#
DynamicVariables=1

[A1R0Fill]
Meter=Image
Group=A1R0
X=0
Y=(#Top1# + #PrefixH#) + (0 * #RowH#)
W=0
H=#RowH#
SolidColor=#cFillGreen#
DynamicVariables=1

[A1R0Text]
Meter=String
Group=A1R0
X=#PAD#
Y=(((#Top1# + #PrefixH#) + (0 * #RowH#)) + 4)
W=(#W# - #PAD# - 36)
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A1R0Pct]
Meter=String
Group=A1R0
X=(#W# - #PAD#)
Y=(((#Top1# + #PrefixH#) + (0 * #RowH#)) + 3)
StringAlign=Right
FontFace=#Face#
FontSize=9
FontWeight=600
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A1R1Track]
Meter=Image
Group=A1R1
X=0
Y=(#Top1# + #PrefixH#) + (1 * #RowH#)
W=#W#
H=#RowH#
SolidColor=#cTrack#
DynamicVariables=1

[A1R1Fill]
Meter=Image
Group=A1R1
X=0
Y=(#Top1# + #PrefixH#) + (1 * #RowH#)
W=0
H=#RowH#
SolidColor=#cFillGreen#
DynamicVariables=1

[A1R1Text]
Meter=String
Group=A1R1
X=#PAD#
Y=(((#Top1# + #PrefixH#) + (1 * #RowH#)) + 4)
W=(#W# - #PAD# - 36)
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A1R1Pct]
Meter=String
Group=A1R1
X=(#W# - #PAD#)
Y=(((#Top1# + #PrefixH#) + (1 * #RowH#)) + 3)
StringAlign=Right
FontFace=#Face#
FontSize=9
FontWeight=600
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A1R2Track]
Meter=Image
Group=A1R2
X=0
Y=(#Top1# + #PrefixH#) + (2 * #RowH#)
W=#W#
H=#RowH#
SolidColor=#cTrack#
DynamicVariables=1

[A1R2Fill]
Meter=Image
Group=A1R2
X=0
Y=(#Top1# + #PrefixH#) + (2 * #RowH#)
W=0
H=#RowH#
SolidColor=#cFillGreen#
DynamicVariables=1

[A1R2Text]
Meter=String
Group=A1R2
X=#PAD#
Y=(((#Top1# + #PrefixH#) + (2 * #RowH#)) + 4)
W=(#W# - #PAD# - 36)
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A1R2Pct]
Meter=String
Group=A1R2
X=(#W# - #PAD#)
Y=(((#Top1# + #PrefixH#) + (2 * #RowH#)) + 3)
StringAlign=Right
FontFace=#Face#
FontSize=9
FontWeight=600
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A1R3Track]
Meter=Image
Group=A1R3
X=0
Y=(#Top1# + #PrefixH#) + (3 * #RowH#)
W=#W#
H=#RowH#
SolidColor=#cTrack#
DynamicVariables=1

[A1R3Fill]
Meter=Image
Group=A1R3
X=0
Y=(#Top1# + #PrefixH#) + (3 * #RowH#)
W=0
H=#RowH#
SolidColor=#cFillGreen#
DynamicVariables=1

[A1R3Text]
Meter=String
Group=A1R3
X=#PAD#
Y=(((#Top1# + #PrefixH#) + (3 * #RowH#)) + 4)
W=(#W# - #PAD# - 36)
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A1R3Pct]
Meter=String
Group=A1R3
X=(#W# - #PAD#)
Y=(((#Top1# + #PrefixH#) + (3 * #RowH#)) + 3)
StringAlign=Right
FontFace=#Face#
FontSize=9
FontWeight=600
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A1R4Track]
Meter=Image
Group=A1R4
X=0
Y=(#Top1# + #PrefixH#) + (4 * #RowH#)
W=#W#
H=#RowH#
SolidColor=#cTrack#
DynamicVariables=1

[A1R4Fill]
Meter=Image
Group=A1R4
X=0
Y=(#Top1# + #PrefixH#) + (4 * #RowH#)
W=0
H=#RowH#
SolidColor=#cFillGreen#
DynamicVariables=1

[A1R4Text]
Meter=String
Group=A1R4
X=#PAD#
Y=(((#Top1# + #PrefixH#) + (4 * #RowH#)) + 4)
W=(#W# - #PAD# - 36)
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A1R4Pct]
Meter=String
Group=A1R4
X=(#W# - #PAD#)
Y=(((#Top1# + #PrefixH#) + (4 * #RowH#)) + 3)
StringAlign=Right
FontFace=#Face#
FontSize=9
FontWeight=600
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A1R5Track]
Meter=Image
Group=A1R5
X=0
Y=(#Top1# + #PrefixH#) + (5 * #RowH#)
W=#W#
H=#RowH#
SolidColor=#cTrack#
DynamicVariables=1

[A1R5Fill]
Meter=Image
Group=A1R5
X=0
Y=(#Top1# + #PrefixH#) + (5 * #RowH#)
W=0
H=#RowH#
SolidColor=#cFillGreen#
DynamicVariables=1

[A1R5Text]
Meter=String
Group=A1R5
X=#PAD#
Y=(((#Top1# + #PrefixH#) + (5 * #RowH#)) + 4)
W=(#W# - #PAD# - 36)
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A1R5Pct]
Meter=String
Group=A1R5
X=(#W# - #PAD#)
Y=(((#Top1# + #PrefixH#) + (5 * #RowH#)) + 3)
StringAlign=Right
FontFace=#Face#
FontSize=9
FontWeight=600
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1
; ---- account 2 meters: #A2# ----

[A2Prefix]
Meter=String
Group=A2Head
X=#PAD#
Y=#Top2#
FontFace=#Face#
FontSize=8
FontWeight=600
FontColor=#cFaint#
AntiAlias=1
Text=#A2#
DynamicVariables=1

[A2ErrRail]
Meter=Image
Group=E2
X=#PAD#
Y=(#Top2# + #PrefixH#)
W=3
H=#ErrH#
SolidColor=#cRed#
DynamicVariables=1

; The account's error (e.g. "HTTP 429"), clipped to the panel and carrying
; the whole message in its tooltip -- both pushed in by the script. The
; alpha-1 backdrop makes the whole box a hover target, not just the glyphs.
[A2ErrText]
Meter=String
Group=E2
X=10R
Y=((#Top2# + #PrefixH#) - 1)
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


[A2R0Track]
Meter=Image
Group=A2R0
X=0
Y=(#Top2# + #PrefixH#) + (0 * #RowH#)
W=#W#
H=#RowH#
SolidColor=#cTrack#
DynamicVariables=1

[A2R0Fill]
Meter=Image
Group=A2R0
X=0
Y=(#Top2# + #PrefixH#) + (0 * #RowH#)
W=0
H=#RowH#
SolidColor=#cFillGreen#
DynamicVariables=1

[A2R0Text]
Meter=String
Group=A2R0
X=#PAD#
Y=(((#Top2# + #PrefixH#) + (0 * #RowH#)) + 4)
W=(#W# - #PAD# - 36)
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A2R0Pct]
Meter=String
Group=A2R0
X=(#W# - #PAD#)
Y=(((#Top2# + #PrefixH#) + (0 * #RowH#)) + 3)
StringAlign=Right
FontFace=#Face#
FontSize=9
FontWeight=600
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A2R1Track]
Meter=Image
Group=A2R1
X=0
Y=(#Top2# + #PrefixH#) + (1 * #RowH#)
W=#W#
H=#RowH#
SolidColor=#cTrack#
DynamicVariables=1

[A2R1Fill]
Meter=Image
Group=A2R1
X=0
Y=(#Top2# + #PrefixH#) + (1 * #RowH#)
W=0
H=#RowH#
SolidColor=#cFillGreen#
DynamicVariables=1

[A2R1Text]
Meter=String
Group=A2R1
X=#PAD#
Y=(((#Top2# + #PrefixH#) + (1 * #RowH#)) + 4)
W=(#W# - #PAD# - 36)
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A2R1Pct]
Meter=String
Group=A2R1
X=(#W# - #PAD#)
Y=(((#Top2# + #PrefixH#) + (1 * #RowH#)) + 3)
StringAlign=Right
FontFace=#Face#
FontSize=9
FontWeight=600
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A2R2Track]
Meter=Image
Group=A2R2
X=0
Y=(#Top2# + #PrefixH#) + (2 * #RowH#)
W=#W#
H=#RowH#
SolidColor=#cTrack#
DynamicVariables=1

[A2R2Fill]
Meter=Image
Group=A2R2
X=0
Y=(#Top2# + #PrefixH#) + (2 * #RowH#)
W=0
H=#RowH#
SolidColor=#cFillGreen#
DynamicVariables=1

[A2R2Text]
Meter=String
Group=A2R2
X=#PAD#
Y=(((#Top2# + #PrefixH#) + (2 * #RowH#)) + 4)
W=(#W# - #PAD# - 36)
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A2R2Pct]
Meter=String
Group=A2R2
X=(#W# - #PAD#)
Y=(((#Top2# + #PrefixH#) + (2 * #RowH#)) + 3)
StringAlign=Right
FontFace=#Face#
FontSize=9
FontWeight=600
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A2R3Track]
Meter=Image
Group=A2R3
X=0
Y=(#Top2# + #PrefixH#) + (3 * #RowH#)
W=#W#
H=#RowH#
SolidColor=#cTrack#
DynamicVariables=1

[A2R3Fill]
Meter=Image
Group=A2R3
X=0
Y=(#Top2# + #PrefixH#) + (3 * #RowH#)
W=0
H=#RowH#
SolidColor=#cFillGreen#
DynamicVariables=1

[A2R3Text]
Meter=String
Group=A2R3
X=#PAD#
Y=(((#Top2# + #PrefixH#) + (3 * #RowH#)) + 4)
W=(#W# - #PAD# - 36)
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A2R3Pct]
Meter=String
Group=A2R3
X=(#W# - #PAD#)
Y=(((#Top2# + #PrefixH#) + (3 * #RowH#)) + 3)
StringAlign=Right
FontFace=#Face#
FontSize=9
FontWeight=600
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A2R4Track]
Meter=Image
Group=A2R4
X=0
Y=(#Top2# + #PrefixH#) + (4 * #RowH#)
W=#W#
H=#RowH#
SolidColor=#cTrack#
DynamicVariables=1

[A2R4Fill]
Meter=Image
Group=A2R4
X=0
Y=(#Top2# + #PrefixH#) + (4 * #RowH#)
W=0
H=#RowH#
SolidColor=#cFillGreen#
DynamicVariables=1

[A2R4Text]
Meter=String
Group=A2R4
X=#PAD#
Y=(((#Top2# + #PrefixH#) + (4 * #RowH#)) + 4)
W=(#W# - #PAD# - 36)
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A2R4Pct]
Meter=String
Group=A2R4
X=(#W# - #PAD#)
Y=(((#Top2# + #PrefixH#) + (4 * #RowH#)) + 3)
StringAlign=Right
FontFace=#Face#
FontSize=9
FontWeight=600
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A2R5Track]
Meter=Image
Group=A2R5
X=0
Y=(#Top2# + #PrefixH#) + (5 * #RowH#)
W=#W#
H=#RowH#
SolidColor=#cTrack#
DynamicVariables=1

[A2R5Fill]
Meter=Image
Group=A2R5
X=0
Y=(#Top2# + #PrefixH#) + (5 * #RowH#)
W=0
H=#RowH#
SolidColor=#cFillGreen#
DynamicVariables=1

[A2R5Text]
Meter=String
Group=A2R5
X=#PAD#
Y=(((#Top2# + #PrefixH#) + (5 * #RowH#)) + 4)
W=(#W# - #PAD# - 36)
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A2R5Pct]
Meter=String
Group=A2R5
X=(#W# - #PAD#)
Y=(((#Top2# + #PrefixH#) + (5 * #RowH#)) + 3)
StringAlign=Right
FontFace=#Face#
FontSize=9
FontWeight=600
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1
; ---- account 3 meters: #A3# ----

[A3Prefix]
Meter=String
Group=A3Head
X=#PAD#
Y=#Top3#
FontFace=#Face#
FontSize=8
FontWeight=600
FontColor=#cFaint#
AntiAlias=1
Text=#A3#
DynamicVariables=1

[A3ErrRail]
Meter=Image
Group=E3
X=#PAD#
Y=(#Top3# + #PrefixH#)
W=3
H=#ErrH#
SolidColor=#cRed#
DynamicVariables=1

; The account's error (e.g. "HTTP 429"), clipped to the panel and carrying
; the whole message in its tooltip -- both pushed in by the script. The
; alpha-1 backdrop makes the whole box a hover target, not just the glyphs.
[A3ErrText]
Meter=String
Group=E3
X=10R
Y=((#Top3# + #PrefixH#) - 1)
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


[A3R0Track]
Meter=Image
Group=A3R0
X=0
Y=(#Top3# + #PrefixH#) + (0 * #RowH#)
W=#W#
H=#RowH#
SolidColor=#cTrack#
DynamicVariables=1

[A3R0Fill]
Meter=Image
Group=A3R0
X=0
Y=(#Top3# + #PrefixH#) + (0 * #RowH#)
W=0
H=#RowH#
SolidColor=#cFillGreen#
DynamicVariables=1

[A3R0Text]
Meter=String
Group=A3R0
X=#PAD#
Y=(((#Top3# + #PrefixH#) + (0 * #RowH#)) + 4)
W=(#W# - #PAD# - 36)
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A3R0Pct]
Meter=String
Group=A3R0
X=(#W# - #PAD#)
Y=(((#Top3# + #PrefixH#) + (0 * #RowH#)) + 3)
StringAlign=Right
FontFace=#Face#
FontSize=9
FontWeight=600
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A3R1Track]
Meter=Image
Group=A3R1
X=0
Y=(#Top3# + #PrefixH#) + (1 * #RowH#)
W=#W#
H=#RowH#
SolidColor=#cTrack#
DynamicVariables=1

[A3R1Fill]
Meter=Image
Group=A3R1
X=0
Y=(#Top3# + #PrefixH#) + (1 * #RowH#)
W=0
H=#RowH#
SolidColor=#cFillGreen#
DynamicVariables=1

[A3R1Text]
Meter=String
Group=A3R1
X=#PAD#
Y=(((#Top3# + #PrefixH#) + (1 * #RowH#)) + 4)
W=(#W# - #PAD# - 36)
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A3R1Pct]
Meter=String
Group=A3R1
X=(#W# - #PAD#)
Y=(((#Top3# + #PrefixH#) + (1 * #RowH#)) + 3)
StringAlign=Right
FontFace=#Face#
FontSize=9
FontWeight=600
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A3R2Track]
Meter=Image
Group=A3R2
X=0
Y=(#Top3# + #PrefixH#) + (2 * #RowH#)
W=#W#
H=#RowH#
SolidColor=#cTrack#
DynamicVariables=1

[A3R2Fill]
Meter=Image
Group=A3R2
X=0
Y=(#Top3# + #PrefixH#) + (2 * #RowH#)
W=0
H=#RowH#
SolidColor=#cFillGreen#
DynamicVariables=1

[A3R2Text]
Meter=String
Group=A3R2
X=#PAD#
Y=(((#Top3# + #PrefixH#) + (2 * #RowH#)) + 4)
W=(#W# - #PAD# - 36)
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A3R2Pct]
Meter=String
Group=A3R2
X=(#W# - #PAD#)
Y=(((#Top3# + #PrefixH#) + (2 * #RowH#)) + 3)
StringAlign=Right
FontFace=#Face#
FontSize=9
FontWeight=600
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A3R3Track]
Meter=Image
Group=A3R3
X=0
Y=(#Top3# + #PrefixH#) + (3 * #RowH#)
W=#W#
H=#RowH#
SolidColor=#cTrack#
DynamicVariables=1

[A3R3Fill]
Meter=Image
Group=A3R3
X=0
Y=(#Top3# + #PrefixH#) + (3 * #RowH#)
W=0
H=#RowH#
SolidColor=#cFillGreen#
DynamicVariables=1

[A3R3Text]
Meter=String
Group=A3R3
X=#PAD#
Y=(((#Top3# + #PrefixH#) + (3 * #RowH#)) + 4)
W=(#W# - #PAD# - 36)
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A3R3Pct]
Meter=String
Group=A3R3
X=(#W# - #PAD#)
Y=(((#Top3# + #PrefixH#) + (3 * #RowH#)) + 3)
StringAlign=Right
FontFace=#Face#
FontSize=9
FontWeight=600
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A3R4Track]
Meter=Image
Group=A3R4
X=0
Y=(#Top3# + #PrefixH#) + (4 * #RowH#)
W=#W#
H=#RowH#
SolidColor=#cTrack#
DynamicVariables=1

[A3R4Fill]
Meter=Image
Group=A3R4
X=0
Y=(#Top3# + #PrefixH#) + (4 * #RowH#)
W=0
H=#RowH#
SolidColor=#cFillGreen#
DynamicVariables=1

[A3R4Text]
Meter=String
Group=A3R4
X=#PAD#
Y=(((#Top3# + #PrefixH#) + (4 * #RowH#)) + 4)
W=(#W# - #PAD# - 36)
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A3R4Pct]
Meter=String
Group=A3R4
X=(#W# - #PAD#)
Y=(((#Top3# + #PrefixH#) + (4 * #RowH#)) + 3)
StringAlign=Right
FontFace=#Face#
FontSize=9
FontWeight=600
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A3R5Track]
Meter=Image
Group=A3R5
X=0
Y=(#Top3# + #PrefixH#) + (5 * #RowH#)
W=#W#
H=#RowH#
SolidColor=#cTrack#
DynamicVariables=1

[A3R5Fill]
Meter=Image
Group=A3R5
X=0
Y=(#Top3# + #PrefixH#) + (5 * #RowH#)
W=0
H=#RowH#
SolidColor=#cFillGreen#
DynamicVariables=1

[A3R5Text]
Meter=String
Group=A3R5
X=#PAD#
Y=(((#Top3# + #PrefixH#) + (5 * #RowH#)) + 4)
W=(#W# - #PAD# - 36)
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A3R5Pct]
Meter=String
Group=A3R5
X=(#W# - #PAD#)
Y=(((#Top3# + #PrefixH#) + (5 * #RowH#)) + 3)
StringAlign=Right
FontFace=#Face#
FontSize=9
FontWeight=600
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1
