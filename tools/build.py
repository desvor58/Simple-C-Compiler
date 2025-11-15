import os
from config import *

if not os.path.isdir(BUILD_DST):
    os.mkdir(BUILD_DST)

os.system(f"{BUILD_COMPILER} {BUILD_COMPILER_FLAGS} -o {BUILD_DST}scc.exe src/*.c")

print("\033[32mBuilding finish successfully\033[0m")