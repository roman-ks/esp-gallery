# ESP Gallery

## Libs used
* https://github.com/littlefs-project/littlefs

## Arduino IDE plugins required
- https://github.com/earlephilhower/arduino-littlefs-upload


## Local tests setup
1. Install https://www.msys2.org/
2. Run in MSYS2
```
pacman -S mingw-w64-ucrt-x86_64-gcc
pacman -S mingw-w64-ucrt-x86_64-gdb
pacman -S mingw-w64-ucrt-x86_64-cmake
```
3. Run in this project:
```
cmake -S . -B build -G "MinGW Makefiles"
```

## Find exeption trace:
1. Find backtrace in logs, look for pairs of hex numbers eg 0x42003ec8:0x3fcebcc0
2. Go to C:\Users\<user>\AppData\Local\arduino\sketches\
3. Find and open the sketch folder, usually most recent one.
4. In command below replace elf file with your name in sketch ffolder. But backtrace in the end of the command and execute
```
C:\Users\<user>\.platformio\packages\toolchain-xtensa-esp32s3\bin\xtensa-esp32s3-elf-addr2line.exe -pf -e .\Read_User_Setup.ino.elf 0x42003ec8:0x3fcebcc0
```