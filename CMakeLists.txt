cmake_minimum_required(VERSION 3.19)
project(FirstHopMobileOffloading)

set(CMAKE_CXX_STANDARD 14)


find_package(Boost 1.74.0 REQUIRED COMPONENTS filesystem)
include_directories(. ${Boost_INCLUDE_DIRS})

add_executable(FirstHopMobileOffloading
        main.cpp
        main.h)
