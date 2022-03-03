#pragma once
#include "amVK_IN.hh"

/**
 * \internal only used in \class amVK_DeviceMK2
 * 
 * as per Physical Device [PD]
 * 
 * NOW:
 *   only filter/find memoryTypeIndex for now
 *   \note this Could have been part of amVK_DeviceMK2    but for now lets just have it here
 * 
 * FUTURE:
 * we only handle the MemoryHeap management here.... a.k.a if its full
 *      but we don't store stuffs like size and etc. we only like check if result is MEMORY OVERFLOW or smth. 
 *          and let you ask for what heap has how much left
 * 
 *
 * 
 * 
 * 
 * - ## Memory Allocation
    1. The number of total allocations is fixed by the driver, and can be a number as low as 1024.
    2. So we gonna use VMA [Vulkan Memory Allocator] (by AMD) & use SubAllocation
    3. The VMA library abstracts this slightly for us, but it’s still very important to know.
 * 
 * \todo Find more details like this on VkMemory,   THIS is From notesSoFar.md
 */
class MemoryMK2 {
  public:
    VkPhysicalDevice _PD = nullptr;
    VkPhysicalDeviceMemoryProperties *mem_props = nullptr;

    MemoryMK2(VkPhysicalDevice PD) : _PD(PD) {  //Internally used, so no error checking, also assuming that, no Double MemoryMK2 is created on same PhysicalDevoce
        uint32_t PD_index = HEART->PD.index(_PD);
        mem_props = &( HEART->PD.mem_props[PD_index] );
    }
    ~MemoryMK2() {}

    /**
     * \todo choose a bigger VkMemoryHeap?
     * NOW:
     *   from: https://github.com/blurrypiano/littleVulkanEngine/blob/master/littleVulkanEngine/tutorial20/lve_device.cpp#L392
     */
    uint32_t _find_mem_type(uint32_t memoryTypeBits, VkMemoryPropertyFlags flags) {
        for (uint32_t i = 0; i < mem_props->memoryTypeCount; i++) {
            if (
                (memoryTypeBits & (1 << i)) &&
                ((mem_props->memoryTypes[i].propertyFlags & flags) == flags)
            ) {
                return i;
            }
        }
        return UINT32_T_NULL;
    }
};


/**
 * | -------------------------------------- |
 *  SOMETHING ABOUT LOADING Vulkan Functions 
 * | -------------------------------------- |
 * 

You may need to configure the way you import Vulkan functions.

By default, VMA assumes you you link statically with Vulkan API. 
If this is not the case, #define VMA_STATIC_VULKAN_FUNCTIONS 0 before #include of the VMA implementation and use another way.

You can #define VMA_DYNAMIC_VULKAN_FUNCTIONS 1 and make sure vkGetInstanceProcAddr and vkGetDeviceProcAddr globals are defined. 
All the remaining Vulkan functions will be fetched automatically.

Finally, you can provide your own pointers to all Vulkan functions needed by VMA using structure member VmaAllocatorCreateInfo::pVulkanFunctions, 
if you fetched them in some custom way e.g. using some loader like Volk.

 * [FROM https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/quick_start.html]
 */
