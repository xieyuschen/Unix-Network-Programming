cmake_minimum_required(VERSION 3.16)
project(Part1)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_executable(client client.c)
add_executable(server server.c)

target_include_directories(client PRIVATE ${LIB_PATH})
target_include_directories(server PRIVATE ${LIB_PATH})

