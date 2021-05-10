cmake_minimum_required(VERSION 3.16)
project(Part8)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(INCLUDE_PATH /home/user/.Project/unpv13e/lib/)
set(LIB_PATH /home/user/.Project/unpv13e/)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
add_executable(client client.c)
add_executable(server server.c)

target_link_libraries(client PUBLIC 
    Threads::Threads
    ${LIB_PATH}/libunp.a
    )
target_include_directories(client PRIVATE 
    ${INCLUDE_PATH} 
)
    
target_link_libraries(server PUBLIC 
    Threads::Threads
    ${LIB_PATH}/libunp.a
)
target_include_directories(server PRIVATE 
    ${INCLUDE_PATH} 
)

