; Rows.inc -- the unrolled row blocks, keyed on #Account#.
;
; Included by each Fumes-<account>/Fumes-<account>.ini. Every regex below reads
; the per-account index (data.by_account.#Account#.records.N.*), so #Account#
; is substituted at parse time and the patterns stay static. Rows are unrolled
; because Rainmeter has no loops; to show more than MaxRows records, copy the
; last block and bump every index in it (and raise MaxRows in Common.inc).
;
; The six blocks are identical except for the row index. When you change one,
; change all six: edit row 0, then propagate the edit to rows 1-5 (and update
; the copy-the-block note in REFERENCE.md if the shape changed).
;
; A row is: label and percentage on top, spend and countdown beneath, bar
; underneath. Spend is shown only for usd rows (opencode, claude's "extra
; usage"); percent rows (claude's 5h/7d) leave the line empty.

; --------------------------------------------------------------------------
; Row measures
; --------------------------------------------------------------------------

; ---- row 0 ----
[mR0Label]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.0\.label=)[^\n]*
StringIndex=1

[mR0Pct]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.0\.pct=)[^\n]*
StringIndex=1
RegExpSubstitute=1
Substitute="^$":"-1"

[mR0Unit]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.0\.unit=)[^\n]*
StringIndex=1
IfMatchMode=1
IfMatch=^usd$
IfMatchAction=[!ShowMeter R0Spend]
IfNotMatchAction=[!HideMeter R0Spend]

[mR0Used]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.0\.used=)[^\n]*
StringIndex=1
RegExpSubstitute=1
Substitute="^$":"0"

; Spend is a Calc, not the raw WebParser string: NumOfDecimals only applies to
; a numeric measure, so routing through here rounds "$6.4884" to "$6.49".
[mR0Spend]
Measure=Calc
Formula=[mR0Used]
DynamicVariables=1

[mR0Reset]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.0\.resets_at_in=)[^\n]*
StringIndex=1

