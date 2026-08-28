-- compact.lua -- the whole data layer for the merged Fumes-compact panel.
--
-- The classic skins use one WebParser child measure per field; with three
-- accounts x six rows that would be ~100 children, and past roughly a dozen
-- this Rainmeter build silently stops parsing them (values come back empty
-- with no log line). So the compact skin keeps a single WebParser parent
-- ([mData] in Compact.inc) and this script does all the parsing itself,
-- reading the parent's downloaded text and pushing values into the meters
-- with bangs. One parser, no child measures.
--
-- Re-parses only when the downloaded text changes; the producer's own
-- adaptive schedule means most ticks are a string compare and a return.

-- Tuning knobs read from Compact.inc's [Variables] so the skin stays the
-- single source of truth. SKIN is not assigned until after the script file
-- loads, so nothing here touches it at load: Initialize() reads the
-- variables once; Update() re-reads the download when it changes.
local MAXROWS = 6
local PAD, W, BARW, HEADH, ROWH, PREFIXH, BLOCKGAP, ERRH, BOTTOM
local ACCOUNTS = {}

-- Tier palette: green under 80, amber 80-94, red at 95 and up. The fill
-- colours carry the row-background alpha; text stays white in the green tier
-- exactly like the classic skin.
local TIERS = {
  { max = 80, fill = 'cFillGreen', text = 'cText' },
  { max = 95, fill = 'cFillAmber', text = 'cAmber' },
  { max = math.huge, fill = 'cFillRed', text = 'cRed' },
}

local lastData = nil

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
  BLOCKGAP = tonumber(SKIN:GetVariable('BlockGap'))
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

local function setRow(i, k, line, pctText, fillColor, textColor, fillW)
  local m = string.format('A%dR%d', i, k)
  bang(string.format('!SetOption %s Text "%s"', m .. 'Text', esc(line)))
  bang(string.format('!SetOption %s Text "%s"', m .. 'Pct', esc(pctText)))
  bang(string.format('!SetOption %s FontColor "%s"', m .. 'Pct', var(textColor)))
  bang(string.format('!SetOption %s W %d', m .. 'Fill', fillW))
  bang(string.format('!SetOption %s SolidColor "%s"', m .. 'Fill', var(fillColor)))
end

local function hideRow(i, k)
  local m = string.format('A%dR%d', i, k)
  setRow(i, k, '', '', 'cTrack', 'cText', 0)
  bang('!HideMeterGroup ' .. m)
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

  local tops = { HEADH + 7, 0, 0 }
  local blockH = { 0, 0, 0 }

  for i, acct in ipairs(ACCOUNTS) do
    local base = 'data.by_account.' .. acct
    local cnt = tonumber(field(data, base .. '.records.count')) or 0
    local errCnt = tonumber(field(data, base .. '.errors.count')) or 0
    local errMsg = field(data, base .. '.errors.0.message')
    local hasErr = errCnt > 0

    -- Account prefix line: visible when the account produced anything.
    if cnt > 0 or hasErr then
      bang('!ShowMeterGroup A' .. i .. 'Head')
    else
      bang('!HideMeterGroup A' .. i .. 'Head')
    end

    -- Error line in place of rows; the full message rides the tooltip.
    if hasErr then
      bang(string.format('!SetOption A%dErrText Text "%s"', i, esc(errMsg)))
      bang(string.format('!SetOption A%dErrText ToolTipText "%s"', i, esc(errMsg)))
      bang('!ShowMeterGroup E' .. i)
    else
      bang('!HideMeterGroup E' .. i)
    end

    local rows = math.min(cnt, MAXROWS)
    for k = 0, MAXROWS - 1 do
      if k < rows and not hasErr then
        local rk = base .. '.records.' .. k
        local label = field(data, rk .. '.label')
        local pctRaw = field(data, rk .. '.pct')
        local unit = field(data, rk .. '.unit')
        local used = tonumber(field(data, rk .. '.used')) or 0
        local reset = field(data, rk .. '.resets_at_in')

        -- One-line row: "label | $spend | countdown". Spend only on usd
        -- rows; the countdown is dropped when empty; "|" because the skin
        -- files must stay ASCII.
        local parts = { label }
        if unit == 'usd' then
          table.insert(parts, string.format('$%.2f', used))
        end
        if reset ~= '' then table.insert(parts, reset) end
        local line = table.concat(parts, ' | ')

        -- Percentage text: honest forms, as in the classic skin.
        local pct = tonumber(pctRaw)
        local pctText, fillColor, textColor, fillW
        if pct == nil then
          pctText, fillColor, textColor, fillW = '--', 'cTrack', 'cFaint', 0
        else
          local tier = tierFor(pct)
          fillColor = tier.fill
          textColor = tier.text
          if pct > 0 and pct < 1 then
            pctText = '<1%'
          else
            pctText = string.format('%d%%', math.floor(pct + 0.5))
          end
          fillW = math.floor(W * math.min(math.max(pct, 0), 100) / 100 + 0.5)
        end

        bang('!ShowMeterGroup A' .. i .. 'R' .. k)
        setRow(i, k, line, pctText, fillColor, textColor, fillW)
      else
        hideRow(i, k)
      end
    end

    blockH[i] = PREFIXH + (hasErr and ERRH or (rows * ROWH))
    if i < #ACCOUNTS then
      tops[i + 1] = tops[i] + blockH[i] + BLOCKGAP
    end
  end

  local height = tops[#ACCOUNTS] + blockH[#ACCOUNTS] + BOTTOM
  local shape = string.format(
    'Rectangle 0.5,0.5,%d,%d,10 | Fill Color %s | StrokeWidth 1 | Stroke Color %s',
    W - 1, height - 1, var('cPanel'), var('cEdge'))
  bang('!SetOption Panel Shape "' .. shape .. '"')

  bang('!SetVariable Top1 ' .. tops[1])
  bang('!SetVariable Top2 ' .. tops[2])
  bang('!SetVariable Top3 ' .. tops[3])
  bang('!UpdateMeter *')
  bang('!Redraw')
end
