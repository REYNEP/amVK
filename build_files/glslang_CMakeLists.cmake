cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

#-----------------------------------------------------------------------------
# Usage
# 1. add_subdirectory(glslang)  in amVK/CMakeLists.txt
# 2. glslang is from [https://github.com/KhronosGroup/glslang] which has a CMakeLists.txt
#
# 3. we don't care about that one, we back that up.... copy this file [to that file's name].... do add_subdirectory() and then revert back to OG
#    [another rule: this file should be as simple as Blender Extern folders]
#
# 4. Godot's Implementation was a huge help.... https://github.com/godotengine/godot/blob/master/modules/glslang/SCsub [NOV 15 2021]
#    I just copy pasted stuffs mostly....

#-----------------------------------------------------------------------------
# Initialize project.
project(glslang_amVK)

set(XD1 glslang/MachineIndependent)
set(XD2 SPIRV)

#Copied from GODOT's that file 😉
set(SRC
    glslang/CInterface/glslang_c_interface.cpp
    ${XD1}/attribute.cpp
    ${XD1}/Constant.cpp
    ${XD1}/glslang_tab.cpp
    ${XD1}/InfoSink.cpp
    ${XD1}/Initialize.cpp
    ${XD1}/Intermediate.cpp
    ${XD1}/intermOut.cpp
    ${XD1}/IntermTraverse.cpp
    ${XD1}/iomapper.cpp
    ${XD1}/limits.cpp
    ${XD1}/linkValidate.cpp
    ${XD1}/parseConst.cpp
    ${XD1}/ParseContextBase.cpp
    ${XD1}/ParseHelper.cpp
    ${XD1}/PoolAlloc.cpp
    ${XD1}/preprocessor/PpAtom.cpp
    ${XD1}/preprocessor/PpContext.cpp
    ${XD1}/preprocessor/Pp.cpp
    ${XD1}/preprocessor/PpScanner.cpp
    ${XD1}/preprocessor/PpTokens.cpp
    ${XD1}/propagateNoContraction.cpp
    ${XD1}/reflection.cpp
    ${XD1}/RemoveTree.cpp
    ${XD1}/Scan.cpp
    ${XD1}/ShaderLang.cpp
    ${XD1}/SpirvIntrinsics.cpp
    ${XD1}/SymbolTable.cpp
    ${XD1}/Versions.cpp

    glslang/GenericCodeGen/CodeGen.cpp
    glslang/GenericCodeGen/Link.cpp
    OGLCompilersDLL/InitializeDll.cpp

    ${XD2}/CInterface/spirv_c_interface.cpp
    ${XD2}/disassemble.cpp
    ${XD2}/doc.cpp
    ${XD2}/GlslangToSpv.cpp
    ${XD2}/InReadableOrder.cpp
    ${XD2}/Logger.cpp
    ${XD2}/SpvBuilder.cpp
    ${XD2}/SpvPostProcess.cpp
    ${XD2}/SPVRemapper.cpp
    ${XD2}/SpvTools.cpp

    StandAlone/ResourceLimits.cpp
)

if (WIN32)
    list(APPEND SRC
        glslang/OSDependent/Windows/ossource.cpp
    )
elseif(UNIX)
    list(APPEND SRC
        glslang/OSDependent/Unix/ossource.cpp
    )
endif()


# https://github.com/godotengine/godot/blob/master/thirdparty/glslang/glslang/build_info.h
if (NOT EXISTS ${PROJECT_SOURCE_DIR}/glslang/build_info.h)
    message("Creating ${PROJECT_SOURCE_DIR}/glslang/build_info.h")
    execute_process(
        COMMAND
          python build_info.py ./ -i build_info.h.tmpl
        WORKING_DIRECTORY 
          ${PROJECT_SOURCE_DIR}
        OUTPUT_FILE
          ${PROJECT_SOURCE_DIR}/glslang/build_info.h
    )
endif()


set(INC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/glslang
    ${PROJECT_SOURCE_DIR}/SPIRV
    ${PROJECT_SOURCE_DIR}/StandAlone
    ${PROJECT_SOURCE_DIR}/glslang/**    #Testing
)

add_library(amVK_glslang ${SRC})
target_include_directories(amVK_glslang PUBLIC ${INC})