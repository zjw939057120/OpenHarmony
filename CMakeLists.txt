cmake_minimum_required(VERSION 3.19)
project(untitled)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_SYSTEM_NAME Linux)
# 设置链接器选项
set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,--dynamic-linker=/lib/hmld.so.elf")

add_executable(untitled main.cpp)
target_link_options(untitled PRIVATE -static-libstdc++)
