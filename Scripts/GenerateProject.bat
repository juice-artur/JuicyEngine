@echo off
setlocal enabledelayedexpansion

REM ------------------------------------------
REM Set root directory
REM ------------------------------------------
set "ROOT_DIR=%~dp0.."

REM ------------------------------------------
REM Create build directory if it doesn't exist
REM ------------------------------------------
if not exist "%ROOT_DIR%\build" (
    mkdir "%ROOT_DIR%\build"
)

cd "%ROOT_DIR%\build" || (
    echo Failed to change directory to build
    pause
    exit /b 1
)

REM ------------------------------------------
REM Run CMake configuration
REM ------------------------------------------
echo Running CMake configuration...
cmake .. || (
    echo CMake configuration failed!
    pause
    exit /b 1
)

echo ------------------------------------------
echo Project generation completed successfully.
echo ------------------------------------------

endlocal
pause
