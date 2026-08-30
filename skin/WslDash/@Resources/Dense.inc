; Dense.inc -- shared foundation for the merged Fumes-dense skin.
;
; The merged presentation of the fumes data: one panel, all accounts,
; one-line rows whose bar
; is a 2px hairline BENEATH the text (not the row background), hairline
; dividers between account sections, two-tone rows (white label, muted
; details, tier-coloured percentage), and each account's error drawn as a
; red-tinted strip. Roughly 19px per row and no per-account gaps.
;
; Included by Fumes-dense.ini after that skin's [Variables], which sets the
; A1..An account names the meter blocks in DenseRows.inc are keyed on. To
; override a value defined here (Port, W, Face, ...), put your key AFTER the
; @Include lines in your skin.
;
; The data layer is ONE WebParser parent plus ONE Lua script: with three
; accounts x six rows, one child measure per field would need ~100 children,
; and past roughly a dozen this Rainmeter build silently stops parsing them.
; dense.lua reads the parent's downloaded text and pushes values into the
; meters, so nothing else in the skin knows the flat format.
;
; DEPLOY CAVEAT: refreshing a skin with a Script measure can crash this
; build (4.5.26). After deploying, restart Rainmeter rather than !Refresh /
; !RefreshApp.
;
; Keep these files ASCII: Rainmeter reads .inc as ANSI unless there is a BOM,
; so a UTF-8 punctuation mark renders as mojibake. Row text uses "|" as its
; separator for the same reason.

[Variables]
; wsl-dash's port and producer. Windows reaches the WSL daemon through the
; localhost relay, which is why this is localhost and not the VM's IP.
Port=8781
Producer=fumes
; Seconds between fetches. Match or exceed the producer's interval in
; wsl-dash.toml -- polling faster only re-reads the same cache.
RefreshSeconds=120

; Geometry. One line per record: label + details + percentage on a 19px
; line, a 2px hairline track + fill BarOff below the line's top, then 2px of
; air. BarOff leaves the descenders (g, $, 5) a clear pixel above the line --
; at +14 the track reads as an underline of the text. SecPad is the air above
; and below each hairline divider (1px), so consecutive sections sit
; 2*SecPad+1 apart. PrefixH is the account section header; ErrH the error
; strip.
W=220
PAD=10
BarW=(#W# - 2 * #PAD#)
HeadH=24
RowH=19
PrefixH=12
SecPad=5
BarH=2
BarOff=15
; Fixed two-tone columns: the label clips at LabelW, details start at DetX
; and clip at DetW, the percentage right-aligns at W-PAD.
LabelW=84
DetX=(#PAD# + #LabelW# + 4)
DetW=(#W# - #DetX# - #PAD# - 30)
; Height of the error strip; ErrW insets it and ErrClipH gives its text a
; clip box.
ErrH=16
ErrW=(#BarW# - 13)
ErrClipH=14
Bottom=8
MaxRows=6

; Account section tops, recomputed by dense.lua from the live row counts;
; the values below are the load state before the first fetch.
Top1=29
Top2=106
Top3=165

; Font. Segoe UI ships with Windows; swap for Inter if you have it installed.
Face=Segoe UI

; Palette. The cLine* entries are the
; hairline-fill counterparts of cGreen/cAmber/cRed at line alpha -- the bar
; must stay subordinate to the text it belongs to. cErrBg/cErrEdge tint the
; error strip.
cPanel=24,26,32,248
cEdge=255,255,255,26
cRule=255,255,255,20
cText=232,234,240
cMuted=132,138,150
cFaint=96,102,114
cGreen=122,199,120
cAmber=232,181,86
cRed=228,105,105
cLineGreen=122,199,120,150
cLineAmber=232,181,86,165
cLineRed=228,105,105,180
cErrBg=228,105,105,22
cErrEdge=228,105,105,45
cTrack=255,255,255,18
Offline=0

; --------------------------------------------------------------------------
; Measures
; --------------------------------------------------------------------------

; The one download. dense.lua parses it; nothing else in the skin reads
; the flat format.
[mData]
Measure=WebParser
Url=http://localhost:#Port#/p/#Producer#.txt
RegExp=(?s)(.*)
StringIndex=1
UpdateRate=#RefreshSeconds#
; If the daemon is down or WSL is not running, the last download keeps its
; values -- stale numbers that still look current. Flip a flag instead so
; the header dot can say so; dense.lua colours the dot from it.
OnConnectErrorAction=[!SetVariable Offline 1][!UpdateMeasure mScript][!UpdateMeter *][!Redraw]
OnDownloadErrorAction=[!SetVariable Offline 1][!UpdateMeasure mScript][!UpdateMeter *][!Redraw]
FinishAction=[!SetVariable Offline 0][!UpdateMeasure mScript][!UpdateMeter *][!Redraw]

; The parser. Update() runs on the skin's one-second tick but re-parses only
; when the downloaded text has changed, so a quiet producer costs a string
; compare; the FinishAction above forces it the moment new data lands.
; ScriptFile, not LuaFile: this Rainmeter build renamed the option and
; silently ignores the old one (the file then reads as "not valid").
[mScript]
Measure=Script
ScriptFile=#@#dense.lua

; --------------------------------------------------------------------------
; Meters
; --------------------------------------------------------------------------

; The panel and header furniture live in DenseRows.inc, generated alongside
; the account blocks: dense.lua replaces the Panel shape, the Dot colour and
; the Age text as data lands, and every meter there may only reference
; measures and variables that already exist when it is created -- so the one
; meter that needs the script's numbers is defined in the same file that
; defines the script's account blocks. The panel is the first meter in that
; file, so it draws underneath the header and the account blocks.
