cmake_minimum_required(VERSION 3.26)
project(EdgeBraker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(./src)

file(GLOB_RECURSE SRC_FILES src/*.cpp)

add_executable(EdgeBraker ${SRC_FILES}
        src/HEPolMesh.cpp
        src/EBBinStreamIO.h
        src/EBBinStreamIO.cpp)
