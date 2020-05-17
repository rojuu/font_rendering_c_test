import os
import sys
import subprocess
import shutil
import argparse
import json
import pathlib

parser = argparse.ArgumentParser(description='Build script for the program.')
parser.add_argument("--buildtype", choices=["release", "debug"], required=True)

if sys.platform == "win32":
    parser.add_argument("--win32libpath", help="path to prebuilt libs for win32",\
                        required=True)

cmd_args = parser.parse_args()

if not os.path.exists('build'):
    os.makedirs('build')

build_dir = "build"

lib_arguments = [""]
inc_arguments = [""]
exe_name = build_dir+"/pg"

if sys.platform == "win32":
    exe_name = exe_name+".exe"

    win32_libs = cmd_args.win32libpath
    win32_libs = win32_libs.rstrip('\\')

    inc_arguments.append("-I"+win32_libs+"\\SDL2\\include")
    lib_arguments.append("-L"+win32_libs+"\\SDL2\\lib\\x64")
    lib_arguments.append("-lSDL2.lib")
    shutil.copyfile(win32_libs + "\\SDL2\\lib\\x64\\SDL2.dll", "build\\SDL2.dll")

else:
    lib_arguments.append("-lSDL2")

src_files = [
    "src/main.c",
    "src/stb_truetype.c",
]

args = []
args.extend(["-o", exe_name])

if cmd_args.buildtype == "debug":
    args.append("-Og")
    args.append("-g")
else:
    args.append("-O3")

args.extend(inc_arguments)
args.extend(lib_arguments)

cmd = ["clang"]
cmd.extend(src_files)
cmd.extend(args)

print("Compiling:", *cmd)
subprocess.run(cmd)

compile_commands_json = []
for f in src_files:
    command = {}
    command["directory"] = os.getcwd()

    c = ""
    for cm in cmd:
        c += cm + " "

    command["command"] = c
    command["file"] = f
    compile_commands_json.append(command)

f = open("compile_commands.json", "w")
f.write(json.dumps(compile_commands_json, indent=4))
f.close