[mR0Calc]
Measure=Calc
Formula=[mR0Pct]
DynamicVariables=1
MinValue=0
MaxValue=100
IfConditionMode=1
IfCondition=(#CURRENTSECTION# >= 90)
IfTrueAction=[!SetOption R0Bar BarColor #cRed#][!SetOption R0Num FontColor #cRed#]
IfCondition2=(#CURRENTSECTION# >= 70) && (#CURRENTSECTION# < 90)
IfTrueAction2=[!SetOption R0Bar BarColor #cAmber#][!SetOption R0Num FontColor #cAmber#]
IfCondition3=(#CURRENTSECTION# < 70)
IfTrueAction3=[!SetOption R0Bar BarColor #cGreen#][!SetOption R0Num FontColor #cText#]
IfCondition4=(#CURRENTSECTION# > 0) && (#CURRENTSECTION# < 1)
IfTrueAction4=[!SetOption R0Num Text "<1%"]
IfFalseAction4=[!SetOption R0Num Text "%1%"]
IfCondition5=(#CURRENTSECTION# < 0)
IfTrueAction5=[!SetOption R0Num Text "--"][!SetOption R0Num FontColor #cFaint#]

; Hides the whole row when the account has fewer than 1 record(s). The
; unit re-check after the group show re-hides the spend line on non-usd rows,
; which !ShowMeterGroup would otherwise re-show.
[mR0Vis]
Measure=Calc
Formula=[mCount]
DynamicVariables=1
IfConditionMode=1
IfCondition=(#CURRENTSECTION# > 0)
IfTrueAction=[!ShowMeterGroup R0][!UpdateMeasure mR0Unit]
IfFalseAction=[!HideMeterGroup R0]

; ---- row 1 ----
[mR1Label]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.1\.label=)[^\n]*
StringIndex=1

[mR1Pct]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.1\.pct=)[^\n]*
StringIndex=1
RegExpSubstitute=1
Substitute="^$":"-1"

[mR1Unit]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.1\.unit=)[^\n]*
StringIndex=1
IfMatchMode=1
IfMatch=^usd$
IfMatchAction=[!ShowMeter R1Spend]
IfNotMatchAction=[!HideMeter R1Spend]

[mR1Used]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.1\.used=)[^\n]*
StringIndex=1
RegExpSubstitute=1
Substitute="^$":"0"

; Spend is a Calc, not the raw WebParser string: NumOfDecimals only applies to
; a numeric measure, so routing through here rounds "$6.4884" to "$6.49".
[mR1Spend]
Measure=Calc
Formula=[mR1Used]
DynamicVariables=1

[mR1Reset]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.1\.resets_at_in=)[^\n]*
StringIndex=1

[mR1Calc]
Measure=Calc
Formula=[mR1Pct]
DynamicVariables=1
MinValue=0
MaxValue=100
IfConditionMode=1
IfCondition=(#CURRENTSECTION# >= 90)
IfTrueAction=[!SetOption R1Bar BarColor #cRed#][!SetOption R1Num FontColor #cRed#]
IfCondition2=(#CURRENTSECTION# >= 70) && (#CURRENTSECTION# < 90)
IfTrueAction2=[!SetOption R1Bar BarColor #cAmber#][!SetOption R1Num FontColor #cAmber#]
IfCondition3=(#CURRENTSECTION# < 70)
IfTrueAction3=[!SetOption R1Bar BarColor #cGreen#][!SetOption R1Num FontColor #cText#]
IfCondition4=(#CURRENTSECTION# > 0) && (#CURRENTSECTION# < 1)
IfTrueAction4=[!SetOption R1Num Text "<1%"]
IfFalseAction4=[!SetOption R1Num Text "%1%"]
IfCondition5=(#CURRENTSECTION# < 0)
IfTrueAction5=[!SetOption R1Num Text "--"][!SetOption R1Num FontColor #cFaint#]

; Hides the whole row when the account has fewer than 2 record(s). The
; unit re-check after the group show re-hides the spend line on non-usd rows,
; which !ShowMeterGroup would otherwise re-show.
[mR1Vis]
Measure=Calc
Formula=[mCount]
DynamicVariables=1
IfConditionMode=1
IfCondition=(#CURRENTSECTION# > 1)
IfTrueAction=[!ShowMeterGroup R1][!UpdateMeasure mR1Unit]
IfFalseAction=[!HideMeterGroup R1]

; ---- row 2 ----
[mR2Label]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.2\.label=)[^\n]*
StringIndex=1

[mR2Pct]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.2\.pct=)[^\n]*
StringIndex=1
RegExpSubstitute=1
Substitute="^$":"-1"

[mR2Unit]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.2\.unit=)[^\n]*
StringIndex=1
IfMatchMode=1
IfMatch=^usd$
IfMatchAction=[!ShowMeter R2Spend]
IfNotMatchAction=[!HideMeter R2Spend]

[mR2Used]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.2\.used=)[^\n]*
StringIndex=1
RegExpSubstitute=1
Substitute="^$":"0"

; Spend is a Calc, not the raw WebParser string: NumOfDecimals only applies to
; a numeric measure, so routing through here rounds "$6.4884" to "$6.49".
[mR2Spend]
Measure=Calc
Formula=[mR2Used]
DynamicVariables=1

[mR2Reset]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.2\.resets_at_in=)[^\n]*
StringIndex=1

[mR2Calc]
Measure=Calc
Formula=[mR2Pct]
DynamicVariables=1
MinValue=0
MaxValue=100
IfConditionMode=1
IfCondition=(#CURRENTSECTION# >= 90)
IfTrueAction=[!SetOption R2Bar BarColor #cRed#][!SetOption R2Num FontColor #cRed#]
IfCondition2=(#CURRENTSECTION# >= 70) && (#CURRENTSECTION# < 90)
IfTrueAction2=[!SetOption R2Bar BarColor #cAmber#][!SetOption R2Num FontColor #cAmber#]
IfCondition3=(#CURRENTSECTION# < 70)
IfTrueAction3=[!SetOption R2Bar BarColor #cGreen#][!SetOption R2Num FontColor #cText#]
IfCondition4=(#CURRENTSECTION# > 0) && (#CURRENTSECTION# < 1)
IfTrueAction4=[!SetOption R2Num Text "<1%"]
IfFalseAction4=[!SetOption R2Num Text "%1%"]
IfCondition5=(#CURRENTSECTION# < 0)
IfTrueAction5=[!SetOption R2Num Text "--"][!SetOption R2Num FontColor #cFaint#]

; Hides the whole row when the account has fewer than 3 record(s). The
; unit re-check after the group show re-hides the spend line on non-usd rows,
; which !ShowMeterGroup would otherwise re-show.
[mR2Vis]
Measure=Calc
Formula=[mCount]
DynamicVariables=1
IfConditionMode=1
IfCondition=(#CURRENTSECTION# > 2)
IfTrueAction=[!ShowMeterGroup R2][!UpdateMeasure mR2Unit]
IfFalseAction=[!HideMeterGroup R2]

; ---- row 3 ----
[mR3Label]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.3\.label=)[^\n]*
StringIndex=1

[mR3Pct]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.3\.pct=)[^\n]*
StringIndex=1
RegExpSubstitute=1
Substitute="^$":"-1"

[mR3Unit]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.3\.unit=)[^\n]*
StringIndex=1
IfMatchMode=1
IfMatch=^usd$
IfMatchAction=[!ShowMeter R3Spend]
IfNotMatchAction=[!HideMeter R3Spend]

[mR3Used]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.3\.used=)[^\n]*
StringIndex=1
RegExpSubstitute=1
Substitute="^$":"0"

; Spend is a Calc, not the raw WebParser string: NumOfDecimals only applies to
; a numeric measure, so routing through here rounds "$6.4884" to "$6.49".
[mR3Spend]
Measure=Calc
Formula=[mR3Used]
DynamicVariables=1

[mR3Reset]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.3\.resets_at_in=)[^\n]*
StringIndex=1

[mR3Calc]
Measure=Calc
Formula=[mR3Pct]
DynamicVariables=1
MinValue=0
MaxValue=100
IfConditionMode=1
IfCondition=(#CURRENTSECTION# >= 90)
IfTrueAction=[!SetOption R3Bar BarColor #cRed#][!SetOption R3Num FontColor #cRed#]
IfCondition2=(#CURRENTSECTION# >= 70) && (#CURRENTSECTION# < 90)
IfTrueAction2=[!SetOption R3Bar BarColor #cAmber#][!SetOption R3Num FontColor #cAmber#]
IfCondition3=(#CURRENTSECTION# < 70)
IfTrueAction3=[!SetOption R3Bar BarColor #cGreen#][!SetOption R3Num FontColor #cText#]
IfCondition4=(#CURRENTSECTION# > 0) && (#CURRENTSECTION# < 1)
IfTrueAction4=[!SetOption R3Num Text "<1%"]
IfFalseAction4=[!SetOption R3Num Text "%1%"]
IfCondition5=(#CURRENTSECTION# < 0)
IfTrueAction5=[!SetOption R3Num Text "--"][!SetOption R3Num FontColor #cFaint#]

; Hides the whole row when the account has fewer than 4 record(s). The
; unit re-check after the group show re-hides the spend line on non-usd rows,
; which !ShowMeterGroup would otherwise re-show.
[mR3Vis]
Measure=Calc
Formula=[mCount]
DynamicVariables=1
IfConditionMode=1
IfCondition=(#CURRENTSECTION# > 3)
IfTrueAction=[!ShowMeterGroup R3][!UpdateMeasure mR3Unit]
IfFalseAction=[!HideMeterGroup R3]

; ---- row 4 ----
[mR4Label]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.4\.label=)[^\n]*
StringIndex=1

[mR4Pct]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.4\.pct=)[^\n]*
StringIndex=1
RegExpSubstitute=1
Substitute="^$":"-1"

[mR4Unit]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.4\.unit=)[^\n]*
StringIndex=1
IfMatchMode=1
IfMatch=^usd$
IfMatchAction=[!ShowMeter R4Spend]
IfNotMatchAction=[!HideMeter R4Spend]

[mR4Used]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.4\.used=)[^\n]*
StringIndex=1
RegExpSubstitute=1
Substitute="^$":"0"

; Spend is a Calc, not the raw WebParser string: NumOfDecimals only applies to
; a numeric measure, so routing through here rounds "$6.4884" to "$6.49".
[mR4Spend]
Measure=Calc
Formula=[mR4Used]
DynamicVariables=1

[mR4Reset]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.4\.resets_at_in=)[^\n]*
StringIndex=1

[mR4Calc]
Measure=Calc
Formula=[mR4Pct]
DynamicVariables=1
MinValue=0
MaxValue=100
IfConditionMode=1
IfCondition=(#CURRENTSECTION# >= 90)
IfTrueAction=[!SetOption R4Bar BarColor #cRed#][!SetOption R4Num FontColor #cRed#]
IfCondition2=(#CURRENTSECTION# >= 70) && (#CURRENTSECTION# < 90)
IfTrueAction2=[!SetOption R4Bar BarColor #cAmber#][!SetOption R4Num FontColor #cAmber#]
IfCondition3=(#CURRENTSECTION# < 70)
IfTrueAction3=[!SetOption R4Bar BarColor #cGreen#][!SetOption R4Num FontColor #cText#]
IfCondition4=(#CURRENTSECTION# > 0) && (#CURRENTSECTION# < 1)
IfTrueAction4=[!SetOption R4Num Text "<1%"]
IfFalseAction4=[!SetOption R4Num Text "%1%"]
IfCondition5=(#CURRENTSECTION# < 0)
IfTrueAction5=[!SetOption R4Num Text "--"][!SetOption R4Num FontColor #cFaint#]

; Hides the whole row when the account has fewer than 5 record(s). The
; unit re-check after the group show re-hides the spend line on non-usd rows,
; which !ShowMeterGroup would otherwise re-show.
[mR4Vis]
Measure=Calc
Formula=[mCount]
DynamicVariables=1
IfConditionMode=1
IfCondition=(#CURRENTSECTION# > 4)
IfTrueAction=[!ShowMeterGroup R4][!UpdateMeasure mR4Unit]
IfFalseAction=[!HideMeterGroup R4]

; ---- row 5 ----
[mR5Label]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.5\.label=)[^\n]*
StringIndex=1

[mR5Pct]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.5\.pct=)[^\n]*
StringIndex=1
RegExpSubstitute=1
Substitute="^$":"-1"

[mR5Unit]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.5\.unit=)[^\n]*
StringIndex=1
IfMatchMode=1
IfMatch=^usd$
IfMatchAction=[!ShowMeter R5Spend]
IfNotMatchAction=[!HideMeter R5Spend]

[mR5Used]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.5\.used=)[^\n]*
StringIndex=1
RegExpSubstitute=1
Substitute="^$":"0"

; Spend is a Calc, not the raw WebParser string: NumOfDecimals only applies to
; a numeric measure, so routing through here rounds "$6.4884" to "$6.49".
[mR5Spend]
Measure=Calc
Formula=[mR5Used]
DynamicVariables=1

[mR5Reset]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.5\.resets_at_in=)[^\n]*
StringIndex=1

[mR5Calc]
Measure=Calc
Formula=[mR5Pct]
DynamicVariables=1
MinValue=0
MaxValue=100
IfConditionMode=1
IfCondition=(#CURRENTSECTION# >= 90)
IfTrueAction=[!SetOption R5Bar BarColor #cRed#][!SetOption R5Num FontColor #cRed#]
IfCondition2=(#CURRENTSECTION# >= 70) && (#CURRENTSECTION# < 90)
IfTrueAction2=[!SetOption R5Bar BarColor #cAmber#][!SetOption R5Num FontColor #cAmber#]
IfCondition3=(#CURRENTSECTION# < 70)
IfTrueAction3=[!SetOption R5Bar BarColor #cGreen#][!SetOption R5Num FontColor #cText#]
IfCondition4=(#CURRENTSECTION# > 0) && (#CURRENTSECTION# < 1)
IfTrueAction4=[!SetOption R5Num Text "<1%"]
IfFalseAction4=[!SetOption R5Num Text "%1%"]
IfCondition5=(#CURRENTSECTION# < 0)
IfTrueAction5=[!SetOption R5Num Text "--"][!SetOption R5Num FontColor #cFaint#]

; Hides the whole row when the account has fewer than 6 record(s). The
; unit re-check after the group show re-hides the spend line on non-usd rows,
; which !ShowMeterGroup would otherwise re-show.
[mR5Vis]
Measure=Calc
Formula=[mCount]
DynamicVariables=1
IfConditionMode=1
IfCondition=(#CURRENTSECTION# > 5)
IfTrueAction=[!ShowMeterGroup R5][!UpdateMeasure mR5Unit]
IfFalseAction=[!HideMeterGroup R5]

; --------------------------------------------------------------------------
; Row meters
; --------------------------------------------------------------------------

[R0Label]
Meter=String
Group=R0
MeasureName=mR0Label
X=#PAD#
Y=(#RowTop# + 0 * #RowH#)
FontFace=#Face#
FontSize=10
FontColor=#cText#
AntiAlias=1
Text=%1

[R0Num]
Meter=String
Group=R0
MeasureName=mR0Calc
X=(#W# - #PAD#)
Y=((#RowTop# + 0 * #RowH#) - 3)
StringAlign=Right
FontFace=#Face#
FontSize=13
FontWeight=600
FontColor=#cText#
NumOfDecimals=0
AntiAlias=1
Text=%1%

[R0Spend]
Meter=String
Group=R0
MeasureName=mR0Spend
X=#PAD#
Y=((#RowTop# + 0 * #RowH#) + 17)
FontFace=#Face#
FontSize=8
FontColor=#cMuted#
NumOfDecimals=2
AntiAlias=1
Text=$%1

[R0Reset]
Meter=String
Group=R0
MeasureName=mR0Reset
X=(#W# - #PAD#)
Y=((#RowTop# + 0 * #RowH#) + 17)
StringAlign=Right
FontFace=#Face#
FontSize=8
FontColor=#cMuted#
AntiAlias=1
Text=%1

[R0Track]
Meter=Image
Group=R0
X=#PAD#
Y=((#RowTop# + 0 * #RowH#) + #BarInset#)
W=#BarW#
H=3
SolidColor=#cTrack#

[R0Bar]
Meter=Bar
Group=R0
MeasureName=mR0Calc
X=#PAD#
Y=((#RowTop# + 0 * #RowH#) + #BarInset#)
W=#BarW#
H=3
BarColor=#cGreen#
BarOrientation=Horizontal

[R1Label]
Meter=String
Group=R1
MeasureName=mR1Label
X=#PAD#
Y=(#RowTop# + 1 * #RowH#)
FontFace=#Face#
FontSize=10
FontColor=#cText#
AntiAlias=1
Text=%1

[R1Num]
Meter=String
Group=R1
MeasureName=mR1Calc
X=(#W# - #PAD#)
Y=((#RowTop# + 1 * #RowH#) - 3)
StringAlign=Right
FontFace=#Face#
FontSize=13
FontWeight=600
FontColor=#cText#
NumOfDecimals=0
AntiAlias=1
Text=%1%

[R1Spend]
Meter=String
Group=R1
MeasureName=mR1Spend
X=#PAD#
Y=((#RowTop# + 1 * #RowH#) + 17)
FontFace=#Face#
FontSize=8
FontColor=#cMuted#
NumOfDecimals=2
AntiAlias=1
Text=$%1

[R1Reset]
Meter=String
Group=R1
MeasureName=mR1Reset
X=(#W# - #PAD#)
Y=((#RowTop# + 1 * #RowH#) + 17)
StringAlign=Right
FontFace=#Face#
FontSize=8
FontColor=#cMuted#
AntiAlias=1
Text=%1

[R1Track]
Meter=Image
Group=R1
X=#PAD#
Y=((#RowTop# + 1 * #RowH#) + #BarInset#)
W=#BarW#
H=3
SolidColor=#cTrack#

[R1Bar]
Meter=Bar
Group=R1
MeasureName=mR1Calc
X=#PAD#
Y=((#RowTop# + 1 * #RowH#) + #BarInset#)
W=#BarW#
H=3
BarColor=#cGreen#
BarOrientation=Horizontal

[R2Label]
Meter=String
Group=R2
MeasureName=mR2Label
X=#PAD#
Y=(#RowTop# + 2 * #RowH#)
FontFace=#Face#
FontSize=10
FontColor=#cText#
AntiAlias=1
Text=%1

[R2Num]
Meter=String
Group=R2
MeasureName=mR2Calc
X=(#W# - #PAD#)
Y=((#RowTop# + 2 * #RowH#) - 3)
StringAlign=Right
FontFace=#Face#
FontSize=13
FontWeight=600
FontColor=#cText#
NumOfDecimals=0
AntiAlias=1
Text=%1%

[R2Spend]
Meter=String
Group=R2
MeasureName=mR2Spend
X=#PAD#
Y=((#RowTop# + 2 * #RowH#) + 17)
FontFace=#Face#
FontSize=8
FontColor=#cMuted#
NumOfDecimals=2
AntiAlias=1
Text=$%1

[R2Reset]
Meter=String
Group=R2
MeasureName=mR2Reset
X=(#W# - #PAD#)
Y=((#RowTop# + 2 * #RowH#) + 17)
StringAlign=Right
FontFace=#Face#
FontSize=8
FontColor=#cMuted#
AntiAlias=1
Text=%1

[R2Track]
Meter=Image
Group=R2
X=#PAD#
Y=((#RowTop# + 2 * #RowH#) + #BarInset#)
W=#BarW#
H=3
SolidColor=#cTrack#

[R2Bar]
Meter=Bar
Group=R2
MeasureName=mR2Calc
X=#PAD#
Y=((#RowTop# + 2 * #RowH#) + #BarInset#)
W=#BarW#
H=3
BarColor=#cGreen#
BarOrientation=Horizontal

[R3Label]
Meter=String
Group=R3
MeasureName=mR3Label
X=#PAD#
Y=(#RowTop# + 3 * #RowH#)
FontFace=#Face#
FontSize=10
FontColor=#cText#
AntiAlias=1
Text=%1

[R3Num]
Meter=String
Group=R3
MeasureName=mR3Calc
X=(#W# - #PAD#)
Y=((#RowTop# + 3 * #RowH#) - 3)
StringAlign=Right
FontFace=#Face#
FontSize=13
FontWeight=600
FontColor=#cText#
NumOfDecimals=0
AntiAlias=1
Text=%1%

[R3Spend]
Meter=String
Group=R3
MeasureName=mR3Spend
X=#PAD#
Y=((#RowTop# + 3 * #RowH#) + 17)
FontFace=#Face#
FontSize=8
FontColor=#cMuted#
NumOfDecimals=2
AntiAlias=1
Text=$%1

[R3Reset]
Meter=String
Group=R3
MeasureName=mR3Reset
X=(#W# - #PAD#)
Y=((#RowTop# + 3 * #RowH#) + 17)
StringAlign=Right
FontFace=#Face#
FontSize=8
FontColor=#cMuted#
AntiAlias=1
Text=%1

[R3Track]
Meter=Image
Group=R3
X=#PAD#
Y=((#RowTop# + 3 * #RowH#) + #BarInset#)
W=#BarW#
H=3
SolidColor=#cTrack#

[R3Bar]
Meter=Bar
Group=R3
MeasureName=mR3Calc
X=#PAD#
Y=((#RowTop# + 3 * #RowH#) + #BarInset#)
W=#BarW#
H=3
BarColor=#cGreen#
BarOrientation=Horizontal

[R4Label]
Meter=String
Group=R4
MeasureName=mR4Label
X=#PAD#
Y=(#RowTop# + 4 * #RowH#)
FontFace=#Face#
FontSize=10
FontColor=#cText#
AntiAlias=1
Text=%1

[R4Num]
Meter=String
Group=R4
MeasureName=mR4Calc
X=(#W# - #PAD#)
Y=((#RowTop# + 4 * #RowH#) - 3)
StringAlign=Right
FontFace=#Face#
FontSize=13
FontWeight=600
FontColor=#cText#
NumOfDecimals=0
AntiAlias=1
Text=%1%

[R4Spend]
Meter=String
Group=R4
MeasureName=mR4Spend
X=#PAD#
Y=((#RowTop# + 4 * #RowH#) + 17)
FontFace=#Face#
FontSize=8
FontColor=#cMuted#
NumOfDecimals=2
AntiAlias=1
Text=$%1

[R4Reset]
Meter=String
Group=R4
MeasureName=mR4Reset
X=(#W# - #PAD#)
Y=((#RowTop# + 4 * #RowH#) + 17)
StringAlign=Right
FontFace=#Face#
FontSize=8
FontColor=#cMuted#
AntiAlias=1
Text=%1

[R4Track]
Meter=Image
Group=R4
X=#PAD#
Y=((#RowTop# + 4 * #RowH#) + #BarInset#)
W=#BarW#
H=3
SolidColor=#cTrack#

[R4Bar]
Meter=Bar
Group=R4
MeasureName=mR4Calc
X=#PAD#
Y=((#RowTop# + 4 * #RowH#) + #BarInset#)
W=#BarW#
H=3
BarColor=#cGreen#
BarOrientation=Horizontal

[R5Label]
Meter=String
Group=R5
MeasureName=mR5Label
X=#PAD#
Y=(#RowTop# + 5 * #RowH#)
FontFace=#Face#
FontSize=10
FontColor=#cText#
AntiAlias=1
Text=%1

[R5Num]
Meter=String
Group=R5
MeasureName=mR5Calc
X=(#W# - #PAD#)
Y=((#RowTop# + 5 * #RowH#) - 3)
StringAlign=Right
FontFace=#Face#
FontSize=13
FontWeight=600
FontColor=#cText#
NumOfDecimals=0
AntiAlias=1
Text=%1%

[R5Spend]
Meter=String
Group=R5
MeasureName=mR5Spend
X=#PAD#
Y=((#RowTop# + 5 * #RowH#) + 17)
FontFace=#Face#
FontSize=8
FontColor=#cMuted#
NumOfDecimals=2
AntiAlias=1
Text=$%1

[R5Reset]
Meter=String
Group=R5
MeasureName=mR5Reset
X=(#W# - #PAD#)
Y=((#RowTop# + 5 * #RowH#) + 17)
StringAlign=Right
FontFace=#Face#
FontSize=8
FontColor=#cMuted#
AntiAlias=1
Text=%1

[R5Track]
Meter=Image
Group=R5
X=#PAD#
Y=((#RowTop# + 5 * #RowH#) + #BarInset#)
W=#BarW#
H=3
SolidColor=#cTrack#

[R5Bar]
Meter=Bar
Group=R5
MeasureName=mR5Calc
X=#PAD#
Y=((#RowTop# + 5 * #RowH#) + #BarInset#)
W=#BarW#
H=3
BarColor=#cGreen#
BarOrientation=Horizontal
