; Common.inc -- shared foundation for the per-account Fumes skins.
;
; Included by each Fumes-<account>/Fumes-<account>.ini after that skin's own
; [Variables], which sets Account and Provider. Everything here is
; account-agnostic: palette, fonts, geometry, the one download, and the
; per-block furniture (status dot, account name, age, hairline, panel shape,
; error line). The account-specific row blocks live in Rows.inc.
;
; This file's [Variables] is merged into the per-skin [Variables] at the point
; of the @Include line, and a later key wins a conflict. Account and Provider
; sit before the @Include lines and are not defined here, so they are never
; clobbered. To override a value defined here (RefreshSeconds, W, Face, ...),
; put your key AFTER the @Include lines in your skin, not next to Account.
;
; Keep these files ASCII: Rainmeter reads .inc as ANSI unless there is a BOM,
; so a UTF-8 punctuation mark renders as mojibake.

[Variables]
; wsl-dash's port and producer. Windows reaches the WSL daemon through the
; localhost relay, which is why this is localhost and not the VM's IP.
Port=8781
Producer=fumes
; Seconds between fetches. Match or exceed the producer's interval in
; wsl-dash.toml -- polling faster only re-reads the same cache.
RefreshSeconds=120

; Geometry. BarW = W - 2*PAD, so the bars inset by PAD on both sides.
W=220
PAD=16
BarW=188
; Vertical rhythm. The header ends at HeadH (the hairline); the first content
; line starts at RowTop; each line adds RowH; a row's bar sits BarInset below
; its line's top.
HeadH=40
RowTop=48
RowH=42
BarInset=34
; Height of the error line (the red rail + text) when an account fails.
ErrH=14
Bottom=15
; The most rows one account can occupy; extra rows are silently dropped (there
; is no way to signal the truncation from a skin). Bump this and copy more
; blocks in Rows.inc if an account ever grows past it -- the two must move
; together.
MaxRows=6

; Font. Segoe UI ships with Windows; swap for Inter if you have it installed.
Face=Segoe UI

; Palette. The panel ships near-opaque on purpose: a translucent panel over a
; light terminal or wallpaper lets text bleed through and reads as muddy.
;
; Note this is only half of the transparency. Rainmeter also applies a
; per-window AlphaValue, stored in Rainmeter.ini rather than here, and the two
; multiply. If a panel looks washed out, check AlphaValue before editing this.
cPanel=24,26,32,248
cEdge=255,255,255,26
cRule=255,255,255,20
cText=232,234,240
cMuted=132,138,150
cFaint=96,102,114
cGreen=122,199,120
cAmber=232,181,86
cRed=228,105,105
cTrack=255,255,255,18
Offline=0

; --------------------------------------------------------------------------
; Measures
; --------------------------------------------------------------------------

; The one download. Every other measure re-parses this measure's data, so each
; skin makes one HTTP request per refresh no matter how many fields it shows.
; Three skins therefore make three requests to a localhost daemon serving from
; cache -- accepted in the issue, and it does not matter.
[mData]
Measure=WebParser
Url=http://localhost:#Port#/p/#Producer#.txt
RegExp=(?s)(.*)
StringIndex=1
UpdateRate=#RefreshSeconds#
; If the daemon is down or WSL is not running, the child measures keep their
; last values -- stale numbers that still look current. Flip a flag instead so
; the header dot can say so.
OnConnectErrorAction=[!SetVariable Offline 1][!UpdateMeasure mStatus][!UpdateMeter *][!Redraw]
OnDownloadErrorAction=[!SetVariable Offline 1][!UpdateMeasure mStatus][!UpdateMeter *][!Redraw]
FinishAction=[!SetVariable Offline 0][!UpdateMeasure *][!UpdateMeter *][!Redraw]

[mOk]
Measure=WebParser
Url=[mData]
RegExp=(?<=\nok=)[^\n]*
StringIndex=1
RegExpSubstitute=1
Substitute="^$":"0"

[mAge]
Measure=WebParser
Url=[mData]
RegExp=(?<=\nage_seconds=)[^\n]*
StringIndex=1
RegExpSubstitute=1
Substitute="^$":"0"

; How many records this account has. Absent -- failed, or not configured --
; reads as 0. Reads the per-account index, not data.records.count, so one skin
; knows only its own rows.
[mCount]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.records\.count=)[^\n]*
StringIndex=1
RegExpSubstitute=1
Substitute="^$":"0"

; The account's own error, empty when it produced records. Served per-account
; (data.by_account.<acct>.errors.*) because a failed account leaves no records,
; and the positional data.errors.N.* keys cannot be matched to an account with
; a static regex.
[mErr]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.errors\.0\.message=)[^\n]*
StringIndex=1
RegExpSubstitute=1
; An HTTP failure is the common case; keep just the status so the line reads
; "HTTP 429" rather than a URL that never fits. Other messages pass through.
Substitute="^(HTTP \d+).*$":"\1"

