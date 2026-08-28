; Compact.inc -- shared foundation for the merged Fumes-compact skin.
;
; The compact variant of the classic per-account skins: one panel, one header,
; all accounts as muted prefix lines with one-line rows beneath. Implements the
; "2+4+6" compression ideas -- one-line rows with the bar as row background,
; merged accounts, squeezed constants -- at roughly 20px per row instead of
; the classic 42px two-line rows.
;
; Included by Fumes-compact.ini after that skin's [Variables], which sets the
; A1..A3 account names the meter blocks in CompactRows.inc are keyed on.
;
; The data layer is ONE WebParser parent plus ONE Lua script: with three
; accounts x six rows, one child measure per field would need ~100 WebParser
; children, and past roughly a dozen this Rainmeter build silently stops
; parsing them. compact.lua reads the parent's downloaded text and pushes
; values into the meters, so nothing else in the skin knows the flat format.
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

; Geometry. One line per record: the bar is the row's background, the text
; sits on top of it. PrefixH is the muted account line above each account's
; rows; BlockGap separates one account's block from the next.
W=220
PAD=12
BarW=196
HeadH=32
RowH=20
PrefixH=13
BlockGap=6
; Height of the error line (the red rail + text) when an account fails.
ErrH=14
; The box the error text is drawn in, clipped to, and hovered over. See
; Common.inc in the classic skin for the full reasoning; the numbers here
; follow it.
ErrW=(#BarW# - 13)
ErrClipH=20
Bottom=10
MaxRows=6

; Account block tops, recomputed by compact.lua from the live row counts;
; the values below are the load state before the first fetch lands.
Top1=39
Top2=138
Top3=197

; Font. Segoe UI ships with Windows; swap for Inter if you have it installed.
Face=Segoe UI

; Palette. Same panel colour as the classic skins; the three cFill* entries
; are the row-background counterparts of cGreen/cAmber/cRed at row-fill alpha.
cPanel=24,26,32,248
cEdge=255,255,255,26
cRule=255,255,255,20
cText=232,234,240
cMuted=132,138,150
cFaint=96,102,114
cGreen=122,199,120
cAmber=232,181,86
cRed=228,105,105
cFillGreen=122,199,120,45
cFillAmber=232,181,86,45
cFillRed=228,105,105,50
cTrack=255,255,255,18
Offline=0

; --------------------------------------------------------------------------
; Measures
; --------------------------------------------------------------------------

; The one download. compact.lua parses it; nothing else in the skin reads
; the flat format.
[mData]
Measure=WebParser
Url=http://localhost:#Port#/p/#Producer#.txt
RegExp=(?s)(.*)
StringIndex=1
UpdateRate=#RefreshSeconds#
; If the daemon is down or WSL is not running, the last download keeps its
; values -- stale numbers that still look current. Flip a flag instead so
; the header dot can say so; compact.lua colours the dot from it.
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
ScriptFile=#@#compact.lua

; --------------------------------------------------------------------------
; Meters
; --------------------------------------------------------------------------

; The panel and header furniture live in CompactRows.inc, generated alongside
; the account blocks: compact.lua replaces the Panel shape, the Dot colour
; and the Age text as data lands, and every meter there may only reference
; measures and variables that already exist when it is created -- so the one
; meter that needs the script's numbers is defined in the same file that
; defines the script's account blocks. The panel is the first meter in that
; file, so it draws underneath the header and the account blocks.
