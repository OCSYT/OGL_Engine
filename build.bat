@echo off
setlocal enabledelayedexpansion

:: Ensure obj and output directories exist
if not exist "obj\" mkdir obj
if not exist "output\" mkdir output

:: Use Precompiled Headers (PCH) if applicable
if exist "include\pch.h" (
    echo Compiling precompiled header...
    clang++ -x c++-header -std=c++17 -I include "include\pch.h" -o "obj\pch.pch"
    if !ERRORLEVEL! neq 0 (
        echo Failed to compile precompiled header
        exit /b !ERRORLEVEL!
    )
)

:: Find all expected .o files from .cpp and .c files
set "EXPECTED_O_FILES="
for /r src %%f in (*.cpp *.c) do (
    set "REL_PATH=%%~pf"
    set "REL_PATH=!REL_PATH:~4!"
    set "FILENAME=%%~nf.o"
    set "EXPECTED_O_FILES=!EXPECTED_O_FILES! "obj\!REL_PATH!%%~nf.o""
)

:: Delete .o files that are no longer needed
for /r obj %%o in (*.o) do (
    set "OBJ_FILE=%%o"
    set "FOUND=0"
    
    for %%e in (!EXPECTED_O_FILES!) do (
        if /i "%%e"=="!OBJ_FILE!" set "FOUND=1"
    )

    if !FOUND! equ 0 (
        echo Removing obsolete object file: %%o
        del /q "%%o"
    )
)

:: Enable Parallel Compilation with a reasonable limit
set /a "JOBS=%NUMBER_OF_PROCESSORS%"
if %JOBS% gtr 8 set JOBS=8

:: Compile source files in parallel with dependency checking
echo Compiling source files...
for /r src %%f in (*.cpp *.c) do (
    set "REL_PATH=%%~pf"
    set "REL_PATH=!REL_PATH:~4!"
    if not exist "obj\!REL_PATH!" mkdir "obj\!REL_PATH!"
    set "OBJ_FILE=obj\!REL_PATH!%%~nf.o"
    set "SRC_FILE=%%f"
    set "COMPILE=1"
    
    :: Check if object file needs rebuilding
    if exist "!OBJ_FILE!" (
        for %%A in ("!OBJ_FILE!") do set "OBJ_TIME=%%~tA"
        for %%B in ("!SRC_FILE!") do set "SRC_TIME=%%~tB"
        :: Only compile if source is newer than object
        if "!SRC_TIME!" leq "!OBJ_TIME!" set "COMPILE=0"
    )
    
    if !COMPILE! equ 1 (
        echo Compiling %%f...
        start /b "" clang++ -I include -I src -c "!SRC_FILE!" -o "!OBJ_FILE!" -std=c++17 -w ^
            -O1 -pipe -march=native -fuse-ld=lld -g0 -pthread ^
            -Wno-deprecated -Wno-writable-strings
    )
)

:: Wait for all compilation jobs to complete
:waitloop
tasklist | find "clang++" >nul
if !ERRORLEVEL! equ 0 (
    timeout /t 1 >nul
    goto waitloop
)

:: Collect all object files recursively
set "ALL_O_FILES="
for /r obj %%o in (*.o) do (
    set "ALL_O_FILES=!ALL_O_FILES! "%%o""
)

:: Check if any object files were created
if "!ALL_O_FILES!"=="" (
    echo No object files were generated. Compilation may have failed.
    exit /b 1
)

:: Link object files into the final executable
echo Linking...
clang++ !ALL_O_FILES! -o "output\Output.exe" -L lib -lgdi32 -lglfw3dll -fuse-ld=lld -pthread
if !ERRORLEVEL! neq 0 (
    echo Linking failed
    exit /b !ERRORLEVEL!
)

echo Build successful.
exit /b 0