; Header dot: offline beats producer error beats fine. An account-level failure
; (HTTP 429) is NOT reflected here -- that is the error line's red rail below;
; the dot stays about the producer as a whole.
[mStatus]
Measure=Calc
Formula=(#Offline# = 1) ? 2 : (([mOk] = 0) ? 1 : 0)
DynamicVariables=1
IfCondition=(#CURRENTSECTION# = 2)
IfTrueAction=[!SetOption Dot SolidColor #cRed#]
IfCondition2=(#CURRENTSECTION# = 1)
IfTrueAction2=[!SetOption Dot SolidColor #cAmber#]
IfCondition3=(#CURRENTSECTION# = 0)
IfTrueAction3=[!SetOption Dot SolidColor #cGreen#]

; True when this account failed (has an error). Drives the error line. The
; contract is one or the other: an account is expected to have either records
; or an error, never both (today's fumes raises before emitting both).
[mErrCount]
Measure=WebParser
Url=[mData]
RegExp=(?<=\ndata\.by_account\.#Account#\.errors\.count=)[^\n]*
StringIndex=1
RegExpSubstitute=1
Substitute="^$":"0"

[mHasErr]
Measure=Calc
Formula=([mErrCount] > 0) ? 1 : 0
DynamicVariables=1
IfConditionMode=1
IfCondition=(#CURRENTSECTION# = 1)
IfTrueAction=[!ShowMeterGroup Err]
IfFalseAction=[!HideMeterGroup Err]

; How many record rows to draw: up to MaxRows, or nothing for an account that
; is configured but has no rows.
[mLines]
Measure=Calc
Formula=Clamp([mCount], 0, #MaxRows#)
DynamicVariables=1

; The panel height follows. One Calc for the whole window, not one per row:
; each window holds exactly one account, so only its own height is dynamic.
; An error line is shorter than a full row, so it sizes itself rather than
; reusing the row formula.
[mHeight]
Measure=Calc
Formula=([mHasErr] = 1) ? (#RowTop# + #ErrH# + #Bottom#) : (([mLines] = 0) ? (#HeadH# + #Bottom#) : (#RowTop# + ([mLines] - 1) * #RowH# + #BarInset# + 3 + #Bottom#))
DynamicVariables=1

; "(provider)" shown only when it differs from the account name, matching
; fumes' _heading(). Done with a Substitute so there is no IfMatch first-update
; timing to get wrong: the meter simply reads "" or "(claude)".
[mProvider]
Measure=String
String=#Provider#
RegExpSubstitute=1
Substitute="^#Account#$":"","^(.+)$":"(\1)"

; --------------------------------------------------------------------------
; Meters
; --------------------------------------------------------------------------

; The panel itself. A Shape meter for rounded corners and a hairline edge; its
; height tracks [mHeight], the one dynamic number in the block.
[Panel]
Meter=Shape
X=0
Y=0
DynamicVariables=1
Shape=Rectangle 0.5,0.5,(#W# - 1),([mHeight] - 1),10 | Fill Color #cPanel# | StrokeWidth 1 | Stroke Color #cEdge#

[Dot]
Meter=Image
X=#PAD#
Y=22
W=6
H=6
SolidColor=#cGreen#

[Title]
Meter=String
X=10R
Y=17
FontFace=#Face#
FontSize=10
FontWeight=600
FontColor=#cMuted#
StringCase=Upper
AntiAlias=1
Text=#Account#

[Provider]
Meter=String
X=4R
Y=20
FontFace=#Face#
FontSize=8
FontColor=#cFaint#
AntiAlias=1
MeasureName=mProvider
Text=%1

[Age]
Meter=String
X=(#W# - #PAD#)
Y=19
StringAlign=Right
FontFace=#Face#
FontSize=8
FontWeight=600
FontColor=#cMuted#
AntiAlias=1
MeasureName=mAge
Text=%1s

[Rule]
Meter=Image
X=#PAD#
Y=#HeadH#
W=#BarW#
H=1
SolidColor=#cRule#

; The error line: a red rail and the account's error (e.g. "HTTP 429"). Shown
; by [mHasErr] only when the account failed. This is the one new colour, and it
; is status used as status, not identity.
[ErrRail]
Meter=Image
Group=Err
X=#PAD#
Y=#RowTop#
W=3
H=#ErrH#
SolidColor=#cRed#

[ErrText]
Meter=String
Group=Err
X=10R
Y=(#RowTop# - 1)
FontFace=#Face#
FontSize=10
FontColor=#cRed#
AntiAlias=1
MeasureName=mErr
Text=%1
