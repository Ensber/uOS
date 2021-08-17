json = require("json")
require("espFs")

print("upload.lua")
--os.exit()

assert(espFs:connect("COM6"))

espFs:writeAll(".file\n")
for i=1,5 do
    print(i, "'"..tostring(espFs:readLn()).."'")
end

print(json.encode(espFs:getInfo()))

espFs:close()