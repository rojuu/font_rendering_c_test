# cplayground

## Building
Use cmake for building
### Windows
For windows you need to provide the cmake argument `-DWIN32_LIB_PATH=<path_to_prebuilt_libs>`. The lib path is assumed to have the following structure:
- SDL2
    - include
        - SDL2
            - ... sdl2-headers
    - lib
        - x64
            - SDL2*.lib
            - SDL2.dll