import os, sys
import subprocess

if __name__ == "__main__":
    if len(sys.argv) < 2:
        raise Exception("USE: python preBuild_glslang.py ./")

    # Same as ${PROJECT_SOURCE_DIR} from glslang_CMakeLists.cmake
    project_source_dir = sys.argv[1]
    build_info_H = os.path.join(project_source_dir, "glslang/build_info.h")
    build_info_PY = os.path.join(project_source_dir, "build_info.py")
    build_info_H_TMPL = os.path.join(project_source_dir, "build_info.h.tmpl")
    
    # create glslang/build_info.h
    with open(build_info_H, 'w') as genX:
        subprocess.run(["python", build_info_PY, project_source_dir, "-i", build_info_H_TMPL,], stdout=genX)
        # Greatest hack of today.... now CMake won't treat this build_info.h as NEWFILE everytime I do a build
        os.utime(build_info_H, (1636924969, 1636924969))    #November 15, 2021 3:22:49 AM GMT+06:00