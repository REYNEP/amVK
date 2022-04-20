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
option(amVK_DEBUG_LOG "IDK, What I am doing...." OFF)
option(amVK_DO_LOG "Turn on GENERAL Logging... e.g. vkCreateInsntace, vkCreateDevice etc...." ON)


set(INC
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/intern
  ${PROJECT_SOURCE_DIR}/extern
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

if (amVK_DEBUG_LOG)
  list(APPEND   DEF   amVK_DEBUG_LOG)

  if (NOT amVK_DO_LOG)
    list(APPEND DEF   amVK_DO_LOG)
  endif()
endif()

if (amVK_DO_LOG)
  list(APPEND   DEF   amVK_DO_LOG)
endif()



#-----------------------------------------------------------------------------
# External Stuffs to build
add_subdirectory(extern)
#glslang


#-----------------------------------------------------------------------------
# The Final Target
add_library(lib-amVK 
  intern/amVK_Instance.cpp
  intern/amVK_Device.cpp
  intern/amVK_WI.cpp 
  intern/amVK_RenderPass.cpp 
  intern/amVK_Pipeline.cpp

  intern/amVK_IMPLs.cpp

  #so that Intellisense Works.... + Compilation boost-up
  amVK_Instance.hh
  amVK_Device.hh
  amVK_Types.hh
  amVK_WI.hh
  amVK_RenderPass.hh
  amVK_Pipeline.hh
  amVK_CMD.hh
  amVK_Sync.hh
  amVK_RendererT1.hh
)
target_include_directories(lib-amVK PUBLIC ${INC})
target_compile_definitions(lib-amVK PUBLIC ${DEF})
if (WIN32)
  # TODO Change this `vulkan-1.lib` cz its from 1.2.170... updating it means redownloading vkSDK, + Also change in amGHOST_Variables.cmake
  target_link_libraries(lib-amVK ${PLATFORM_LINKLIBS} ${SPIRV} ${amVK_extern_lib_vulkan} ${amVK_extern_lib_glslang})
endif()
install(TARGETS lib-amVK 
        DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})

#target_compile_options(lib-amVK PUBLIC "/P")


#-----------------------------------------------------------------------------
# UnSet Options:
unset(amVK_DEBUG_LOG CACHE)
unset(amVK_DO_LOG CACHE)