cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

set(This cplayground)
project(${This})

add_executable(${This}
    src/main.c
    src/common.h
    src/utils.h
    src/utils.c
    src/renderer.h
    src/renderer.c
    src/stb_ds.h
    src/stb_ds.c
    src/stb_truetype.h
    src/stb_truetype.c
)

if(MSVC)
    set(CPG_WIN32_ENABLE_CONSOLE OFF CACHE BOOL "If set to TRUE, uses subsystem:console, otherwise (by default) subsystem:windows")

    set(win32_extra_flags)
    if (CPG_WIN32_ENABLE_CONSOLE)
        target_link_options(${This} PRIVATE -subsystem:console)
    else()
        set(win32_extra_flags ${win32_extra_flags} -DWIN32_WINMAIN=1)
        target_link_options(${This} PRIVATE -subsystem:windows)
    endif()
    target_compile_options(${This} PRIVATE -wd6011 -wd4996 ${win32_extra_flags})
endif()

add_subdirectory(deps/SDL2-2.0.12)

target_link_libraries(${This} PRIVATE SDL2-static)
