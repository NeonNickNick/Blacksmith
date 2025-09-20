cmake_minimum_required(VERSION 3.28)
project(connect C)

set(CMAKE_C_STANDARD 11)

add_library(connect STATIC library.c)
