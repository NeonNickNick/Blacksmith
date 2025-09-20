cmake_minimum_required(VERSION 3.28)
project(constant C)

set(CMAKE_C_STANDARD 11)

add_library(constant STATIC library.c)
