@echo off
setlocal enabledelayedexpansion

:: Collect all .cpp files in src and its subdirectories
set "SRC_FILES="
for /r src %%f in (*.cpp *.c) do (
    set "SRC_FILES=!SRC_FILES! %%f"
)

:: Compile the project
g++ -I include -I src %SRC_FILES% -o output/Output.exe -std=c++17 -Wall -Wextra -L lib -lgdi32 -lglfw3dll

if %ERRORLEVEL% neq 0 (
    echo Build failed.
    exit /b %ERRORLEVEL%
)

echo Build successful.
