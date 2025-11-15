import os
import sys
import threading
import subprocess
from config import *

def build_test(test_path: str):
    print(f"Building test {test_path} ...")
    ret = os.system(f"{BUILD_COMPILER} -o {TESTS_DIR}/bin/{test_path[test_path.rfind("/"):-2]}.exe {test_path}")
    if ret == 0:
        print(f"Done {test_path}")
    else:
        print(f"Error in {test_path}")

def test(module: str) -> int:
    total_errs = 0

    if not os.path.exists(f"{TESTS_DIR}/bin"):
        os.mkdir(f"{TESTS_DIR}/bin")
    if not os.path.exists(f"{TESTS_DIR}/log"):
        os.mkdir(f"{TESTS_DIR}/log")

    print("\033[36m", end="")
    build_threads = []
    for file in os.listdir(f"{TESTS_DIR}/{module}"):
        if file.endswith(".c"):
            build_thread = threading.Thread(target=build_test, args=[f"{TESTS_DIR}/{module}/{file}"])
            build_thread.start()
            build_threads.append(build_thread)

    for th in build_threads:
        th.join()
    print("\033[0m")

    for file in os.listdir(f"{TESTS_DIR}/bin"):
        print(f"{file[:-4]}:...", end="")
        process = subprocess.run([f"{TESTS_DIR}/bin/{file}", f"{TESTS_DIR}/log/{file[:-3]}log"])
        if process.returncode == 0:
            print("\b\b\b\033[32mok \033[00m")
        else:
            print(f"\b\b\b\033[31merr ({process.returncode})\033[00m")
            total_errs += 1

    return total_errs
    
if (__name__ == "__main__"):
    if not os.path.isdir(BUILD_DST) or not os.path.isfile(BUILD_DST + "scc.exe"):
        print("\033[31mError: scc not compiled, please build it:")
        print("    python tools/build.py\033[00m")
        exit(1)
    
    total_errs = 0
    modules = []
    if sys.argv[1] == "all":
        modules = TEST_ALL_MODULES
    else:
        modules = sys.argv[1:]

    for module in modules:
        total_errs += test(module)
    
    if total_errs == 0:
        print("\n\033[32m", end="")
    else:
        print("\n\033[31m", end="")
    print(f"total errors:{total_errs}\033[0m")