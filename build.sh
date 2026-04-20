#!/bin/bash

#工具链下载地址
#https://repo.huaweicloud.com/harmonyos/os/4.1.4-Release/ohos-sdk-windows_linux-public.tar.gz
BUILD_DIR=$(pwd)/build/
rm -rf ${BUILD_DIR}
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

cmake -DCMAKE_TOOLCHAIN_FILE="./CrossCompile.cmake"  ..
make -j16
