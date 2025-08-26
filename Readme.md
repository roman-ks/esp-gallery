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

