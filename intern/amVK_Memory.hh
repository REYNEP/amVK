#ifndef amVK_MEMORY_HH
#define amVK_MEMORY_HH
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

#endif  //amVK_MEMORY_HH