cmake_minimum_required(VERSION 3.15)
project(xinOS C)

set(CMAKE_C_STANDARD 99)

add_executable(xinOS src/kernel/kernel.c)
target_include_directories(xinOS PRIVATE src)