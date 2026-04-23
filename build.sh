#!/bin/bash

#工具链下载地址
#https://repo.huaweicloud.com/harmonyos/os/5.1.0-Release/ohos-sdk-windows_linux-public.tar.gz
#clang下载地址
#https://repo.huaweicloud.com/harmonyos/compiler/clang/15.0.4-283a25/linux/clang_linux-x86_64-283a25-20260403.tar.gz
BUILD_DIR=$(pwd)/build/
rm -rf ${BUILD_DIR}
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

#cmake ..
#cmake -DCMAKE_TOOLCHAIN_FILE="CrossCompile.cmake"  ..
#cmake -DCMAKE_TOOLCHAIN_FILE="/opt/native-linux-x64-4.1.10.5-Release/build/cmake/ohos.toolchain.cmake"  ..
cmake -DCMAKE_TOOLCHAIN_FILE="/opt/native-linux-x64-5.1.0.107-Release/build/cmake/ohos.toolchain.cmake" ..
make
