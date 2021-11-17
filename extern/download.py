import os, sys, subprocess
import "REYNEP_utils/REYNEP_utils" as REYNEP_utils

params = ["GDrive", "Github"]

if __name__ = "__main__":
    if ((len(sys.argv) != 2) or sys.argv[1] not in params)
        print("Use `python download.py GDrive`.... [GDrive/Github] - Only 2 accepted params")

    if (sys.argv[1] == "GDrive"):
        REYNEP_utils.gdrive_download("14A_2sXYsnkEeBK6FR2Pq7AUQO1E0OB2O", "extern.zip");
        REYNEP_utils.unzip_components("extern.zip", "./", True)   #Will overwrite....  True means delete .zip after unzipping
    elif (sys.argv[1] == "GitHub"):
        subprocess.run(["git", "clone", "https://github.com/KhronosGroup/glslang"], shell=True)