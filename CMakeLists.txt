cmake_minimum_required(VERSION 3.21)

project(na_serializer VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS FALSE)

add_executable(na_serializer test.cpp)
