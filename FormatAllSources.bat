@echo off
setlocal enabledelayedexpansion

REM Path to clang-format (edit if not in PATH)
set CLANG_FORMAT=clang-format

REM Root folder where .clang-format is located
set ROOT_DIR=D:\JuicyEngine

REM Source directories
set ENGINE_SRC=%ROOT_DIR%\JuicyEngine\Source
set SANDBOX_SRC=%ROOT_DIR%\Sandbox\Source

echo.
echo ==============================================
echo   Running clang-format on all source files
echo   Using config: %ROOT_DIR%\.clang-format
echo ==============================================
echo.

REM Format all .cpp and .h files recursively in both source dirs
for /r "%ENGINE_SRC%" %%f in (*.cpp *.h) do (
    echo Formatting: %%f
    "%CLANG_FORMAT%" -i -style=file "%%f"
)

for /r "%SANDBOX_SRC%" %%f in (*.cpp *.h) do (
    echo Formatting: %%f
    "%CLANG_FORMAT%" -i -style=file "%%f"
)

echo.
echo ==============================================
echo   Formatting completed!
echo ==============================================

endlocal
pause
