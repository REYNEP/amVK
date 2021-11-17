#CMAKE_CURRENT_SOURCE_DIR = where this extern/CMakeLists.txt is    even tho there is no call to project()

#${PROJECT_SOURCE_DIR}/extern = CMAKE_CURRENT_SOURCE_DIR
set(_amVK_EXTERN_LIB  ${PROJECT_SOURCE_DIR}/extern/lib CACHE STRING "" FORCE)
set(_amVK_EXTERN_INC  ${PROJECT_SOURCE_DIR}/extern/inc CACHE STRING "" FORCE)


#-----------------------------------------------------------------------------
# glslang
set(amVK_extern_lib_glslang amVK_glslang PARENT_SCOPE)

find_library(_lib_glslang
    NAMES
      amVK_glslang
    DOC
      ""
    PATHS
      ${_amVK_EXTERN_LIB}
    PATH_SUFFIXES
      Debug
      Release
    NO_DEFAULT_PATH
)

if ((NOT EXISTS ${_lib_glslang}) OR (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/inc))
  if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/glslang)
    message(FATAL_ERROR "you need to use amVK/extern/download.py to either clone dependant extern stuffs from github or download 'extern.zip' from GDrive")
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
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/glslang
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/glslang/glslang
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/glslang/SPIRV
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/glslang/StandAlone
    )
    target_link_libraries(amVK_glslang INTERFACE ${_lib_glslang})
endif()



#-----------------------------------------------------------------------------
# vulkan-1.lib
find_library(amVK_extern_lib_vulkan
    NAMES
      vulkan-1
    DOC
      ""
    PATHS
      ${_amVK_EXTERN_LIB}
    PATH_SUFFIXES
      Debug
      Release
    NO_DEFAULT_PATH
)