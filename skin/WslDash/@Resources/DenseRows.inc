; DenseRows.inc -- the Fumes-dense panel's meter blocks.
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

; The panel and header furniture. The script replaces the Panel shape, the
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

; ---- account 1 meters: #A1# ----

[A1PDot]
Meter=Shape
Group=A1Head
Hidden=1
X=#PAD#
Y=(#Top1# + 4)
Shape=Rectangle 0.5,0.5,4,4,1.5 | Fill Color #cGreen#

[A1Prefix]
Meter=String
Group=A1Head
Hidden=1
X=(#PAD# + 9)
Y=#Top1#
FontFace=#Face#
FontSize=8
FontWeight=600
FontColor=#cFaint#
StringCase=Upper
AntiAlias=1
Text=#A1#
DynamicVariables=1

; The account's error as a strip: a red-tinted rounded band with a dot and
; the clipped message (whole message in the tooltip, pushed in by the
; script).
[A1ErrBg]
Meter=Shape
Group=E1
Hidden=1
X=#PAD#
Y=(#Top1# + #PrefixH#)
Shape=Rectangle 0.5,0.5,(#BarW# - 1),(#ErrH# - 1),4 | Fill Color #cErrBg# | StrokeWidth 1 | Stroke Color #cErrEdge#
DynamicVariables=1

[A1ErrDot]
Meter=Image
Group=E1
Hidden=1
X=(#PAD# + 4)
Y=((#Top1# + #PrefixH#) + 6)
W=4
H=4
SolidColor=#cRed#
DynamicVariables=1

[A1ErrText]
Meter=String
Group=E1
Hidden=1
X=(#PAD# + 13)
Y=((#Top1# + #PrefixH#) + 2)
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

[A1R0Text]
Meter=String
Group=A1R0
Hidden=1
X=#PAD#
Y=((#Top1# + #PrefixH#) + (0 * #RowH#) + 1)
W=#LabelW#
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A1R0Det]
Meter=String
Group=A1R0
Hidden=1
X=#DetX#
Y=((#Top1# + #PrefixH#) + (0 * #RowH#) + 1)
W=#DetW#
H=14
ClipString=1
FontFace=#Face#
FontSize=8
FontColor=#cFaint#
AntiAlias=1
Text=
DynamicVariables=1

[A1R0Pct]
Meter=String
Group=A1R0
Hidden=1
X=(#W# - #PAD#)
Y=((#Top1# + #PrefixH#) + (0 * #RowH#) + 1)
StringAlign=Right
FontFace=#Face#
FontSize=9
FontWeight=600
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A1R0Track]
Meter=Image
Group=A1R0
Hidden=1
X=#PAD#
Y=((#Top1# + #PrefixH#) + (0 * #RowH#) + #BarOff#)
W=#BarW#
H=#BarH#
SolidColor=#cTrack#
DynamicVariables=1

[A1R0Fill]
Meter=Image
Group=A1R0
Hidden=1
X=#PAD#
Y=((#Top1# + #PrefixH#) + (0 * #RowH#) + #BarOff#)
W=0
H=#BarH#
SolidColor=#cLineGreen#
DynamicVariables=1

[A1R1Text]
Meter=String
Group=A1R1
Hidden=1
X=#PAD#
Y=((#Top1# + #PrefixH#) + (1 * #RowH#) + 1)
W=#LabelW#
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A1R1Det]
Meter=String
Group=A1R1
Hidden=1
X=#DetX#
Y=((#Top1# + #PrefixH#) + (1 * #RowH#) + 1)
W=#DetW#
H=14
ClipString=1
FontFace=#Face#
FontSize=8
FontColor=#cFaint#
AntiAlias=1
Text=
DynamicVariables=1

[A1R1Pct]
Meter=String
Group=A1R1
Hidden=1
X=(#W# - #PAD#)
Y=((#Top1# + #PrefixH#) + (1 * #RowH#) + 1)
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
Hidden=1
X=#PAD#
Y=((#Top1# + #PrefixH#) + (1 * #RowH#) + #BarOff#)
W=#BarW#
H=#BarH#
SolidColor=#cTrack#
DynamicVariables=1

[A1R1Fill]
Meter=Image
Group=A1R1
Hidden=1
X=#PAD#
Y=((#Top1# + #PrefixH#) + (1 * #RowH#) + #BarOff#)
W=0
H=#BarH#
SolidColor=#cLineGreen#
DynamicVariables=1

[A1R2Text]
Meter=String
Group=A1R2
Hidden=1
X=#PAD#
Y=((#Top1# + #PrefixH#) + (2 * #RowH#) + 1)
W=#LabelW#
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A1R2Det]
Meter=String
Group=A1R2
Hidden=1
X=#DetX#
Y=((#Top1# + #PrefixH#) + (2 * #RowH#) + 1)
W=#DetW#
H=14
ClipString=1
FontFace=#Face#
FontSize=8
FontColor=#cFaint#
AntiAlias=1
Text=
DynamicVariables=1

[A1R2Pct]
Meter=String
Group=A1R2
Hidden=1
X=(#W# - #PAD#)
Y=((#Top1# + #PrefixH#) + (2 * #RowH#) + 1)
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
Hidden=1
X=#PAD#
Y=((#Top1# + #PrefixH#) + (2 * #RowH#) + #BarOff#)
W=#BarW#
H=#BarH#
SolidColor=#cTrack#
DynamicVariables=1

[A1R2Fill]
Meter=Image
Group=A1R2
Hidden=1
X=#PAD#
Y=((#Top1# + #PrefixH#) + (2 * #RowH#) + #BarOff#)
W=0
H=#BarH#
SolidColor=#cLineGreen#
DynamicVariables=1

[A1R3Text]
Meter=String
Group=A1R3
Hidden=1
X=#PAD#
Y=((#Top1# + #PrefixH#) + (3 * #RowH#) + 1)
W=#LabelW#
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A1R3Det]
Meter=String
Group=A1R3
Hidden=1
X=#DetX#
Y=((#Top1# + #PrefixH#) + (3 * #RowH#) + 1)
W=#DetW#
H=14
ClipString=1
FontFace=#Face#
FontSize=8
FontColor=#cFaint#
AntiAlias=1
Text=
DynamicVariables=1

[A1R3Pct]
Meter=String
Group=A1R3
Hidden=1
X=(#W# - #PAD#)
Y=((#Top1# + #PrefixH#) + (3 * #RowH#) + 1)
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
Hidden=1
X=#PAD#
Y=((#Top1# + #PrefixH#) + (3 * #RowH#) + #BarOff#)
W=#BarW#
H=#BarH#
SolidColor=#cTrack#
DynamicVariables=1

[A1R3Fill]
Meter=Image
Group=A1R3
Hidden=1
X=#PAD#
Y=((#Top1# + #PrefixH#) + (3 * #RowH#) + #BarOff#)
W=0
H=#BarH#
SolidColor=#cLineGreen#
DynamicVariables=1

[A1R4Text]
Meter=String
Group=A1R4
Hidden=1
X=#PAD#
Y=((#Top1# + #PrefixH#) + (4 * #RowH#) + 1)
W=#LabelW#
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A1R4Det]
Meter=String
Group=A1R4
Hidden=1
X=#DetX#
Y=((#Top1# + #PrefixH#) + (4 * #RowH#) + 1)
W=#DetW#
H=14
ClipString=1
FontFace=#Face#
FontSize=8
FontColor=#cFaint#
AntiAlias=1
Text=
DynamicVariables=1

[A1R4Pct]
Meter=String
Group=A1R4
Hidden=1
X=(#W# - #PAD#)
Y=((#Top1# + #PrefixH#) + (4 * #RowH#) + 1)
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
Hidden=1
X=#PAD#
Y=((#Top1# + #PrefixH#) + (4 * #RowH#) + #BarOff#)
W=#BarW#
H=#BarH#
SolidColor=#cTrack#
DynamicVariables=1

[A1R4Fill]
Meter=Image
Group=A1R4
Hidden=1
X=#PAD#
Y=((#Top1# + #PrefixH#) + (4 * #RowH#) + #BarOff#)
W=0
H=#BarH#
SolidColor=#cLineGreen#
DynamicVariables=1

[A1R5Text]
Meter=String
Group=A1R5
Hidden=1
X=#PAD#
Y=((#Top1# + #PrefixH#) + (5 * #RowH#) + 1)
W=#LabelW#
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A1R5Det]
Meter=String
Group=A1R5
Hidden=1
X=#DetX#
Y=((#Top1# + #PrefixH#) + (5 * #RowH#) + 1)
W=#DetW#
H=14
ClipString=1
FontFace=#Face#
FontSize=8
FontColor=#cFaint#
AntiAlias=1
Text=
DynamicVariables=1

[A1R5Pct]
Meter=String
Group=A1R5
Hidden=1
X=(#W# - #PAD#)
Y=((#Top1# + #PrefixH#) + (5 * #RowH#) + 1)
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
Hidden=1
X=#PAD#
Y=((#Top1# + #PrefixH#) + (5 * #RowH#) + #BarOff#)
W=#BarW#
H=#BarH#
SolidColor=#cTrack#
DynamicVariables=1

[A1R5Fill]
Meter=Image
Group=A1R5
Hidden=1
X=#PAD#
Y=((#Top1# + #PrefixH#) + (5 * #RowH#) + #BarOff#)
W=0
H=#BarH#
SolidColor=#cLineGreen#
DynamicVariables=1
; ---- account 2 meters: #A2# ----

[A2PDot]
Meter=Shape
Group=A2Head
Hidden=1
X=#PAD#
Y=(#Top2# + 4)
Shape=Rectangle 0.5,0.5,4,4,1.5 | Fill Color #cGreen#

[A2Prefix]
Meter=String
Group=A2Head
Hidden=1
X=(#PAD# + 9)
Y=#Top2#
FontFace=#Face#
FontSize=8
FontWeight=600
FontColor=#cFaint#
StringCase=Upper
AntiAlias=1
Text=#A2#
DynamicVariables=1

; Hairline divider above the section; dense.lua shows it only when both this
; section and the one above rendered something.
[A2Div]
Meter=Image
Group=Div2
Hidden=1
X=#PAD#
Y=(#Top2# - #SecPad# - 1)
W=#BarW#
H=1
SolidColor=#cRule#
DynamicVariables=1

; The account's error as a strip: a red-tinted rounded band with a dot and
; the clipped message (whole message in the tooltip, pushed in by the
; script).
[A2ErrBg]
Meter=Shape
Group=E2
Hidden=1
X=#PAD#
Y=(#Top2# + #PrefixH#)
Shape=Rectangle 0.5,0.5,(#BarW# - 1),(#ErrH# - 1),4 | Fill Color #cErrBg# | StrokeWidth 1 | Stroke Color #cErrEdge#
DynamicVariables=1

[A2ErrDot]
Meter=Image
Group=E2
Hidden=1
X=(#PAD# + 4)
Y=((#Top2# + #PrefixH#) + 6)
W=4
H=4
SolidColor=#cRed#
DynamicVariables=1

[A2ErrText]
Meter=String
Group=E2
Hidden=1
X=(#PAD# + 13)
Y=((#Top2# + #PrefixH#) + 2)
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

[A2R0Text]
Meter=String
Group=A2R0
Hidden=1
X=#PAD#
Y=((#Top2# + #PrefixH#) + (0 * #RowH#) + 1)
W=#LabelW#
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A2R0Det]
Meter=String
Group=A2R0
Hidden=1
X=#DetX#
Y=((#Top2# + #PrefixH#) + (0 * #RowH#) + 1)
W=#DetW#
H=14
ClipString=1
FontFace=#Face#
FontSize=8
FontColor=#cFaint#
AntiAlias=1
Text=
DynamicVariables=1

[A2R0Pct]
Meter=String
Group=A2R0
Hidden=1
X=(#W# - #PAD#)
Y=((#Top2# + #PrefixH#) + (0 * #RowH#) + 1)
StringAlign=Right
FontFace=#Face#
FontSize=9
FontWeight=600
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A2R0Track]
Meter=Image
Group=A2R0
Hidden=1
X=#PAD#
Y=((#Top2# + #PrefixH#) + (0 * #RowH#) + #BarOff#)
W=#BarW#
H=#BarH#
SolidColor=#cTrack#
DynamicVariables=1

[A2R0Fill]
Meter=Image
Group=A2R0
Hidden=1
X=#PAD#
Y=((#Top2# + #PrefixH#) + (0 * #RowH#) + #BarOff#)
W=0
H=#BarH#
SolidColor=#cLineGreen#
DynamicVariables=1

[A2R1Text]
Meter=String
Group=A2R1
Hidden=1
X=#PAD#
Y=((#Top2# + #PrefixH#) + (1 * #RowH#) + 1)
W=#LabelW#
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A2R1Det]
Meter=String
Group=A2R1
Hidden=1
X=#DetX#
Y=((#Top2# + #PrefixH#) + (1 * #RowH#) + 1)
W=#DetW#
H=14
ClipString=1
FontFace=#Face#
FontSize=8
FontColor=#cFaint#
AntiAlias=1
Text=
DynamicVariables=1

[A2R1Pct]
Meter=String
Group=A2R1
Hidden=1
X=(#W# - #PAD#)
Y=((#Top2# + #PrefixH#) + (1 * #RowH#) + 1)
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
Hidden=1
X=#PAD#
Y=((#Top2# + #PrefixH#) + (1 * #RowH#) + #BarOff#)
W=#BarW#
H=#BarH#
SolidColor=#cTrack#
DynamicVariables=1

[A2R1Fill]
Meter=Image
Group=A2R1
Hidden=1
X=#PAD#
Y=((#Top2# + #PrefixH#) + (1 * #RowH#) + #BarOff#)
W=0
H=#BarH#
SolidColor=#cLineGreen#
DynamicVariables=1

[A2R2Text]
Meter=String
Group=A2R2
Hidden=1
X=#PAD#
Y=((#Top2# + #PrefixH#) + (2 * #RowH#) + 1)
W=#LabelW#
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A2R2Det]
Meter=String
Group=A2R2
Hidden=1
X=#DetX#
Y=((#Top2# + #PrefixH#) + (2 * #RowH#) + 1)
W=#DetW#
H=14
ClipString=1
FontFace=#Face#
FontSize=8
FontColor=#cFaint#
AntiAlias=1
Text=
DynamicVariables=1

[A2R2Pct]
Meter=String
Group=A2R2
Hidden=1
X=(#W# - #PAD#)
Y=((#Top2# + #PrefixH#) + (2 * #RowH#) + 1)
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
Hidden=1
X=#PAD#
Y=((#Top2# + #PrefixH#) + (2 * #RowH#) + #BarOff#)
W=#BarW#
H=#BarH#
SolidColor=#cTrack#
DynamicVariables=1

[A2R2Fill]
Meter=Image
Group=A2R2
Hidden=1
X=#PAD#
Y=((#Top2# + #PrefixH#) + (2 * #RowH#) + #BarOff#)
W=0
H=#BarH#
SolidColor=#cLineGreen#
DynamicVariables=1

[A2R3Text]
Meter=String
Group=A2R3
Hidden=1
X=#PAD#
Y=((#Top2# + #PrefixH#) + (3 * #RowH#) + 1)
W=#LabelW#
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A2R3Det]
Meter=String
Group=A2R3
Hidden=1
X=#DetX#
Y=((#Top2# + #PrefixH#) + (3 * #RowH#) + 1)
W=#DetW#
H=14
ClipString=1
FontFace=#Face#
FontSize=8
FontColor=#cFaint#
AntiAlias=1
Text=
DynamicVariables=1

[A2R3Pct]
Meter=String
Group=A2R3
Hidden=1
X=(#W# - #PAD#)
Y=((#Top2# + #PrefixH#) + (3 * #RowH#) + 1)
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
Hidden=1
X=#PAD#
Y=((#Top2# + #PrefixH#) + (3 * #RowH#) + #BarOff#)
W=#BarW#
H=#BarH#
SolidColor=#cTrack#
DynamicVariables=1

[A2R3Fill]
Meter=Image
Group=A2R3
Hidden=1
X=#PAD#
Y=((#Top2# + #PrefixH#) + (3 * #RowH#) + #BarOff#)
W=0
H=#BarH#
SolidColor=#cLineGreen#
DynamicVariables=1

[A2R4Text]
Meter=String
Group=A2R4
Hidden=1
X=#PAD#
Y=((#Top2# + #PrefixH#) + (4 * #RowH#) + 1)
W=#LabelW#
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A2R4Det]
Meter=String
Group=A2R4
Hidden=1
X=#DetX#
Y=((#Top2# + #PrefixH#) + (4 * #RowH#) + 1)
W=#DetW#
H=14
ClipString=1
FontFace=#Face#
FontSize=8
FontColor=#cFaint#
AntiAlias=1
Text=
DynamicVariables=1

[A2R4Pct]
Meter=String
Group=A2R4
Hidden=1
X=(#W# - #PAD#)
Y=((#Top2# + #PrefixH#) + (4 * #RowH#) + 1)
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
Hidden=1
X=#PAD#
Y=((#Top2# + #PrefixH#) + (4 * #RowH#) + #BarOff#)
W=#BarW#
H=#BarH#
SolidColor=#cTrack#
DynamicVariables=1

[A2R4Fill]
Meter=Image
Group=A2R4
Hidden=1
X=#PAD#
Y=((#Top2# + #PrefixH#) + (4 * #RowH#) + #BarOff#)
W=0
H=#BarH#
SolidColor=#cLineGreen#
DynamicVariables=1

[A2R5Text]
Meter=String
Group=A2R5
Hidden=1
X=#PAD#
Y=((#Top2# + #PrefixH#) + (5 * #RowH#) + 1)
W=#LabelW#
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A2R5Det]
Meter=String
Group=A2R5
Hidden=1
X=#DetX#
Y=((#Top2# + #PrefixH#) + (5 * #RowH#) + 1)
W=#DetW#
H=14
ClipString=1
FontFace=#Face#
FontSize=8
FontColor=#cFaint#
AntiAlias=1
Text=
DynamicVariables=1

[A2R5Pct]
Meter=String
Group=A2R5
Hidden=1
X=(#W# - #PAD#)
Y=((#Top2# + #PrefixH#) + (5 * #RowH#) + 1)
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
Hidden=1
X=#PAD#
Y=((#Top2# + #PrefixH#) + (5 * #RowH#) + #BarOff#)
W=#BarW#
H=#BarH#
SolidColor=#cTrack#
DynamicVariables=1

[A2R5Fill]
Meter=Image
Group=A2R5
Hidden=1
X=#PAD#
Y=((#Top2# + #PrefixH#) + (5 * #RowH#) + #BarOff#)
W=0
H=#BarH#
SolidColor=#cLineGreen#
DynamicVariables=1
; ---- account 3 meters: #A3# ----

[A3PDot]
Meter=Shape
Group=A3Head
Hidden=1
X=#PAD#
Y=(#Top3# + 4)
Shape=Rectangle 0.5,0.5,4,4,1.5 | Fill Color #cGreen#

[A3Prefix]
Meter=String
Group=A3Head
Hidden=1
X=(#PAD# + 9)
Y=#Top3#
FontFace=#Face#
FontSize=8
FontWeight=600
FontColor=#cFaint#
StringCase=Upper
AntiAlias=1
Text=#A3#
DynamicVariables=1

; Hairline divider above the section; dense.lua shows it only when both this
; section and the one above rendered something.
[A3Div]
Meter=Image
Group=Div3
Hidden=1
X=#PAD#
Y=(#Top3# - #SecPad# - 1)
W=#BarW#
H=1
SolidColor=#cRule#
DynamicVariables=1

; The account's error as a strip: a red-tinted rounded band with a dot and
; the clipped message (whole message in the tooltip, pushed in by the
; script).
[A3ErrBg]
Meter=Shape
Group=E3
Hidden=1
X=#PAD#
Y=(#Top3# + #PrefixH#)
Shape=Rectangle 0.5,0.5,(#BarW# - 1),(#ErrH# - 1),4 | Fill Color #cErrBg# | StrokeWidth 1 | Stroke Color #cErrEdge#
DynamicVariables=1

[A3ErrDot]
Meter=Image
Group=E3
Hidden=1
X=(#PAD# + 4)
Y=((#Top3# + #PrefixH#) + 6)
W=4
H=4
SolidColor=#cRed#
DynamicVariables=1

[A3ErrText]
Meter=String
Group=E3
Hidden=1
X=(#PAD# + 13)
Y=((#Top3# + #PrefixH#) + 2)
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

[A3R0Text]
Meter=String
Group=A3R0
Hidden=1
X=#PAD#
Y=((#Top3# + #PrefixH#) + (0 * #RowH#) + 1)
W=#LabelW#
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A3R0Det]
Meter=String
Group=A3R0
Hidden=1
X=#DetX#
Y=((#Top3# + #PrefixH#) + (0 * #RowH#) + 1)
W=#DetW#
H=14
ClipString=1
FontFace=#Face#
FontSize=8
FontColor=#cFaint#
AntiAlias=1
Text=
DynamicVariables=1

[A3R0Pct]
Meter=String
Group=A3R0
Hidden=1
X=(#W# - #PAD#)
Y=((#Top3# + #PrefixH#) + (0 * #RowH#) + 1)
StringAlign=Right
FontFace=#Face#
FontSize=9
FontWeight=600
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A3R0Track]
Meter=Image
Group=A3R0
Hidden=1
X=#PAD#
Y=((#Top3# + #PrefixH#) + (0 * #RowH#) + #BarOff#)
W=#BarW#
H=#BarH#
SolidColor=#cTrack#
DynamicVariables=1

[A3R0Fill]
Meter=Image
Group=A3R0
Hidden=1
X=#PAD#
Y=((#Top3# + #PrefixH#) + (0 * #RowH#) + #BarOff#)
W=0
H=#BarH#
SolidColor=#cLineGreen#
DynamicVariables=1

[A3R1Text]
Meter=String
Group=A3R1
Hidden=1
X=#PAD#
Y=((#Top3# + #PrefixH#) + (1 * #RowH#) + 1)
W=#LabelW#
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A3R1Det]
Meter=String
Group=A3R1
Hidden=1
X=#DetX#
Y=((#Top3# + #PrefixH#) + (1 * #RowH#) + 1)
W=#DetW#
H=14
ClipString=1
FontFace=#Face#
FontSize=8
FontColor=#cFaint#
AntiAlias=1
Text=
DynamicVariables=1

[A3R1Pct]
Meter=String
Group=A3R1
Hidden=1
X=(#W# - #PAD#)
Y=((#Top3# + #PrefixH#) + (1 * #RowH#) + 1)
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
Hidden=1
X=#PAD#
Y=((#Top3# + #PrefixH#) + (1 * #RowH#) + #BarOff#)
W=#BarW#
H=#BarH#
SolidColor=#cTrack#
DynamicVariables=1

[A3R1Fill]
Meter=Image
Group=A3R1
Hidden=1
X=#PAD#
Y=((#Top3# + #PrefixH#) + (1 * #RowH#) + #BarOff#)
W=0
H=#BarH#
SolidColor=#cLineGreen#
DynamicVariables=1

[A3R2Text]
Meter=String
Group=A3R2
Hidden=1
X=#PAD#
Y=((#Top3# + #PrefixH#) + (2 * #RowH#) + 1)
W=#LabelW#
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A3R2Det]
Meter=String
Group=A3R2
Hidden=1
X=#DetX#
Y=((#Top3# + #PrefixH#) + (2 * #RowH#) + 1)
W=#DetW#
H=14
ClipString=1
FontFace=#Face#
FontSize=8
FontColor=#cFaint#
AntiAlias=1
Text=
DynamicVariables=1

[A3R2Pct]
Meter=String
Group=A3R2
Hidden=1
X=(#W# - #PAD#)
Y=((#Top3# + #PrefixH#) + (2 * #RowH#) + 1)
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
Hidden=1
X=#PAD#
Y=((#Top3# + #PrefixH#) + (2 * #RowH#) + #BarOff#)
W=#BarW#
H=#BarH#
SolidColor=#cTrack#
DynamicVariables=1

[A3R2Fill]
Meter=Image
Group=A3R2
Hidden=1
X=#PAD#
Y=((#Top3# + #PrefixH#) + (2 * #RowH#) + #BarOff#)
W=0
H=#BarH#
SolidColor=#cLineGreen#
DynamicVariables=1

[A3R3Text]
Meter=String
Group=A3R3
Hidden=1
X=#PAD#
Y=((#Top3# + #PrefixH#) + (3 * #RowH#) + 1)
W=#LabelW#
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A3R3Det]
Meter=String
Group=A3R3
Hidden=1
X=#DetX#
Y=((#Top3# + #PrefixH#) + (3 * #RowH#) + 1)
W=#DetW#
H=14
ClipString=1
FontFace=#Face#
FontSize=8
FontColor=#cFaint#
AntiAlias=1
Text=
DynamicVariables=1

[A3R3Pct]
Meter=String
Group=A3R3
Hidden=1
X=(#W# - #PAD#)
Y=((#Top3# + #PrefixH#) + (3 * #RowH#) + 1)
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
Hidden=1
X=#PAD#
Y=((#Top3# + #PrefixH#) + (3 * #RowH#) + #BarOff#)
W=#BarW#
H=#BarH#
SolidColor=#cTrack#
DynamicVariables=1

[A3R3Fill]
Meter=Image
Group=A3R3
Hidden=1
X=#PAD#
Y=((#Top3# + #PrefixH#) + (3 * #RowH#) + #BarOff#)
W=0
H=#BarH#
SolidColor=#cLineGreen#
DynamicVariables=1

[A3R4Text]
Meter=String
Group=A3R4
Hidden=1
X=#PAD#
Y=((#Top3# + #PrefixH#) + (4 * #RowH#) + 1)
W=#LabelW#
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A3R4Det]
Meter=String
Group=A3R4
Hidden=1
X=#DetX#
Y=((#Top3# + #PrefixH#) + (4 * #RowH#) + 1)
W=#DetW#
H=14
ClipString=1
FontFace=#Face#
FontSize=8
FontColor=#cFaint#
AntiAlias=1
Text=
DynamicVariables=1

[A3R4Pct]
Meter=String
Group=A3R4
Hidden=1
X=(#W# - #PAD#)
Y=((#Top3# + #PrefixH#) + (4 * #RowH#) + 1)
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
Hidden=1
X=#PAD#
Y=((#Top3# + #PrefixH#) + (4 * #RowH#) + #BarOff#)
W=#BarW#
H=#BarH#
SolidColor=#cTrack#
DynamicVariables=1

[A3R4Fill]
Meter=Image
Group=A3R4
Hidden=1
X=#PAD#
Y=((#Top3# + #PrefixH#) + (4 * #RowH#) + #BarOff#)
W=0
H=#BarH#
SolidColor=#cLineGreen#
DynamicVariables=1

[A3R5Text]
Meter=String
Group=A3R5
Hidden=1
X=#PAD#
Y=((#Top3# + #PrefixH#) + (5 * #RowH#) + 1)
W=#LabelW#
H=14
ClipString=1
FontFace=#Face#
FontSize=9
FontColor=#cText#
AntiAlias=1
Text=
DynamicVariables=1

[A3R5Det]
Meter=String
Group=A3R5
Hidden=1
X=#DetX#
Y=((#Top3# + #PrefixH#) + (5 * #RowH#) + 1)
W=#DetW#
H=14
ClipString=1
FontFace=#Face#
FontSize=8
FontColor=#cFaint#
AntiAlias=1
Text=
DynamicVariables=1

[A3R5Pct]
Meter=String
Group=A3R5
Hidden=1
X=(#W# - #PAD#)
Y=((#Top3# + #PrefixH#) + (5 * #RowH#) + 1)
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
Hidden=1
X=#PAD#
Y=((#Top3# + #PrefixH#) + (5 * #RowH#) + #BarOff#)
W=#BarW#
H=#BarH#
SolidColor=#cTrack#
DynamicVariables=1

[A3R5Fill]
Meter=Image
Group=A3R5
Hidden=1
X=#PAD#
Y=((#Top3# + #PrefixH#) + (5 * #RowH#) + #BarOff#)
W=0
H=#BarH#
SolidColor=#cLineGreen#
DynamicVariables=1
