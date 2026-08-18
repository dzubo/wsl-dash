; Glue.inc -- keeps the stack together: this skin drags the one below it.
;
; Included after Common.inc (it reads that file's [mHeight]) and only by a skin
; that has another skin beneath it, which must set GlueNext first:
;
;   [Variables]
;   Account=opencode
;   Provider=opencode
;   GlueNext=WslDash\Fumes-claude
;
;   @Include3=#@#Glue.inc
;
; The last skin in the stack sets no GlueNext and does not include this file --
; !Move with an empty config argument would target the skin issuing the bang.
;
; The chain is one hop per skin rather than one skin placing all the others:
; a panel's height depends on how many rows its account has, and a skin can
; only read its own. So each skin parks its follower directly beneath itself
; and the follower does the same for the next one.
;
; Note this measures the panel, [mHeight], and not the window,
; #CURRENTCONFIGHEIGHT#. They are not the same: the window stays as tall as the
; full MaxRows block (292px as of this writing) because the hidden row meters
; still extend it, so the empty space below a short panel is a transparent part
; of the window. Stacking on the window height would leave that whole gap
; showing between panels.
;
; OnChangeAction, not OnUpdateAction: the move fires only when this skin's
; position or panel height actually changes (a drag, a row appearing), so a
; quiet second costs nothing and Rainmeter.ini is not rewritten once a second.
;
; The followers settle a beat after a refresh rather than instantly.
; OnChangeAction ignores the first change from "nothing" to a value, so the
; stack re-forms on the next real change -- which arrives on its own, since the
; panel resizes when the first fetch lands.
;
; Keep this file ASCII: Rainmeter reads .inc as ANSI unless there is a BOM.

[Variables]
; Pixels between one panel's bottom edge and the next panel's top. 0 stacks
; them flush into one continuous surface. To change it for one skin, set
; GlueGap after the @Include lines, not next to Account -- a later key wins.
GlueGap=5

; The follower's top edge: this panel's bottom, plus the gap. Defined before
; mGlueX because that measure reads it, and measures update in definition
; order.
[mGlueY]
Measure=Calc
Formula=(#CURRENTCONFIGY# + [mHeight] + #GlueGap#)
DynamicVariables=1
OnChangeAction=[!Move "#CURRENTCONFIGX#" "[mGlueY:0]" "#GlueNext#"]

; A second measure only so a sideways drag carries the follower too: one Calc
; watches one value, and the bang is the same either way.
[mGlueX]
Measure=Calc
Formula=#CURRENTCONFIGX#
DynamicVariables=1
OnChangeAction=[!Move "#CURRENTCONFIGX#" "[mGlueY:0]" "#GlueNext#"]
