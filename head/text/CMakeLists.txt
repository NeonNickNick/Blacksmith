cmake_minimum_required(VERSION 3.28)
project(text C)

set(CMAKE_C_STANDARD 11)
include_directories("D:/Code/test/head/constant")

set(SOURCES
        D:/Code/test/head/text/text.c
        D:/Code/test/head/constant/constant.c
)

add_library(text STATIC ${SOURCES})
