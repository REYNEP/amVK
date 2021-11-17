import os, subprocess

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


def get_submodules():
    print("\nmake.py : `git submodule init`")
    subprocess.run(["git", "submodule", "init"], shell=True)
    print("\nmake.py : `git submodule update`")
    subprocess.run(["git", "submodule", "update"], shell=True)


#               ┏┳┓┏━┓╻┏┓╻
#   ╺━╸╺━╸╺━╸   ┃┃┃┣━┫┃┃┗┫   ╺━╸╺━╸╺━╸
#               ╹ ╹╹ ╹╹╹ ╹
if __name__ == "__main__":
    get_submodules();
    build_cmake();
    