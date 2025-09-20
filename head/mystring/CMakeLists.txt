cmake_minimum_required(VERSION 3.28)
project(mystring C)

set(CMAKE_C_STANDARD 11)

add_library(mystring STATIC library.c)
