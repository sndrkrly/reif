@echo off
IF EXIST build (
    echo delete build dir
    rmdir /s /q build
)

mkdir build
cd build

cmake .. -G "MinGW Makefiles"
cmake --build .

if EXIST reif.exe (
    start "" reif.exe
) else (
    echo something went wrong, fatal error
)

pause
