cmake_minimum_required(VERSION 3.10.2)
project(src)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

include_directories(
        ${PROJECT_SOURCE_DIR}/src/
)
AUX_SOURCE_DIRECTORY(src DIR_SRCS)
add_executable(code main.cpp UnrolledLinkedList.hpp BookStore.h BookStore.cpp programm.h DiskManager.hpp functions.hpp Myexception.hpp BPlusTree.hpp)