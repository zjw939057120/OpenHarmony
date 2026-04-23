set(CMAKE_SYSTEM_NAME Linux)

set(TOOLCHAIN_PATH /opt/BUILD_SDK/sysroots/x86_64-eulersdk-linux)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--allow-shlib-undefined")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--allow-shlib-undefined")

set(CMAKE_C_COMPILER   ${TOOLCHAIN_PATH}/BiSheng/wrapper/bin/clang)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/BiSheng/wrapper/bin/clang++)

message(STATUS "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")

set(CMAKE_SYSROOT /opt/BUILD_SDK/sysroots/aarch64-euler-elf/)

set(CMAKE_C_COMPILER_TARGET aarch64-linux-ohos)
set(CMAKE_ASM_COMPILER_TARGET aarch64-linux-ohos)
set(CMAKE_CXX_COMPILER_TARGET aarch64-linux-ohos)

# 设置链接器选项，但不包含不存在的库
set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,--dynamic-linker=/lib/hmld.so.elf")

# 禁用默认标准库链接（可选）
set(CMAKE_C_STANDARD_LIBRARIES "")
set(CMAKE_CXX_STANDARD_LIBRARIES "")

# 添加 include 路径
include_directories(/opt/BUILD_SDK/sysroots/aarch64-euler-elf/usr/local/include)
# 添加 link 路径
link_directories(/opt/BUILD_SDK/sysroots/aarch64-euler-elf/usr/local/lib)