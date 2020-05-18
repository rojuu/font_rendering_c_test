cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

set(This cplayground)
project(${This})

add_executable(${This}
    src/main.c
    src/stb_truetype.c
)

# Using prebuild binaries for win32, see README.md
if (WIN32)
    set(WIN32_LIB_PATH CACHE PATH "Path to pre-built win32 libs")

    #SDL2
    target_include_directories(${This} PRIVATE ${WIN32_LIB_PATH}/SDL2/include)
    target_link_directories(${This} PRIVATE ${WIN32_LIB_PATH}/SDL2/lib/x64)
    configure_file(${WIN32_LIB_PATH}/SDL2/lib/x64/SDL2.dll ${CMAKE_CURRENT_BINARY_DIR}/SDL2.dll COPYONLY)
endif()

target_link_libraries(${This} PRIVATE SDL2)