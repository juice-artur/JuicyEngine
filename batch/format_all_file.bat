@echo off

REM Format files in engine/src
CD ..\src
if %ERRORLEVEL% neq 0 (
    echo ERROR: Could not change to directory ../src
    exit /b 1
)
FOR /R %%f IN (*.cpp *.h) DO (
    clang-format -i "%%f"
    echo Formatted: "%%f"
)

echo Done!
exit /b 0
