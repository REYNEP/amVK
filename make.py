# Well, why make.py?   FASTEST thing I could think of.... and will work on ALL Platforms   + the syntax is WAY TOO COOL
# Thanks to Threads that made the process so so fast....  https://stackoverflow.com/a/21406995.... https://stackoverflow.com/a/50255019.... https://stackoverflow.com/a/3131251.... https://stackoverflow.com/a/50670037
import subprocess
import sys
import os


# -------- We Need gdown to Downlad from GDrive --------
try:
    import gdown
except (ImportError, ModuleNotFoundError) as err:
    print("gdown not found.... calling pip install gdown\n\n\n")
    subprocess.check_call([sys.executable, "-m", "pip", "install", "gdown"])
    print("\n\n\nHoping that gdown got installed")
    try:
        import gdown
    except ImportError:
        print("Error Still Exists....")



#               ╺┳┓┏━┓╻ ╻┏┓╻╻  ┏━┓┏━┓╺┳┓     ╻ ╻┏┓╻╺━┓╻┏━┓     ┏━┓┏━╸┏┳┓┏━┓╻ ╻┏━╸
#   ╺━╸╺━╸╺━╸    ┃┃┃ ┃┃╻┃┃┗┫┃  ┃ ┃┣━┫ ┃┃     ┃ ┃┃┗┫┏━┛┃┣━┛     ┣┳┛┣╸ ┃┃┃┃ ┃┃┏┛┣╸    ╺━╸╺━╸╺━╸
#               ╺┻┛┗━┛┗┻┛╹ ╹┗━╸┗━┛╹ ╹╺┻┛ ┛   ┗━┛╹ ╹┗━╸╹╹   ┛   ╹┗╸┗━╸╹ ╹┗━┛┗┛ ┗━╸

externs = {
    "vulkan-sdk-lunarg"
}
extern_dir = "./extern"
extern_zip = "extern-tmp-youCanDelete.zip"
def download_components():
    print("Downlaoding", extern_zip + ", using gdown from GoogleDrive")
    url = "https://drive.google.com/uc?id=105eAuD_0WNCh_RQxubeL1p-IAyY5ubX6"
    output = extern_zip
    gdown.download(url, output, quiet=False)
    print("\n\n")

import zipfile
def unzip_components(what, where_to):
    print("Unzipping", extern_zip, "\n\n")
    with zipfile.ZipFile(what, 'r') as zip_ref:
        zip_ref.extractall(where_to)

def remove_zips():
    print("Deleting", extern_zip)
    try: os.remove(extern_zip)
    except FileNotFoundError: print("We just downloaded ", extern_zip, "\nDid your OS delete that on its own?")
    print("\n\n")

def check_get_components():
    for lib in externs:
        if not os.path.isdir(extern_dir + "/" + lib):
            download_components()
            unzip_components(extern_zip, "./")
            remove_zips()
            return


#                ┏━╸╻ ╻┏━╸┏━╸╻┏    ┏┓     ╻ ╻┏━┓┏━╸   ┏━╸┏┳┓┏━┓╻┏ ┏━╸
#    ╺━╸╺━╸╺━╸   ┃  ┣━┫┣╸ ┃  ┣┻┓   ┃╺╋╸   ┃ ┃┗━┓┣╸    ┃  ┃┃┃┣━┫┣┻┓┣╸    ╺━╸╺━╸╺━╸
#                ┗━╸╹ ╹┗━╸┗━╸╹ ╹   ┗━┛    ┗━┛┗━┛┗━╸   ┗━╸╹ ╹╹ ╹╹ ╹┗━╸
def check_cmake():
    print("TODO")

def build_cmake():
    should_gen_build_system = True
    if os.path.isdir("build-x64"):
        if os.path.exists("build/CMakeCache.cmake"):
            print("build/CMakeCache.cmake exists.... [not calling 'cmake .']")
            should_gen_build_system = False
    else:
        os.mkdir("build-x64")


    # Change Dir
    cwd = os.getcwd()
    os.chdir("./build-x64")

    # Gen Build System if needed
    if should_gen_build_system:
        subprocess.check_call(["cmake ", "../"])

    # Finally BUILD   -- [TODO: Should we call check_call or smth ELSE]
    subprocess.run(["cmake", "--build", ".", "--target", "install", "--config", "release"])

    os.chdir(cwd)




#                ┏┳┓┏━┓╻┏┓╻
#    ╺━╸╺━╸╺━╸   ┃┃┃┣━┫┃┃┗┫   ╺━╸╺━╸╺━╸
#                ╹ ╹╹ ╹╹╹ ╹
if __name__ == "__main__":
    check_get_components()

    print("Finally calling cmake....\n\n")
    build_cmake()

    # I mean, Python is just WAY TOO MUCH COOL