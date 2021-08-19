local rs232 = require("luars232")
local json = require("json")

espFs = {
  connected = false,
  debug = false
}

function string.trim(s)
  return (string.gsub(s, "^%s*(.-)%s*$", "%1"))
end

function espFs:connect(port)
  local e, p = rs232.open(port)
  if e ~= rs232.RS232_ERR_NOERROR then
    -- handle error
    print(string.format("can't open serial port '%s', error: '%s'", port, rs232.error_tostring(e)))
    return false
  end

  -- set port settings
  assert(p:set_baud_rate(rs232.RS232_BAUD_115200) == rs232.RS232_ERR_NOERROR)
  assert(p:set_data_bits(rs232.RS232_DATA_8) == rs232.RS232_ERR_NOERROR)
  assert(p:set_parity(rs232.RS232_PARITY_NONE) == rs232.RS232_ERR_NOERROR)
  assert(p:set_stop_bits(rs232.RS232_STOP_1) == rs232.RS232_ERR_NOERROR)
  assert(p:set_flow_control(rs232.RS232_FLOW_OFF)  == rs232.RS232_ERR_NOERROR)

  if self.debug then print(string.format("OK, port open with values '%s'", tostring(p))) end
  self.port = p
  self.connected = true

  self:init()
  return true
end

function espFs:init()
  self:print("fs i")
  local initialized = false
  for i=1,3 do
    line = self:readLn() or ""
    if line:find("fs") then initialized = true end
  end
  if not initialized then
    print("'fs i' command propably did not initialize correctly!")
  end
end

function espFs:writeAll(data)
  bytesWritten = 1
  while #data > bytesWritten do
    err, len_written = self.port:write(data:sub(bytesWritten))
    if err ~= rs232.RS232_ERR_NOERROR then return false, err end
    if self.debug then print("#WRITTEN", string.sub(data, bytesWritten, len_written)) end
    bytesWritten = bytesWritten + len_written
  end
  return true, 0
end

function espFs:readAll()
  data = ""
  while true do
    local err, data_read, size = self.port:read(1024, 100) -- read 1k bytes and timeout after 100ms
    assert(err == rs232.RS232_ERR_NOERROR)
    if not data_read then break end
    if self.debug then print("#RECV", data_read) end
    data = data .. data_read
  end
  return data
end

function espFs:readLn()
  data = ""
  timeoutCounter = 0
  while true do
    local err, data_read, size = self.port:read(1, 100) -- read 1 byte and timeout after 100ms
    assert(err == rs232.RS232_ERR_NOERROR)
    if data_read == "\n" then break end
    if not data_read then
      timeoutCounter = timeoutCounter + 1
      if timeoutCounter == 2 then break end
    else
      data = data .. data_read
    end
  end
  data = data:gsub("\r","")
  if data == "" then data = nil end
  if self.debug then print("#RECV", data) end
  return data
end

function espFs:print(data)
  self:writeAll(data .. "\r\n")
end

function espFs:close()
  self:print(".exit")
  for i=1,3 do
    self:readLn() -- clear last lines
  end
  self.connected = false
  assert(self.port:close() == rs232.RS232_ERR_NOERROR)
end

function espFs:fsInfo()
  self:print(".fsinfo")
  local data = {}
  while true do
    local line = self:readLn()
    if not line then break end
    if line:sub(1,5) == "fs> {" then
      table.insert(data,json.decode(line:sub(5)))
    end
  end
  return data
end

function espFs:run(command)
  self:print(command)
  local out = {
    ok = false;
  }
  local lines = 0
  local line

  -- try 10 times
  for i=1,10 do
    line = self:readLn()
    if line then
      lines = lines + 1
      if lines == 2 then
        local lineRes = {line:find("fs> (.*) %(state (.*)/(.*)%)")}
        return true, line:sub(5), lineRes[3], lineRes[4], lineRes[5]
      end
    end
  end
  return false, ""
end

function espFs:readLnPattern(pattern)
  local line
  -- try 3 times
  for i=1,3 do
    line = self:readLn()
    if line then break end
  end
  if not line then return false end
  local filter = {line:find(pattern)}
  local found = table.remove( filter, 1 )
  table.remove( filter, 1 )
  return found ~= nil, unpack(filter)
