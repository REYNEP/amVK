cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
#Paths with SPACES Aren't Supported
#Cmake Docs on this command says,  SubDirectory should have usually have its own project() too

#-----------------------------------------------------------------------------
# Initialize project.
project(amVK LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)    #amVK_IN's Member variables [amVK_Common.hh]
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#-----------------------------------------------------------------------------
# Options
# Currently VMA must be ON
option(amVK_AMD_VMA_SUPPORT "Vulkan Memory Allocator.... amVK_Device will have some VMA-Related stuffs, as VMA is Device Specific" ON)
option(amVK_MK1_NOOB_LOG "kinda FUMPS stuffs like available Vulkan stuffs in the terminal like an NOOB API" OFF)
option(amVK_HISTORY "Log every function, its kinda like a live stack_tarce" OFF)
option(amVK_HISTORY_INTERNAL  "Also the internal functions that are called" OFF)

#-----------------------------------------------------------------------------
# Externs
if(${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR})
  set(amVK_EXTERN ${PROJECT_SOURCE_DIR}/extern)   #where make.py downloads: use `python make.py download`
else()
  if (NOT EXISTS ${PROJECT_SOURCE_DIR}/extern)
    if (NOT amVK_EXTERN)
      message(STATUS "Did you move amVK/extern/ folder?   [or maybe you didn't download it. use `python make.py download`]")
      message(STATUS "if you did move, then define amVK_EXTERN variable. to whereever you moved extern dependencies to....")
      message(NOTICE "")
      message(FATAL_ERROR "amVK Can't be built.... ")
    endif()
  endif()
endif()

set(INC_EXTERN
  ${PROJECT_SOURCE_DIR}
  ${amVK_EXTERN}/vulkan-sdk-lunarg/include
  ${amVK_EXTERN}/VulkanMemoryAllocator/include
)

set(_all_extern_found TRUE)
foreach(_DIR ${INC_EXTERN})
  if (NOT EXISTS ${_DIR})
    message(STATUS "amVK [Extern]: " ${_DIR} " - not found")
    set(_all_extern_found FALSE)
  endif()
endforeach()
if (NOT _all_extern_found)
  message(STATUS "Not all reuired extern libraries found....")
  message(NOTICE "")
  message(FATAL_ERROR "amVK Can't be built.... ")
endif()


#-----------------------------------------------------------------------------
# Definitions, Platform Specifics (Libs & Defs)
SET(DEF

)

if(WIN32)
  list(APPEND   DEF   amVK_BUILD_WIN32)
  list(APPEND   DEF   amVK_LOGGER_BLI_ASSERT)

  set(PLATFORM_LINKLIBS dbghelp shlwapi kernel32 Version)
endif()


if(amVK_AMD_VMA_SUPPORT)
  list(APPEND   DEF   amVK_AMD_VMA_SUP)
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
# GLSLC & Shaders
find_program (glslc-exe
    NAMES 
      glslc
    DOC 
      "GLSL Compiler - Outputs SPIR-V files [which are VULKAN Compatible]"
    PATHS
      $ENV{VK_SDK_PATH}
    PATH_SUFFIXES
      Bin
      bin
    NO_DEFAULT_PATH
    REQUIRED
)

message("\n${glslc-exe}\n")


#-----------------------------------------------------------------------------
# The Final Target
add_library(lib-amVK amVK.cpp amVK_WI.cpp amVK_RenderPass.cpp amVK_Types.cpp amVK_Device.cpp amVK_Pipeline.cpp amVK_ImgMemBuf.cpp)
target_include_directories(lib-amVK PUBLIC ${INC_EXTERN})
target_compile_definitions(lib-amVK PUBLIC ${DEF})
if (WIN32)
  target_link_libraries(lib-amVK ${PLATFORM_LINKLIBS})
endif()
install(TARGETS lib-amVK 
        DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})


#-----------------------------------------------------------------------------
# UnSet Options:
unset(amVK_AMD_VMA_SUPPORT CACHE)
unset(amVK_MK1_NOOB_LOG CACHE)
unset(amVK_HISTORY CACHE)
unset(amVK_HISTORY_INTERNAL CACHE)