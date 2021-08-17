local rs232 = require("luars232")
local json = require("json")

espFs = {
  connected = false
}

function string.trim(s)
  return (string.gsub(s, "^%s*(.-)%s*$", "%1"))
end

local out = io.stderr

function espFs:connect(port)
  local e, p = rs232.open(port)
  if e ~= rs232.RS232_ERR_NOERROR then
    -- handle error
    out:write(string.format("can't open serial port '%s', error: '%s'\n", port, rs232.error_tostring(e)))
    return false
  end

  -- set port settings
  assert(p:set_baud_rate(rs232.RS232_BAUD_115200) == rs232.RS232_ERR_NOERROR)
  assert(p:set_data_bits(rs232.RS232_DATA_8) == rs232.RS232_ERR_NOERROR)
  assert(p:set_parity(rs232.RS232_PARITY_NONE) == rs232.RS232_ERR_NOERROR)
  assert(p:set_stop_bits(rs232.RS232_STOP_1) == rs232.RS232_ERR_NOERROR)
  assert(p:set_flow_control(rs232.RS232_FLOW_OFF)  == rs232.RS232_ERR_NOERROR)

  out:write(string.format("OK, port open with values '%s'\n", tostring(p)))
  self.port = p
  self.connected = true

  self.init()
  return true
end

function espFs:init()

end

function espFs:writeAll(data)
  bytesWritten = 1
  while #data > bytesWritten do
    err, len_written = self.port:write(data:sub(bytesWritten))
    assert(err == rs232.RS232_ERR_NOERROR)
    bytesWritten = bytesWritten + len_written
  end
end

function espFs:readAll()
  data = ""
  while true do
    local err, data_read, size = self.port:read(1000, 100) -- read 1k bytes and timeout after 100ms
    assert(err == rs232.RS232_ERR_NOERROR)
    if not data_read then break end
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
  return data
end

function espFs:print(data)
  self:writeAll(data .. "\r\n")
end

function espFs:close()
  self.connected = false
  assert(self.port:close() == rs232.RS232_ERR_NOERROR)
end

function espFs:getInfo()
  self:print(".fsinfo")
  local data = {}
  while true do
    local line = self:readLn()
    if not line then break end
    print(line)
    if line:sub(1,5) == "fs> {" then
      table.insert(data,json.decode(line:sub(5)))
    end
  end
  return data
end

function xCopy(localPath, remotePath)
  
end