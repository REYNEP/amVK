import os, sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        raise Exception("USE: python postBuild_glslang.py ./")

    # Same as ${PROJECT_SOURCE_DIR} from glslang_CMakeLists.cmake
    project_source_dir = sys.argv[1]
    build_info_H = os.path.join(project_source_dir, "glslang/build_info.h")

    print("Removing: " + build_info_H)
    os.remove(build_info_H)