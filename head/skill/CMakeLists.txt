cmake_minimum_required(VERSION 3.28)
project(skill C)

set(CMAKE_C_STANDARD 11)

include_directories("D:/Code/test/head/constant")
include_directories("D:/Code/test/head/connect")

set(SOURCES
        D:/Code/test/head/skill/skill.c
        D:/Code/test/head/constant/constant.c
	D:/Code/test/head/connect/connect.c
)


add_library(skill STATIC ${SOURCES})
