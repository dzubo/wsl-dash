-- dense.lua -- the data layer for the merged Fumes-dense panel.
--
-- The merged panel keeps a single WebParser parent ([mData] in Dense.inc)
-- and this script does all the parsing: past roughly a dozen WebParser
-- children this Rainmeter build silently stops parsing them, so the script
-- reads the parent's downloaded text and pushes values into the meters with
-- bangs. One parser, no child measures.
--
-- What makes the dense panel dense: a row's bar is a 2px hairline beneath
--   the text, not the row background; the fill width therefore scales
--   against BarW (content width), not the panel width.
--   * rows are two-tone: white label, muted details, tier-coloured percentage
--   * account sections are separated by hairline dividers, shown only when
--     both neighbouring sections rendered something
--   * each account's prefix dot shows the account's worst tier (red on error)
--
-- Re-parses only when the downloaded text changes; the producer's own
-- adaptive schedule means most ticks are a string compare and a return.

-- Tuning knobs read from Dense.inc's [Variables] so the skin stays the
-- single source of truth. SKIN is not assigned until after the script file
-- loads, so nothing here touches it at load: Initialize() reads the
-- variables once; Update() re-reads the download when it changes.
local MAXROWS = 6
local PAD, W, BARW, HEADH, ROWH, PREFIXH, SECPAD, ERRH, BOTTOM
local ACCOUNTS = {}

-- Tier thresholds: green under 80, amber 80-94, red at 95 and up, on the
-- raw percentage (a display-rounded "80%" on a 79.x row stays green).
-- text = percentage glyphs, line = the hairline fill (translucent),
-- dot = the account prefix dot (solid).
local TIERS = {
  { max = 80,          text = 'cGreen', line = 'cLineGreen', dot = 'cGreen' },
  { max = 95,          text = 'cAmber', line = 'cLineAmber', dot = 'cAmber' },
  { max = math.huge,   text = 'cRed',   line = 'cLineRed',   dot = 'cRed' },
}

local lastData = nil

local function bang(s) SKIN:Bang(s) end

local function var(name) return SKIN:GetVariable(name) end

local function init()
  if ACCOUNTS[1] then return end
  PAD = tonumber(SKIN:GetVariable('PAD'))
  W = tonumber(SKIN:GetVariable('W'))
  BARW = tonumber(SKIN:GetVariable('BarW')) or (W - 2 * PAD)
  HEADH = tonumber(SKIN:GetVariable('HeadH'))
  ROWH = tonumber(SKIN:GetVariable('RowH'))
  PREFIXH = tonumber(SKIN:GetVariable('PrefixH'))
  SECPAD = tonumber(SKIN:GetVariable('SecPad'))
  ERRH = tonumber(SKIN:GetVariable('ErrH'))
  BOTTOM = tonumber(SKIN:GetVariable('Bottom'))
  ACCOUNTS = { SKIN:GetVariable('A1'), SKIN:GetVariable('A2'), SKIN:GetVariable('A3') }
end

function Initialize()
  init()
end

local function esc(s)
  -- Meters take text through bang options; quotes and control chars would
  -- break the bang, and fumes labels never legitimately carry them.
  return tostring(s):gsub('[%c"\\]', ' ')
end

-- A flat-form value: the line "\nkey=value". Dots are literal in the key.
local function field(data, key)
  local pat = '\n' .. key:gsub('(%W)', '%%%1') .. '=([^\n]*)'
  local v = data:match(pat)
  return v or ''
end

