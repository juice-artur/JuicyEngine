@echo off

REM Format files in engine/src
CD ..\engine\src
if %ERRORLEVEL% neq 0 (
    echo ERROR: Could not change to directory ../engine/src
    exit /b 1
)
FOR /R %%f IN (*.cpp *.h) DO (
    clang-format -i "%%f"
    echo Formatted: "%%f"
)

REM Format files in testbed/src
CD ../../testbed/src
if %ERRORLEVEL% neq 0 (
    echo ERROR: Could not change to directory ../../testbed/src
    exit /b 1
)
FOR /R %%f IN (*.cpp *.h) DO (
    clang-format -i "%%f"
    echo Formatted: "%%f"
)

echo Done!
exit /b 0
