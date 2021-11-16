#CMAKE_CURRENT_SOURCE_DIR = where this extern/CMakeLists.txt is    even tho there is no call to project()

#${PROJECT_SOURCE_DIR}/extern = CMAKE_CURRENT_SOURCE_DIR
set(_amVK_EXTERN_LIB  ${PROJECT_SOURCE_DIR}/extern/lib CACHE STRING "" FORCE)
set(_amVK_EXTERN_INC  ${PROJECT_SOURCE_DIR}/extern/inc CACHE STRING "" FORCE)

message("\namVK:")
message("    Will Copy 'amVK/build_files/glslang_CMakeLists.cmake'  to  'amVK/glslang/CMakeLists.txt'")
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