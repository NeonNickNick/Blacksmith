cmake_minimum_required(VERSION 3.28)
project(realrandom C)

set(CMAKE_C_STANDARD 11)

add_library(realrandom STATIC library.c)