local function tierFor(pct)
  for _, t in ipairs(TIERS) do
    if pct < t.max then return t end
  end
  return TIERS[#TIERS]
end

local function setRow(i, k, label, det, pctText, textColor, lineColor, fillW)
  local m = string.format('A%dR%d', i, k)
  bang(string.format('!SetOption %s Text "%s"', m .. 'Text', esc(label)))
  bang(string.format('!SetOption %s Text "%s"', m .. 'Det', esc(det)))
  bang(string.format('!SetOption %s Text "%s"', m .. 'Pct', esc(pctText)))
  bang(string.format('!SetOption %s FontColor "%s"', m .. 'Pct', var(textColor)))
  bang(string.format('!SetOption %s W %d', m .. 'Fill', fillW))
  bang(string.format('!SetOption %s SolidColor "%s"', m .. 'Fill', var(lineColor)))
end

local function hideRow(i, k)
  setRow(i, k, '', '', '', 'cFaint', 'cTrack', 0)
  bang('!HideMeterGroup ' .. string.format('A%dR%d', i, k))
end

local function setPrefixDot(i, colorVar)
  bang(string.format('!SetOption A%dPDot Shape "Rectangle 0.5,0.5,4,4,1.5 | Fill Color %s"',
    i, var(colorVar)))
end

function Update()
  init()
  local parent = SKIN:GetMeasure('mData')
  local data = parent:GetStringValue()
  if data == lastData then return end
  lastData = data

  local offline = var('Offline') == '1'
  local ok = field(data, 'ok')
  local age = field(data, 'age')
  if age == '' then age = '-' end

  -- Header furniture: the dot is about the producer as a whole; the age is
  -- the daemon-side humanized staleness.
  if offline then
    bang('!SetOption Dot SolidColor "' .. var('cRed') .. '"')
  elseif ok == '0' or ok == '' then
    bang('!SetOption Dot SolidColor "' .. var('cAmber') .. '"')
  else
    bang('!SetOption Dot SolidColor "' .. var('cGreen') .. '"')
  end
  bang('!SetOption Age Text "' .. esc(age) .. '"')

  local tops = { HEADH + SECPAD, 0, 0 }
  local blockH = { 0, 0, 0 }
  local hasContent = { false, false, false }

  for i, acct in ipairs(ACCOUNTS) do
    local base = 'data.by_account.' .. acct
    local cnt = tonumber(field(data, base .. '.records.count')) or 0
    local errCnt = tonumber(field(data, base .. '.errors.count')) or 0
    local errMsg = field(data, base .. '.errors.0.message')
    local hasErr = errCnt > 0
    hasContent[i] = (cnt > 0) or hasErr

    -- Account section header: visible when the account produced anything.
    if hasContent[i] then
      bang('!ShowMeterGroup A' .. i .. 'Head')
    else
      bang('!HideMeterGroup A' .. i .. 'Head')
    end

    -- Hairline divider above the section: only when both neighbours render.
    if i > 1 and hasContent[i] and hasContent[i - 1] then
      bang('!ShowMeterGroup Div' .. i)
    else
      bang('!HideMeterGroup Div' .. i)
    end

    -- Error strip in place of rows; the full message rides the tooltip.
    if hasErr then
      bang(string.format('!SetOption A%dErrText Text "%s"', i, esc(errMsg)))
      bang(string.format('!SetOption A%dErrText ToolTipText "%s"', i, esc(errMsg)))
      bang('!ShowMeterGroup E' .. i)
      setPrefixDot(i, 'cRed')
    else
      bang('!HideMeterGroup E' .. i)
    end

    local rows = math.min(cnt, MAXROWS)
    local worstPct
    for k = 0, MAXROWS - 1 do
      if k < rows and not hasErr then
        local rk = base .. '.records.' .. k
        local label = field(data, rk .. '.label')
        local pctRaw = field(data, rk .. '.pct')
        local unit = field(data, rk .. '.unit')
        local used = tonumber(field(data, rk .. '.used')) or 0
        local reset = field(data, rk .. '.resets_at_in')

        -- Details column: "$spend | countdown". Spend only on usd rows; the
        -- countdown is dropped when empty; "|" because skin files are ASCII.
        local parts = {}
        if unit == 'usd' then
          table.insert(parts, string.format('$%.2f', used))
        end
        if reset ~= '' then table.insert(parts, reset) end
        local det = table.concat(parts, ' | ')

        -- Percentage text: honest forms (<1% rather than 0%, -- uncapped).
        local pct = tonumber(pctRaw)
        local pctText, tier, fillW
        if pct == nil then
          pctText, tier, fillW = '--', nil, 0
        else
          tier = tierFor(pct)
          worstPct = math.max(worstPct or 0, pct)
          if pct > 0 and pct < 1 then
            pctText = '<1%'
          else
            pctText = string.format('%d%%', math.floor(pct + 0.5))
          end
          fillW = math.floor(BARW * math.min(math.max(pct, 0), 100) / 100 + 0.5)
        end

        bang('!ShowMeterGroup A' .. i .. 'R' .. k)
        setRow(i, k, label, det, pctText,
          tier and tier.text or 'cFaint',
          tier and tier.line or 'cTrack',
          fillW)
      else
        hideRow(i, k)
      end
    end

    -- The section dot shows the account's worst row tier; an error is red.
    if not hasErr then
      if worstPct then
        setPrefixDot(i, tierFor(worstPct).dot)
      else
        setPrefixDot(i, 'cFaint')
      end
    end

    if hasContent[i] then
      blockH[i] = PREFIXH + (hasErr and ERRH or (rows * ROWH))
    else
      blockH[i] = 0
    end
    if i < #ACCOUNTS then
      tops[i + 1] = tops[i] + blockH[i] + (2 * SECPAD + 1)
    end
  end

  local lastBottom = HEADH + SECPAD
  for i = 1, #ACCOUNTS do
    lastBottom = math.max(lastBottom, tops[i] + blockH[i])
  end
  local height = lastBottom + BOTTOM
  local shape = string.format(
    'Rectangle 0.5,0.5,%d,%d,8 | Fill Color %s | StrokeWidth 1 | Stroke Color %s',
    W - 1, height - 1, var('cPanel'), var('cEdge'))
  bang('!SetOption Panel Shape "' .. shape .. '"')

  bang('!SetVariable Top1 ' .. tops[1])
  bang('!SetVariable Top2 ' .. tops[2])
  bang('!SetVariable Top3 ' .. tops[3])
  bang('!UpdateMeter *')
  bang('!Redraw')
end
