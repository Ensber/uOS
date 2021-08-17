#include "fileSystem.hpp"

fileSystem::realFSLink fileSystem::getRealPath(String path) {
    fileSystem::realFSLink rp;
    if (path[0] != '/')
        path = '/' + path;
    rp.internal = this->sdCardAttached && path.indexOf(this->sdPath) != 0;
    if (rp.internal) {
        rp.path = path;
    } else {
        rp.path = path.substring(this->sdPath.length()-1);
    }
    if (rp.path[0] == '/')
    return rp;
}

bool fileSystem::attachSD(String path, int cs) {
    if (!this->sdCardAttached && SD.begin(cs)) {
        if (path[0] != '/')
            path = '/' + path;
        if (path[path.length()-1] != '/')
            path += '/';
        this->sdPath = path;
        this->sdCardAttached = true;
        return true;
    }
    return false;
}

bool fileSystem::detatchSD() {
    if (this->sdCardAttached) {
        SD.end();
        this->sdPath = "";
        this->sdCardAttached = false;
        return true;
    }
    return false;
}

bool fileSystem::beginInternalFs() {
    this->internalFsAttached = LittleFS.begin();
    return this->internalFsAttached;
}

fileSystem::fileResponse fileSystem::open(String path, String mode) {
    fileResponse outFile;

    if (this->fileOpen) {
        outFile.ok = false;
        return outFile;
    }

    auto rp = this->getRealPath(path);
    if (rp.internal) {
        outFile.file = LittleFS.open(rp.path, mode.c_str());
    } else {
        outFile.file = SD.open(rp.path, mode.c_str());
    }
    outFile.ok = outFile.file;

    return outFile;
}

void fileSystem::close(File file) {
    this->fileOpen = false;
    file.close();
}

void fileSystem::close(fileSystem::fileResponse fr) {
    this->close(fr.file);
}

bool fileSystem::mkDir(String path) {
    auto rp = getRealPath(path);
    if (rp.internal) {
        return LittleFS.mkdir(rp.path);
    } else {
        return SD.mkdir(rp.path);
    }
}

bool fileSystem::rmDir(String path) {
    auto rp = getRealPath(path);
    if (rp.internal) {
        return LittleFS.rmdir(rp.path);
    } else {
        return SD.rmdir(rp.path);
    }
}

std::vector<String> fileSystem::lsDir(String path, byte mode) {
    std::vector<String> outCollection;
    outCollection.reserve(10);

    bool searchFiles   = (mode & fileSystem::LIST_FILES) > 0;
    bool searchFolders = (mode & fileSystem::LIST_DRECTORIES) > 0;

    auto f = this->open(path, "r");
    if (!f.ok) return outCollection;

    while (true) {
        File entry = f.file.openNextFile();
        if (!entry) break;
        bool isDirectory = entry.isDirectory();
        if (searchFiles && !isDirectory || searchFolders && isDirectory)
            outCollection.push_back(entry.name());
    }

    return outCollection;
}