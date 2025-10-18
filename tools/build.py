import os
from config import *

if not os.path.isdir(BUILD_DST):
    os.mkdir(BUILD_DST)

os.system(f"{BUILD_COMPILER} {BUILD_COMPILER_FLAGS} -o {BUILD_DST}scc.exe src/*.c")

print("\033[38;2;256;0;0mBuilding finish successfully\033[00m")