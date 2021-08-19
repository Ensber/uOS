sha1 = require("sha1")
require("espFs")
require("json")
require("lfs")

print("upload.lua")
--os.exit()

args = {...}
ARDUINO_AUTO = false
for _, arg in pairs(args) do
    if arg == "-arduino-auto" then ARDUINO_AUTO = true end
end

-- if ARDUINO_AUTO then os.exit() end

function hashFile(path)
    local f = io.open(path, "r")
    local hash = ""
    local data
    while true do
        data = f:read(10240)
        if not data then break end
        hash = sha1.sha1(hash..data)
    end
    f:close()
    return hash
end

files = {}
folders = {}
function getFilePaths(base, path)
    local hasFolders = false
    for file in lfs.dir(base) do
        if lfs.attributes(base.."/"..file,"mode") == "file" then
            -- print("found "..path.."/"..file)
            files[path.."/"..file] = hashFile(base.."/"..file)
        elseif file ~= "." and file ~= ".." and lfs.attributes(base.."/"..file,"mode") == "directory" then
            --print("found dir  '"..file.."'")
            hasFolders = true
            folders[path.."/"..file] = 1,
            getFilePaths(base.."/"..file, path.."/"..file)
        end
    end
end

-- generate file manifest
getFilePaths(lfs.currentdir().."/files", "")
folders["/sd"] = nil -- dont create a /sd folder. It either exists, or it doesn't.
manifest = {
    files = files,
    folders = folders
}
manifest.hash = sha1.sha1(json.encode(manifest))
manifest.createdAt = os.time()
-- print(json.encode(manifest))

-- open com
assert(espFs:connect("COM6"))

fsInfo = espFs:fsInfo()
hasSd = false
for i=1,#fsInfo do
    if fsInfo[i].name == "SD-Card" and fsInfo[i].attached then
        hasSd = true
    end
end

missingFiles = {}
missingFolders = {}

removeFiles = {} -- TODO
removeFolders = {} -- TODO

local espManifiest = {}

-- get manifest of esp
ok, err = espFs:xRead("/sd/manifest.json", "esp-manifest.json")
-- if it has no manifest, copy all files and create all folders
if not ok then
    missingFiles = manifest.files
    missingFolders = manifest.folders
else
    -- load manifet
    local f = io.open("esp-manifest.json","r")
    ok, espManifiest = pcall(json.decode,f:read("*all"))
    f:close()
    if ok then
        -- print("got ESP manifest")
        for file, hash in pairs(manifest.files) do
            if espManifiest.files[file] ~= hash then
                missingFiles[file] = hash
            end
        end
        for folder, num in pairs(manifest.folders) do
            if espManifiest.folders[folder] ~= hash then
                missingFolders[folder] = num
            end
        end
    else
        -- print("got no ESP manifest")
    end
end

-- only copy files if a file has changed
if espManifiest.hash ~= manifest.hash then
    print("\nManifest hash changed from "..espManifiest.hash.." to "..manifest.hash)
    print("Updating files...\n")
    for folder,_ in pairs(missingFolders) do 
        folder = folder .. "/"
        ok, msg = espFs:run(".mkdir \""..folder.."\"")
        if not ok or msg ~= "ok" then print("Error creatig folder '"..folder.."'. Error: "..msg) end
    end

    for file,_ in pairs(missingFiles) do 
        ok, err = espFs:xWrite("files"..file,file)
        if not ok then print("Error while copying file "..file.." to the ESP: "..err) end
    end

    -- writing manifest to esp
    local f = io.open("esp-manifest.json", "w")
    f:write(json.encode(manifest))
    f:close()
    ok, err = espFs:xWrite("esp-manifest.json", "/sd/manifest.json")
    if not ok then print("Error while copying the manifest to the ESP: "..err) end
end

--print(espFs:xWrite("espFs.lua", "/sd/espFs.lua"))
--print(espFs:xRead("/sd/espFs.lua", "espFs_copy.lua"))

espFs:close()