cmake_minimum_required(VERSION 3.25)
project(demo C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")

enable_language(RC)




include_directories("SDL/SDL2-2.30.11/x86_64-w64-mingw32/include")
link_directories("SDL/SDL2-2.30.11/x86_64-w64-mingw32/lib")

include_directories("SDL/SDL2_image-2.8.4/x86_64-w64-mingw32/include")
link_directories("SDL/SDL2_image-2.8.4/x86_64-w64-mingw32/lib")

include_directories("SDL/SDL2_mixer-2.7.2/x86_64-w64-mingw32/include")
link_directories("SDL/SDL2_mixer-2.7.2/x86_64-w64-mingw32/lib")

include_directories("SDL/SDL2_ttf-2.22.0/x86_64-w64-mingw32/include")
link_directories("SDL/SDL2_ttf-2.22.0/x86_64-w64-mingw32/lib")

include_directories("head/skill")
include_directories("head/text")
include_directories("head/constant")
include_directories("head/mystring")
include_directories("head/realrandom")
include_directories("head/text")
include_directories("head/connect")
set(SOURCES
        main.c
        head/skill/skill.c
        head/text/text.c
        head/constant/constant.c
        head/mystring/mystring.c
        head/realrandom/realrandom.c
        head/text/text.c
        head/connect/connect.c
        resource.rc
)

add_executable(demo ${SOURCES})
target_include_directories(demo PRIVATE ${CMAKE_SOURCE_DIR}/image)
target_link_libraries(demo PRIVATE
        mingw32
        SDL2main
        SDL2
        SDL2_image
        SDL2_mixer
        SDL2_ttf
)
target_link_libraries(demo PRIVATE ws2_32)