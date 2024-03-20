cmake_minimum_required(VERSION 3.22)
project(Parallels_C__)

add_definitions(-DSIZE=1000)
add_definitions(-DCOUNT=8)
add_definitions(-DK=100)
add_definitions(-DPARAM=guided)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")

set(CMAKE_CXX_STANDARD 17)

add_executable(Parallels_C__ lin.cpp)
