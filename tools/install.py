import os
import sys
from config import *

if not os.path.isdir(BUILD_DST) or not os.path.isfile(BUILD_DST + "scc.exe"):
    print("\033[38;2;256;0;0mError: scc not compiled, please build it:")
    print("    python tools/build.py\033[00m")
    exit(1)

if sys.platform == "win32":
    import winreg

    root_key = winreg.ConnectRegistry(None, winreg.HKEY_CURRENT_USER)
    key = winreg.OpenKey(root_key, "Environment", 0, winreg.KEY_ALL_ACCESS)
    value, value_type = winreg.QueryValueEx(key, "path")
    if not BUILD_DST in value.split(";"):
        print(os.path.abspath(BUILD_DST))
        value = value.rstrip(";") + ";" + os.path.abspath(BUILD_DST)
        winreg.SetValueEx(key, "path", 0, value_type, value)
    winreg.CloseKey(key)
    winreg.CloseKey(root_key)

print("\033[38;2;256;0;0mInstallation finish successfully\033[00m")
