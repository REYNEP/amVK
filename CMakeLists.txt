#-----------------------------------------------------------------------------
# Usage
# 1. add_subdirectory(amVK)
# 2. target_link_libraries(your_program lib-amVK)    its safe.... 
#      cz we used target_include_directories(amVK PUBLIC ${INC})   and such new way, this `PUBLIC` means ${INC will be treated as Include_Dirs for `your_program` too}
#
# 3. Feel free to check this file out, before you build....
#
# 4. you can Define variables like amVK_EXT_<name>_PATH
#      <name>: 'Vulkan', 'glslang'   [2 possible vals]
#
#    Paths with SPACES Aren't Supported
#    Cmake Docs on this command says,  SubDirectory should have usually have its own project() too
#
#       CMAKE_CURRENT_SOURCE_DIR = where this extern/CMakeLists.txt is    even tho there is no call to project()
#       CMAKE_CURRENT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}/extern
#             PROJECT_SOURCE_DIR = amVK/<CMakeLists.txt> where that was
#
# NOTES
# 1. This file returns if any error occured... using `return()` function
#

cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

project(amVK LANGUAGES CXX)

  if (NOT (${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR}))
    message(NOTICE "\nadd_subdirectory: amVK")
  endif()
  list(APPEND CMAKE_MESSAGE_INDENT "-- ")

  set(CMAKE_CXX_STANDARD 17)            # amVK_IN's Member variables [intern/amVK_IN.hh]
  set(CMAKE_CXX_STANDARD_REQUIRED ON)

  option(amVK_DEV_LOG "IDK, What I am doing...." ON)
  option(amVK_ZEN_LOG "Turn on GENERAL Logging... e.g. vkCreateInsntace, vkCreateDevice etc...." ON)



# --------------------
set(SRC
  intern/amVK_Device.cpp
  intern/amVK_Instance.cpp
  intern/amVK_RenderPass.cpp
  intern/amVK_WI.cpp
  intern/amVK_Pipeline.cpp
  
  intern/amVK_IMPLs.cpp
  intern/amVK_RendererT1.hh

  include/amVK_Types.hh
  include/amVK_CMD.hh
  include/amVK_Sync.hh

  #so that Intellisense Works.... + Compilation boost-up
  amVK_Instance.hh
  amVK_Device.hh
  amVK_WI.hh
  amVK_RenderPass.hh
  amVK_Pipeline.hh
)
set(INC
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/intern
  ${PROJECT_SOURCE_DIR}/extern
  ${PROJECT_SOURCE_DIR}/VK/include    #Safe, cz we switched to SubModule

  ${PROJECT_SOURCE_DIR}/extern/inc    #TODO: inside extern/CMakeLists.txt a Check should happen... if glslang is there
  ${PROJECT_SOURCE_DIR}/extern/inc/glslang
)
set(DEF)
set(LIB)




#-----------------------------------------------------------------------------
# MODS & Platform Specifics (Libs & Defs)

if(WIN32)
  list(APPEND   DEF   amVK_BUILD_WIN32)
  list(APPEND   LIB   dbghelp shlwapi kernel32 Version) #BLI_ASSERT in amVK_Logger.hh
elseif(UNIX)
  list(APPEND   DEF   amVK_BUILD_UNIX)
endif()

if (amVK_DEV_LOG)
  list(APPEND   DEF   amVK_DEV_LOG)
endif()

if (amVK_ZEN_LOG)
  list(APPEND   DEF   amVK_ZEN_LOG)
endif()



#-----------------------------------------------------------------------------
# External Stuffs to build
#   |- VK-SDK
#   |- glslang
add_subdirectory(extern)
if (NOT _amVK_EXTERN_OK)
  set(CMAKE_MESSAGE_INDENT "")
  message(STATUS "\n[amVK_FATAL_ERROR]\n")
  return()
endif()


# lib-amVK -----------
add_library(               lib-amVK            ${SRC})
target_include_directories(lib-amVK   PUBLIC   ${INC})
target_compile_definitions(lib-amVK   PUBLIC   ${DEF})
target_link_libraries(     lib-amVK   PRIVATE  ${LIB} ${amVK_EXT_Vulkan} ${amVK_EXT_glslang})


# install ------------
install(TARGETS lib-amVK 
        DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})

#---------------------
# UnSet Options:
unset(amVK_DEV_LOG CACHE)
unset(amVK_ZEN_LOG CACHE)