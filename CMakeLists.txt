cmake_minimum_required(VERSION 3.19)
project(untitled)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_SYSTEM_NAME Linux)

add_executable(untitled main.cpp)

target_link_options(untitled PRIVATE "-Wl,--dynamic-linker=/lib/hmld.so.elf")
