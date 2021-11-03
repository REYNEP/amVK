#ifndef amVK_IMG_MEM_BUF_MK2_HH
#define amVK_IMG_MEM_BUF_MK2_HH

/** for now the Template below... [static vars] represent a 1 time image that could be used as an ShaderInput, you will need to change it for other kinds of usage */
class ImageMK2 {
  public:
    VkImage               IMG = nullptr;
    VkImageView          VIEW = nullptr;
    VmaAllocation _allocation = nullptr;
    ImageMK2() {}
    ~ImageMK2() {}


    static inline VkImageCreateInfo CI = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO, nullptr, 0,   /** Feel free to change any of these.... before you call create() */
            VK_IMAGE_TYPE_2D, 
            VK_FORMAT_UNDEFINED, {},    /** [.extent] */

            1, 1, /** [.mipLevels], [.arrayLayers] */
            VK_SAMPLE_COUNT_1_BIT,
            
            VK_IMAGE_TILING_OPTIMAL, /** [.tiling], \todo */
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT,      /** [.usage] */
            
            VK_SHARING_MODE_EXCLUSIVE, 0, nullptr, /** [.VkSharingMode] */
            VK_IMAGE_LAYOUT_UNDEFINED   /** [.initialLayout] */
        };
    static inline VkImageViewCreateInfo view_CI = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, nullptr, 0,
            nullptr, VK_IMAGE_VIEW_TYPE_2D,
            VK_FORMAT_UNDEFINED,

            {}, /** [.components] */
            {   /** [.subresourceRange] */
                VK_IMAGE_ASPECT_COLOR_BIT,     /** [.aspectMask] */
                0, 1, /** [.baseMipLevel], [.levelCount] */
                0, 1  /** [.baseArrayLayer], [.layerCount] */
            }
        };
    static inline VmaAllocationCreateInfo alloc_info = {
            0, VMA_MEMORY_USAGE_GPU_ONLY, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT   /** flags, usage, requiredFlags */
            /** more options available, but we ont be needing them */
        };

    static inline amVK_DeviceMK2 *s_amVK_D = nullptr;
    static void set_device(amVK_DeviceMK2 *D) {
        if (D == nullptr) {amVK_SET_activeD(s_amVK_D);}
        else {amVK_CHECK_DEVICE(D, s_amVK_D);}
    }
    


    void create(VkFormat IMG_Format, uint32_t width, uint32_t height) {
        CI.format = IMG_Format;
        view_CI.format = IMG_Format;
        CI.extent = {width, height, 1};
        this->_create();
    }
    /** use this if you want faster perf.... say like when you wanna create same type of image thousands of times.... cz this function doesn't modify Format & extent everytime */
    inline void _create(void)
    {
        #ifndef amVK_RELEASE
            if (s_amVK_D == nullptr) {LOG_EX("call BufferMK2::set_device(); before....");}
        #endif
        VkResult res = vmaCreateImage(s_amVK_D->_allocator, &this->CI, &this->alloc_info, &this->IMG, &this->_allocation, nullptr);

        if (res != VK_SUCCESS) {LOG_DBG(amVK_Utils::vulkan_result_msg(res)); LOG("vmaCreateImage failed [amASSERT]"); amASSERT(true); return;}

        view_CI.image = this->IMG;
        vkCreateImageView(s_amVK_D->_D, &this->view_CI, nullptr, &this->VIEW);
    }
};



/** 
 * \todo stop using VMA, it is safe & standard.... but we wanna be fast, not safe always.... still would be good to keep a fallback option 
 * \todo add support for Big Chunk GPU Allocation but small chunk usage.... & Copy
 * 
 * \note static stuffs inside it is like from the same concept like amVK_GraphicsPipes.... we dont wanna keep Re-Set-ing values that we dont need to
 *          like there could be so many alike format Images.... and we would simply maybe need to just change width & height mostly.... nothing else.... so a universal static one is better
*/
class BufferMK2 {
  public:
    /** CreateInfo */
    static inline VkBufferCreateInfo CI = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, nullptr, 0,
        /* [.size] */0, /* [.usage] */0, /* [.sharingMode]: EXCLUSIVE */{}, 0, nullptr
    };

    /** 
     * Wanted BufferMK2 & ImageMK2 to be really fast.... & Having a amVK_Device as member or parameter felt like waste of CPU time & Memory 
     * \todo consider having a UNIVERSAL one for stuffs in this file...
     */
    static inline amVK_DeviceMK2 *s_amVK_D = nullptr;
    static void set_device(amVK_DeviceMK2 *D) {
        if (D == nullptr) {amVK_SET_activeD(s_amVK_D);}
        else {amVK_CHECK_DEVICE(D, s_amVK_D);}
    }

    VkBuffer _buffer          = nullptr;
    VmaAllocation _allocation = nullptr;
    uint64_t _sizeByte; //maybe deprecate this.... cz the resources e.g. img, or anything really.... will prolly have their own classes. and that will/should have specific sizes. like images have width and height

    BufferMK2(void) {}
    ~BufferMK2(void) {}

    /** \call \fn set_device() before this */
    void create(uint64_t sizeByte, VkBufferUsageFlags usage, VmaMemoryUsage vmaUsage = {})
    {
        #ifndef amVK_RELEASE
            if (s_amVK_D == nullptr) {LOG_EX("call BufferMK2::set_device(); before....");}
        #endif
        _sizeByte = sizeByte;
        CI.size = sizeByte;
        CI.usage = usage;

        VmaAllocationCreateInfo alloc_info = {};
        switch (usage)
        {
            case VK_BUFFER_USAGE_TRANSFER_SRC_BIT:
                alloc_info.usage = VMA_MEMORY_USAGE_CPU_ONLY;
        }

        vmaCreateBuffer(s_amVK_D->_allocator, &CI, &alloc_info,
            &_buffer, &_allocation,
            nullptr
        );
    }

    void copy(const void *from) {
        void* data;
        vmaMapMemory(s_amVK_D->_allocator, _allocation, &data);
        memcpy(data, from, _sizeByte);
        vmaUnmapMemory(s_amVK_D->_allocator, _allocation);
    }
};

#endif //amVK_IMG_MEM_BUF_MK2_HH