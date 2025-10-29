import os
import sys
from config import *

if not os.path.isdir(BUILD_DST) or not os.path.isfile(BUILD_DST + "scc.exe"):
    print("\033[38;2;256;0;0mError: scc not compiled, please build it:")
    print("    python tools/build.py\033[00m")
    exit(1)

def test(module: str):
    for file in os.listdir(f"{TEST_CODE_DIR}/{module}"):
        if file.endswith(".c"):
            print(f"test:{module}:{file}{TEST_FORMAT_TAB}...", end="")
            os.system(f"scc {os.path.join(f"{TEST_CODE_DIR}/{module}", file)} -E > {TEST_OUT_FILE}")
            res_file = open(f"{TEST_RESULT_DIR}/{module}/{file[:-2]}")
            out_file = open(TEST_OUT_FILE, "r")
            if out_file.read()[:-2] == res_file.read():
                print("\b\b\bOK ")
            else:
                print("\b\b\bERR")
            res_file.close()
            out_file.close()

if sys.argv[1] == "all":
    test("preproc")
    test("tokenizer")
else:
    test(sys.argv[1])