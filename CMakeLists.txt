cmake_minimum_required(VERSION 3.16)
project(assembler_project C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -ansi -pedantic -g")
#source and head files
set(SRC_DIR "${CMAKE_SOURCE_DIR}/Source_Files")
set(HEADER_DIR "${CMAKE_SOURCE_DIR}/Header_Files")

#all c source files
file(GLOB SRC_FILES "${SRC_DIR}/*.c")

#add include directories search
include_directories(${HEADER_DIR})

#add the final executable
add_executable(assembler ${SRC_FILES}
        Header_files/instruction_memory.h
        Header_files/util.h
        source_files/sys_memory.c
        source_files/sys_memory.h)