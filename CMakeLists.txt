cmake_minimum_required(VERSION 3.19)
project(spidev_test)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 99)

set(CMAKE_SYSTEM_NAME Linux)

add_executable(${PROJECT_NAME} spidev_test.c)
add_executable(sgm51620 sgm51620.c)
add_executable(sgm51242 sgm51242.c)