# Font rendering test in C
Unless otherwise specified, code is lisences under the license found in the UNLICENSE file. Included Roboto font is lisenced under Apache licensed that can be found on the font [download page](https://fonts.google.com/specimen/Roboto)

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