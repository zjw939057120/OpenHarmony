@echo off

set BUILD_DIR=%~dp0build\
set TOOLCHAIN=E:\SDK\native-windows-x64-5.1.0.107-Release\build\cmake\ohos.toolchain.cmake

if exist "%BUILD_DIR%CMakeCache.txt" del /q "%BUILD_DIR%CMakeCache.txt"
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"

cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE="%TOOLCHAIN%" -DOHOS_ARCH=armeabi-v7a ..
cmake --build .
