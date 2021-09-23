# One of the Core Concept here is, You don't need to do everything with Functions, You can Directly Access/Write to CLASS VARS in amVK

# Hey so, about the Monitors, Monitors should have Contrasty HIGH Whites..... But the Browser should not use the Maximum Contrast, only the Movies should, Everyone, every developer should be aware of what its gonna look like on the eyes, Every Monitor should come with Contrast BenchMark & a Standard Point for Different COLOR-Profiles from Factory

# 1. Many things could have been just impl as NAMESPACE... but we kept them as CLASS (members not static still).... bcz of the USABILITY and MOBILITY of devs.... amVK is supposed to be VERY UNDERSTANDING and a TOOL for FASTER creation....      - So again we PUT OPTimization-Tricks all around.... [e.g. see the amVK_Pipeline.hh]

# Remember that TIME-SLIDE option in Timeline (Blender?).... Maybe have that way to Map Colors in Compositor

It's just safe to Include      amVK.hh      [just like you include vulkan/vulkan.hh]      Currently you will need to use amVK_INCLUDE_LIB
    Current Dependency of Each Modules:-
        amVK_Device:
            |-- [.hh]  vk_mem_alloc [AMD__VMA__H]
            |-- [.cpp] VMA_IMPLEMENTATION   [Definition_Kinda_Stuffs]
        amVK_WI:
            |-- [.hh]  amVK_Device  [pointerOnly]
            |-- [.cpp] amVK_Device  [CLASS__INFO]
        ANY:
            |-- amVK_Utils.hh
              |-- amVK.cpp  [vulkan_result_reg();] function in this TranslationUnit [The Sole Goal was to Reduce another TRANSLATION-UNIT]
        amVK_Logger:
            |-- [.hh]  amVK_Utils
        amVK_Pipeline:
            |-- [.hh]  amVK_Device  [pointerOnly]
            |-- [.cpp] amVK_Device  [CLASS__INFO]


# BENCHMARKS   [WIP-TODO_CustomBenchMarks]
    vkEnumeratePhysicalDevices - ~0.0016s  [First Call]   0.0010 [2nd Call]
    vkCreateSwapchain          - ~0.04s    [First Call]   ~0.008 [Re-Creattion on 768p Window]    [+vkCreateImageViews + vkCreateFrameBuffer]



[WIP]
# Passing Data Between Shader Stages
- be careful with it, as the more data you need to pass between stages, the more overhead you will have. 
- Old GPUs even have a very small number of how many variables can be passed.

# Multiple Pipelines
- Just a Couple Hours ago I thought, For switching from Solid to WireFrame view, my solution would be to have 2 Different Pipelines already there
- Just now I realized this is fairly Standard, [https://vkguide.dev/docs/chapter-2/toggling_shaders/#multiple-pipelines]

# Vertex Buffer on HOST/CPU Memory
- In Vulkan, you can allocate memory visible to the GPU, and read it from the shaders.
    1. on RX Vega 64 there is 256MB that kinda fits the needs [https://www.youtube.com/watch?v=zSG6dPq57P8&t=308s]

- You can allocate memory for two purposes, for images and buffers.  [vblanco20-1]  True for entire VULKAN API???
- ## Memory Allocation
    1. The number of total allocations is fixed by the driver, and can be a number as low as 1024.
    2. So we gonna use VMA [Vulkan Memory Allocator] (by AMD) & use SubAllocation
    3. The VMA library abstracts this slightly for us, but it’s still very important to know.

- ## Vertex Input Layout
    1. We can fill it out to let Vulkan know how to interpret the vertex data for your shader.

# VMA [Vulkan_Memory_Allocator_AMD]
- Current DEMO Usage is in RTR
- WIP [To Be impl. in amVK with cmake CHOICES]
- KEYPOINTS
    1. VMA_MEMORY_USAGE_ enums
    2. 