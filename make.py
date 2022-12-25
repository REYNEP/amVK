import os 
import sys
import subprocess
from pathlib import Path



#                ┏━╸╺┳┓┏━┓╻ ╻┏┓╻
#    ╺━╸╺━╸╺━╸   ┃╺┓ ┃┃┃ ┃┃╻┃┃┗┫   ╺━╸╺━╸╺━╸
#                ┗━┛╺┻┛┗━┛┗┻┛╹ ╹
# -------- gdown can fail on some old pythons.... as one my friend's PC  --------
_gDown_not_found_  = 'gDown not found.... calling `python -m pip install gdown`\n\n';
_gDown_pip_called_ = '\n\nHoping that gDown got installed';

try:
    import gdown
except (ImportError, ModuleNotFoundError) as err:
    print(_gDown_not_found_)
    subprocess.check_call( [sys.executable, "-m", "pip", "install", "gdown"] )
    print(_gDown_pip_called_)
    try:
        import gdown
    except ImportError:
        print("[gdown] ImportError Still Exists....")



#                ╺━┓╻┏━┓
#    ╺━╸╺━╸╺━╸   ┏━┛┃┣━┛   ╺━╸╺━╸╺━╸
#                ┗━╸╹╹
import zipfile

def unzip_components(what, where_to, remove_after = False):
    """
    Will overwrite existing stuffs...
    """
    print("Unzipping", what)
    with zipfile.ZipFile(what, 'r') as zip_ref:
        zip_ref.extractall(where_to)

    if (remove_after):
        try: os.remove(what)
        except FileNotFoundError: print("REYNEP_utils::unzip_components(): trying to remove param 'what' [FileNotFoundError]")




#                 ┏━╸┏┳┓┏━┓╻┏ ┏━╸
#    ╺━╸╺━╸╺━╸    ┃  ┃┃┃┣━┫┣┻┓┣╸    ╺━╸╺━╸╺━╸
#                 ┗━╸╹ ╹╹ ╹╹ ╹┗━╸
def check_cmake():
    print("TODO")

def build_cmake():
    should_gen_build_system = True
    if os.path.isdir("build-x64"):
        if os.path.exists("build-x64/CMakeCache.cmake"):
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






DOWNLOAD_ASK = "download external Libs? [Y/N]   (check ReadMe for link)\n";
DOWNLOAD_MSG = "\nmake.py : `gdown.download [output = 'amVK_Externs.zip']`";

def get_extern_GDrive():
    print(DOWNLOAD_MSG)
    #       https://drive.google.com/file/d/1x7KNV1undB74meBUt9tkvl4aHbd5RVok
    gdown.download(output="amVK_Externs.zip", id="1x7KNV1undB74meBUt9tkvl4aHbd5RVok", quiet=False)

    if(input("unzip amVK_Externs.zip? [Y/N]").lower() in ['y', 'yes']):
        if ( Path( 'amVK_Externs.zip' ).is_file() ):
            # Will overwrite....
            unzip_components("amVK_Externs.zip", "./extern/", remove_after = True)
        else:
            print("./amVK_Externs.zip not found")


def get_submodules():
    print("\nmake.py : `git submodule init`")
    subprocess.run(["git", "submodule", "init"], shell=True)
    print("\nmake.py : `git submodule update`")
    subprocess.run(["git", "submodule", "update"], shell=True)


def _user_io_download_():
    W = input("[GDrive / GitHub]...?\n");

    if (W.lower() == 'gdrive'):
        get_extern_GDrive()
    
    elif (W.lower() == 'github'):
        print("\nmake.py : `git clone https://github.com/KhronosGroup/glslang`")
        subprocess.run(["git", "clone", "https://github.com/KhronosGroup/glslang"], shell=True)

    else:
        return -1

#               ┏┳┓┏━┓╻┏┓╻
#   ╺━╸╺━╸╺━╸   ┃┃┃┣━┫┃┃┗┫   ╺━╸╺━╸╺━╸
#               ╹ ╹╹ ╹╹╹ ╹
if __name__ == "__main__":
    if (len(sys.argv) > 1):
        print("Use `python make.py`....")


    if (input(DOWNLOAD_ASK).lower() in [ 'y', 'yes']):
        while(_user_io_download_() == -1):
            print("wrong input.... try again.... \n")


    if (input("Get SubModules? [Y/N]\n").lower() in [ 'y', 'yes']):
        get_submodules();

    if (input("build lib-amVK? [Y/N]\n").lower() in [ 'y', 'yes']):
        build_cmake();
