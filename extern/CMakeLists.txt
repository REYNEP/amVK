#-----------------------------------------------------------------------------
#  amVK_EXT_<name>: <name> should be smth that can be printed out as '<name> Found/NotFound'
#  amVK_EXT_Vulkan: Vulkan-SDK Stuffs....
# amVK_EXT_glslang:
#

set(_amVK_EXTERN_OK false PARENT_SCOPE)

# IMPLEMENT: re_Find_Library???
macro(_SMTH_IN_THE_WAY   module_name)
  if (NOT amVK_EXT_${module_name})
    message(NOTICE "[amVK]   " ${module_name} "   Not Found.... Use make.py to download it.\n  OR You can define 'amVK_EXT_${module_name}_PATH'\n  OR Follow REYNEP-Extern-Download-Guide")
    return()
  
  else()
    set(_G_ 12)     # GAP
    string(LENGTH  ${module_name}  _L_)
    math  (EXPR   _L_  "${_G_} - ${_L_}")
    string(REPEAT  " "  ${_L_}     _G_)

    message(NOTICE "[amVK] " ${_G_} "Found " ${module_name} ": " ${amVK_EXT_${module_name}})
  endif()
endmacro()


set(_amVK_EXTERN_LIB  ${PROJECT_SOURCE_DIR}/extern/lib CACHE STRING "" FORCE)
set(_amVK_EXTERN_INC  ${PROJECT_SOURCE_DIR}/extern/inc CACHE STRING "" FORCE)


#-----------------------------------------------------------------------------
# vulkan-1.lib
#   Vulkan::Vulkan should not be used. That is a <TARGET> that cmake provides.... & I Hate https://cmake.org/cmake/help/latest/policy/CMP0028.html
#   because it doesn't make sense to limit it in those curcumstances, if `::` actually aren't the problems, tho I think they are....  [on WIN32 it doesn't work]
#  
find_package(Vulkan QUIET)

if (Vulkan_FOUND)
  message("[amVK] FindVulkan  Worked: ${Vulkan_LIBRARY}")

  set(amVK_EXT_Vulkan ${Vulkan_LIBRARY} CACHE STRING "" FORCE)
else()
  find_library(amVK_EXT_Vulkan_TMP
    NAMES
      vulkan      # UNIX/MAC
      vulkan-1    # Windows
    DOC
      ""
    PATHS
      ${_amVK_EXTERN_LIB}
      ${amVK_EXT_Vulkan_PATH}
    PATH_SUFFIXES
      Debug
      Release
      Lib         # Windows
      lib         # UNIX/MAC
    NO_DEFAULT_PATH
    NO_CACHE
  )

  if (EXISTS ${amVK_EXT_Vulkan_TMP})
    set(amVK_EXT_Vulkan ${amVK_EXT_Vulkan_TMP} CACHE STRING "" FORCE)
  endif()

  _SMTH_IN_THE_WAY(Vulkan)    # Also Messages....
endif()






#-----------------------------------------------------------------------------
# glslang
if (Vulkan_glslang_FOUND)   # New in CMAKE-3.24
  message("[amVK] FindVulkan GLSLANG: " ${Vulkan_glslang_LIBRARY})

  set(amVK_EXT_glslang ${Vulkan_glslang_LIBRARY} CACHE STRING "" FORCE)
else()
  find_library(amVK_EXT_glslang
    NAMES
      amVK_glslang    # amVK Stuff
      glslang         # linux [https://archlinux.org/packages/extra/x86_64/glslang/]
    DOC
      "IDK What I am doing...."
    PATHS
      ${_amVK_EXTERN_LIB}
      ${amVK_EXT_glslang}
    PATH_SUFFIXES
      Debug
      Release
      Lib             # Windows
      lib             # UNIX/MAC
    NO_DEFAULT_PATH
  )

  _SMTH_IN_THE_WAY(glslang)

  # NOTE: RE_FIND actually prints, WHAT WE Tried to find and where....
  find_path(glslang_INCLUDE_DIR
    NAMES
      "glslang/SPIRV/GlslangToSpv.h"
    DOC
      "amVK_Pipeline.hh"
    PATHS
      ${_amVK_EXTERN_INC}
      ${_amVK_EXTERN_INC}/glslang
      ${amVK_EXT_glslang}
    PATH_SUFFIXES
      glslang
      include
      SPIRV
    NO_CACHE
    NO_DEFAULT_PATH
  )
endif()





set(_amVK_EXTERN_OK true PARENT_SCOPE)




#[[

set(amVK_EXT_glslang amVK_glslang PARENT_SCOPE)

if ((NOT EXISTS ${_lib_glslang}) OR (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/inc))
  if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/glslang)
    message(FATAL_ERROR "you need to use amVK/make.py to either clone dependant extern stuffs from github or download 'extern.zip' from GDrive")
  endif()
  
  message("\namVK:")
  message(STATUS "    Will Copy 'amVK/build_files/glslang_CMakeLists.cmake'  to  'amVK/glslang/CMakeLists.txt'")
  file(RENAME     ${CMAKE_CURRENT_SOURCE_DIR}/glslang/CMakeLists.txt 
                  ${CMAKE_CURRENT_SOURCE_DIR}/glslang/OG_CMakeLists.txt)
  file(COPY_FILE  ${CMAKE_CURRENT_SOURCE_DIR}/build_files/glslang_CMakeLists.cmake
                  ${CMAKE_CURRENT_SOURCE_DIR}/glslang/CMakeLists.txt)

  # We use amVK/build_files/glslang_CMakeLists.cmake file.... first copying it
  message("    add_subdirectory: glslang")
  add_subdirectory(glslang)   # TARGET: amVK_glslang

  file(REMOVE     ${CMAKE_CURRENT_SOURCE_DIR}/glslang/CMakeLists.txt)
  file(RENAME     ${CMAKE_CURRENT_SOURCE_DIR}/glslang/OG_CMakeLists.txt
                  ${CMAKE_CURRENT_SOURCE_DIR}/glslang/CMakeLists.txt)
  message("    after add_subdirectory(glslang),  will Restore OG 'amVK/glslang/CMakeLists.txt'")
  message("")

else()

  add_library(amVK_glslang INTERFACE)
  target_include_directories(amVK_glslang INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/glslang
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/glslang/glslang
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/glslang/SPIRV
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/glslang/StandAlone
  )
  target_link_libraries(amVK_glslang INTERFACE ${_lib_glslang})

endif()

]]