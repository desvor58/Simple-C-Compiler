import os
import sys
import threading
import subprocess
from config import *

if not os.path.isdir(BUILD_DST) or not os.path.isfile(BUILD_DST + "scc.exe"):
    print("\033[31mError: scc not compiled, please build it:")
    print("    python tools/build.py\033[00m")
    exit(1)

def build_test(test_path: str):
    print(f"Building test {test_path} ...")
    ret = os.system(f"{BUILD_COMPILER} -o {TESTS_DIR}/bin/{test_path[test_path.rfind("/"):-2]}.exe {test_path}")
    if ret == 0:
        print(f"Done {test_path}")
    else:
        print(f"Error in {test_path}")

def test(module: str):
    if not os.path.exists(f"{TESTS_DIR}/bin"):
        os.mkdir(f"{TESTS_DIR}/bin")
    if not os.path.exists(f"{TESTS_DIR}/log"):
        os.mkdir(f"{TESTS_DIR}/log")

    print("\033[36m", end="")
    for file in os.listdir(f"{TESTS_DIR}/{module}"):
        if file.endswith(".c"):
            build_thread = threading.Thread(target=build_test, args=[f"{TESTS_DIR}/{module}/{file}"])
            build_thread.start()
            build_thread.join()
    print("\033[0m")
    for file in os.listdir(f"{TESTS_DIR}/bin"):
        print(f"{file[:-4]}:...", end="")
        process = subprocess.run([f"{TESTS_DIR}/bin/{file}", f"{TESTS_DIR}/log/{file[:-3]}log"])
        if process.returncode == 0:
            print("\b\b\b\033[32mok \033[00m")
        else:
            print(f"\b\b\b\033[31merr ({process.returncode})\033[00m")

if sys.argv[1] == "all":
    test("common")
else:
    test(sys.argv[1])