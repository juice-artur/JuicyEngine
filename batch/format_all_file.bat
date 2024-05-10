@echo off
CD ../src
FOR /R %%f IN (*.cpp, *.h) DO (
    clang-format -i %%f
    echo %%f
)