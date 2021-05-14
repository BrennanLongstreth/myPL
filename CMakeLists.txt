######################################################
# Name: Shawn Bowers
# File: CMakeLists.txt
# Contributors: Brennan Longstreth
# Descritpion: File for CMake to build executables
# Notes: 
# Date Edited: 5/5/21
######################################################

project(MyPL)

cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-O0")
set(CMAKE_BUILD_TYPE Debug)

# build executables
add_executable(mypl hw6.cpp)
add_executable(mypltocpp Translator.cpp)
