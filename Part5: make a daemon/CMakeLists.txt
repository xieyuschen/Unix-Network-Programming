cmake_minimum_required(VERSION 3.16)
project(Part5)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(LIB_PATH /home/user/.Project/unpv13e/lib/)
add_executable(process process.c)

target_include_directories(process PRIVATE ${LIB_PATH})

