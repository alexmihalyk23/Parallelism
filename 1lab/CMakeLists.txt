cmake_minimum_required(VERSION 3.14)
project(1lab)

set(CMAKE_CXX_STANDART 11)

option(double "set_double" OFF)

if(double)
        add_compile_definitions(DOUBLE)
        add_executable(main_double main.cpp)
else()
        add_executable(main_float main.cpp)
endif()
