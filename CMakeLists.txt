cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
#Paths with SPACES Aren't Supported
#Cmake Docs on this command says,  SubDirectory should have usually have its own project() too

#-----------------------------------------------------------------------------
# Usage
# 1. add_subdirectory(amVK)
# 2. target_link_libraries(your_program lib-amVK)    its safe.... 
#      cz we used target_include_directories(amVK PUBLIC ${INC})   and such new way, this `PUBLIC` means ${INC will be treated as Include_Dirs for `your_program` too}
#
# 3. Feel free to modify options below, before you build....


#-----------------------------------------------------------------------------
# Initialize project.
project(amVK LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)    #amVK_IN's Member variables [intern/amVK_IN.hh]
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#-----------------------------------------------------------------------------
# Options
option(amVK_MK1_NOOB_LOG "kinda DUMPS stuffs like 'available Vulkan exts/devices/stuffs' in the terminal like an NOOB API" ON)
option(amVK_HISTORY "Log every function, its kinda like a live stack_tarce" OFF)
option(amVK_HISTORY_INTERNAL  "Also the internal functions that are called" OFF)


set(INC
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/intern
  ${PROJECT_SOURCE_DIR}/VK/include    #Safe, cz we switched to SubModule
)




#-----------------------------------------------------------------------------
# Definitions, Platform Specifics (Libs & Defs)
SET(DEF

)

if(WIN32)
  list(APPEND   DEF   amVK_BUILD_WIN32)

  set(PLATFORM_LINKLIBS dbghelp shlwapi kernel32 Version) #BLI_ASSERT in amVK_Logger.hh
endif()


if(amVK_MK1_NOOB_LOG)
  list(APPEND   DEF   amVK_LOG_MK1)
endif()
if(amVK_HISTORY)
  list(APPEND   DEF   amVK_HISTORY)
endif()
if(amVK_HISTORY_INTERNAL)
  list(APPEND   DEF   amVK_HISTORY_INTERNAL)
endif()




#-----------------------------------------------------------------------------
# External Stuffs to build
message("\namVK:")
message("    Copying 'amVK/build_files/glslang_CMakeLists.cmake'  to  'amVK/glslang/CMakeLists.txt'")
file(RENAME     ${PROJECT_SOURCE_DIR}/glslang/CMakeLists.txt 
                ${PROJECT_SOURCE_DIR}/glslang/OG_CMakeLists.txt)
file(COPY_FILE  ${PROJECT_SOURCE_DIR}/build_files/glslang_CMakeLists.cmake
                ${PROJECT_SOURCE_DIR}/glslang/CMakeLists.txt)

# We use amVK/build_files/glslang_CMakeLists.cmake file.... first copying it
message("    add_subdirectory: glslang")
add_subdirectory(glslang)   # TARGET: amVK_glslang

file(REMOVE     ${PROJECT_SOURCE_DIR}/glslang/CMakeLists.txt)
file(RENAME     ${PROJECT_SOURCE_DIR}/glslang/OG_CMakeLists.txt
                ${PROJECT_SOURCE_DIR}/glslang/CMakeLists.txt)
message("    Restored OG 'amVK/glslang/CMakeLists.txt'")
message("")


#-----------------------------------------------------------------------------
# The Final Target
add_library(lib-amVK 
  intern/amVK_CX.cpp
  intern/amVK_Types.cpp
  intern/amVK_Device.cpp
  intern/amVK_WI.cpp 
  intern/amVK_RenderPass.cpp 
  intern/amVK_Pipeline.cpp
)
target_include_directories(lib-amVK PUBLIC ${INC})
target_compile_definitions(lib-amVK PUBLIC ${DEF})
if (WIN32)
  # TODO Change this `vulkan-1.lib` cz its from 1.2.170... updating it means redownloading vkSDK, + Also change in amGHOST_Variables.cmake
  target_link_libraries(lib-amVK ${PLATFORM_LINKLIBS} ${SPIRV} ${PROJECT_SOURCE_DIR}/vulkan-1.lib amVK_glslang)
endif()
install(TARGETS lib-amVK 
        DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})


#-----------------------------------------------------------------------------
# UnSet Options:
unset(amVK_MK1_NOOB_LOG CACHE)
unset(amVK_HISTORY CACHE)
unset(amVK_HISTORY_INTERNAL CACHE)