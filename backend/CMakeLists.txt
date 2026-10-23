cmake_minimum_required(VERSION 3.20)

project(OneLineBackend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(one_line_backend src/main.cpp)