end

function espFs:xWrite(localPath, remotePath)
  print("copying '"..localPath.."' to 'ESP:/"..remotePath.."'")
  local file = io.open(localPath, "rb")
  local cPos = file:seek()
  local fSize = file:seek("end")
  file:seek("set", cPos)
  local bytesWritten = 0

  local ok, raw, response = self:run(".open \""..remotePath.."\" w")
  if not ok or response ~= "ok" then return false, raw end

  writeStart = os.clock()
  while true do
    local lFileData = file:read(255) -- read 255 Bytes (max bytes sendable via serial)
    if not lFileData then break end
    ok, raw, response = self:run(".w "..#lFileData)
    if not ok or response ~= "ok" then return false, raw end
    ok, err = self:writeAll(lFileData)
    if not ok then return false, err end

    bytesWritten = bytesWritten + #lFileData
    local progress = bytesWritten/fSize
    local bar = ""
    local barWidth = 30
    local lastActive = true
    for i=1,barWidth do
      if progress > (i/barWidth) then
        bar = bar .. "="
      else
        if lastActive then
          lastActive = false
          bar = bar .. ">"
        else
          bar = bar .. " "
        end
      end
    end

    io.write("\r<<< Writing... ["..bar.."] "..bytesWritten.."/"..fSize.." ("..(math.floor(progress*10000)/100).."%) ")
    if progress ~= 1 then
      io.write("eta: "..(math.floor((os.clock()-writeStart)/bytesWritten*(fSize-bytesWritten)*10)/10).."s ")
    end

    -- 3 attempts
    s = os.clock()
    for i=1,3 do
      if self.debug then print("waiting for confirmation...") end
      ok, bytes = self:readLnPattern("fs> Wrote (.*) bytes %(state 2/write file%)")
      if self.debug then print("wrote "..bytes.." bytes") end
      if ok then break end
    end
  end
  print("Transmitted @ "..(math.floor(fSize/(os.clock()-writeStart)*8/10.24)/100).."kbit/s")

  local ok, raw, response = self:run(".close")
  if not ok or response ~= "ok" then return false, raw end

  file:close()

  return true, ""
end

function espFs:xRead(remotePath, localPath)
  print("copying 'ESP:/"..remotePath.."' to '"..localPath.."'")
  local file = io.open(localPath, "wb")

  local ok, raw, response = self:run(".open \""..remotePath.."\" r")
  if not ok or response ~= "ok" then return false, raw end

  local ok, raw, response = self:run(".read")
  _,_, bytes = raw:find("reading (.*) Bytes")
  if not ok or not bytes then return false, raw end
  local fSize = tonumber(bytes)

  bytesToRead = fSize
  readStart = os.clock()
  while true do
    local err, data_read, size = self.port:read(math.min(1024, bytesToRead), 50) -- read 1k byte or timeout after 100ms
    if self.debug then print("#RECV", data_read) end
    assert(err == rs232.RS232_ERR_NOERROR)
    file:write(data_read or "nil")
    bytesToRead = bytesToRead - size

    local bytesRead = fSize - bytesToRead
    local progress = bytesRead/fSize
    local bar = ""
    local barWidth = 30
    local lastActive = true
    for i=1,barWidth do
      if progress > (i/barWidth) then
        bar = bar .. "="
      else
        if lastActive then
          lastActive = false
          bar = bar .. ">"
        else
          bar = bar .. " "
        end
      end
    end

    io.write("\r>>> Reading... ["..bar.."] "..bytesRead.."/"..fSize.." ("..(math.floor(progress*10000)/100).."%) ")
    if progress ~= 1 then
      io.write("eta: "..(math.floor((os.clock()-readStart)/bytesRead*(fSize-bytesRead)*10)/10).."s ")
    end

    if bytesToRead == 0 then break end
  end
  
  print("Transmitted @ "..(math.floor(fSize/(os.clock()-readStart)*8/10.24)/100).."kbit/s")

  local ok, raw, response = self:run(".close")
  if not ok or response ~= "ok" then return false, raw end

  file:close()
  return true
end