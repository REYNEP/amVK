cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
# Paths with SPACES Aren't Supported
project(amVK)  #Cmake Docs on this command says,  SubDirectory should have usually have its own project() too

# If you do add_subdirectory(amVK)   specity   amVK_EXTERN cmake var
# TODO: extern should be OPTIONAL.... cz ppl could have these in their very own EXTERN dir
if(${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR})
  include_directories(
    extern/vulkan-sdk-lunarg/include
    extern/VulkanMemoryAllocator/include          #TODO: Move this into amVK?
  )
  set_directory_properties(PROPERTIES COMPILE_DEFINITIONS amVK_BUILD_WIN32=1)
else()
  include_directories(
    ${amVK_EXTERN}/vulkan-sdk-lunarg/include
    ${amVK_EXTERN}/VulkanMemoryAllocator/include          #TODO: Move this into amVK?
  )
endif()

if(WIN32)
    add_compile_definitions(amVK_BUILD_WIN32)
endif()

#USE #define VMA_IMPLEMENTATION in one of CPP File
option(amVK_AMD_VMA_SUPPORT "Vulkan Memory Allocator.... amVK_Device will have some VMA-Related stuffs, as VMA is Device Specific" ON)
add_definitions(-DamVK_AMD_VMA_SUP)


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


add_library(lib-amVK amVK.cpp amVK_WI.cpp amVK_Device.cpp amVK_Pipeline.cpp amVK_ImgMemBuf.cpp)
install(TARGETS lib-amVK 
        DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})