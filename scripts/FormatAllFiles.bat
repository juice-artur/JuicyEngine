@echo off
pushd ..\
CD JuicyEngine/src
FOR /R %%f IN (*.cpp, *.h, *.cs) DO (
    clang-format -i %%f
    echo %%f
)
popd
PAUSE
