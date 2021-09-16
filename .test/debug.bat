@echo off
cls
set addr2line="C:\Users\niklas\AppData\Local\Arduino15\packages\esp8266\tools\xtensa-lx106-elf-gcc\2.5.0-4-b40a506\bin\xtensa-lx106-elf-addr2line.exe"
REM set projectElf="C:\\Users\\niklas\\AppData\\Local\\Temp\\arduino_build_127878/uOS.ino.elf"
set projectElf="%CD%/build\uOS.ino.elf"
set dumpFile="dbg_stack.txt"
java -jar tools/EspStackTraceDecoder.jar %addr2line% %projectElf% %dumpFile%