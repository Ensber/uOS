@echo off

REM upload the files from the storage/files folder
cd storage
lua upload.lua
cd ..