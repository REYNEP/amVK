#pragma once

#include "amVK_INK.hh"
#include "amVK_Device.hh"


class MemoryMK1 {
    public:
    amVK_DeviceMK2 *amVK_D = nullptr;  /** has PhysicalDevice [PD] */
    VkDeviceMemory M = nullptr;

    MemoryMK1(amVK_DeviceMK2 *amVK_D) : amVK_D(amVK_D) {}
    bool choose_memory_heap(bool DeviceLocal, bool BiggestOne, bool HostVisible) {
        return false;
    }
    ~MemoryMK1() {}
};


/** 
 *   Wanted BufferMK2 & ImageMK2 to be really fast.... 
 * & Having a amVK_Device as member or parameter felt like waste of CPU time & Memory
 * 
 * \todo gotta change how activate_device acts.... an universal way would be better
 */
class amVK_ImgNBuf_Kernal {
  public:
    static inline amVK_DeviceMK2 *s_amVK_D = nullptr;
    static void set_device(amVK_DeviceMK2 *D) {
        if (D == nullptr) {amVK_SET_activeD(s_amVK_D);}
        else {amVK_CHECK_DEVICE(D, s_amVK_D);}
    }
};

#define S_amVK_DEVICE amVK_ImgNBuf_Kernal::s_amVK_D







/** 
 * \todo add support for Big Chunk GPU Allocation but small chunk usage.... & Copy
 * 
 * \note static stuffs inside it is like from the same concept like amVK_GraphicsPipes.... we dont wanna keep Re-Set-ing values that we dont need to
 *          like there could be so many alike format Images.... and we would simply maybe need to just change width & height mostly.... nothing else.... so a universal static one is better
*/
class BufferMK2 {
  public:       /** TODO: make this private ???? but its used in ImageMK2:: amVKCreateImage2 */
    MemoryMK1 MEMORY     = nullptr;
    uint64_t m_sizeByte  = 0;
    uint64_t m_memOffset = 0;
  public:
    VkBuffer BUFFER    = nullptr;

    static inline VkBufferCreateInfo s_CI = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, nullptr, 0,
        /* [.size] */0, /* [.usage] */0, /* [.sharingMode]: EXCLUSIVE */{}, 0, nullptr
    };

    BufferMK2(void) {}
    ~BufferMK2(void) {}

    /** 
     * ALLOCATES & BINDS VkMemory underneath....
     * \call \fn set_device() before this
     * 
     * TODO: INVESTIGATE: What did AMD show us as 256MB here? https://www.youtube.com/watch?v=zSG6dPq57P8&t=308s
     */
    void amvkCreateBuffer(uint64_t sizeByte, VkBufferUsageFlags usage)
    {
        if (S_amVK_DEVICE == nullptr) {amVK_LOG_EX("call amVK_ImgNBuf_Kernal::set_device();");}
        m_sizeByte = sizeByte;
        s_CI.size = sizeByte;
        s_CI.usage = usage;

        VkMemoryPropertyFlags flags = 0;
        switch (usage)
        {
            case VK_BUFFER_USAGE_TRANSFER_SRC_BIT:
                flags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;    // 😉 means that GPU can read from here
            case VK_BUFFER_USAGE_VERTEX_BUFFER_BIT:
                flags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            case VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT:
                flags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        }

        vkCreateBuffer(S_amVK_DEVICE->D, &s_CI, nullptr, &BUFFER);
        MEMORY.M = S_amVK_DEVICE->BindBufferMemory(BUFFER, flags);
    }

    /** \todo VkMemoryMapFlags */
    void amvkCopyBuffer(const void *from) {
        void* data;
        vkMapMemory(  S_amVK_DEVICE->D, MEMORY.M, 0, m_sizeByte, 0, &data);
        memcpy(data, from, m_sizeByte);
        vkUnmapMemory(S_amVK_DEVICE->D, MEMORY.M);
    }

    void amvkDestroyBuffer(void) {
        vkDestroyBuffer(S_amVK_DEVICE->D, BUFFER, nullptr);
        vkFreeMemory(   S_amVK_DEVICE->D, MEMORY.M, nullptr);
    }
};







/** 
 * for now the Template below... [static vars] represent a 1 time image that could be used as an ShaderInput, you will need to change it for other kinds of usage 
 * 
 * NOTE: Images has to be vkCmdCopyBufferToImage too!   soooo...  we implemented amVKCreateImage2.... it shouldn't be needin' that
 */
class ImageMK2 {
  public:
    VkImage               IMG = nullptr;
    VkImageView          VIEW = nullptr;
    VkDeviceMemory     MEMORY = nullptr;
    ImageMK2() {}
    ~ImageMK2() {}

    static inline VkImageCreateInfo s_CI = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO, nullptr,     /** Feel free to change any of these.... before you call amvkCreateImage() */
        0,                                      /** .imageCreateFlags   check: VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT  */
        VK_IMAGE_TYPE_2D,                       /** 1D/2D/3D */
        VK_FORMAT_UNDEFINED, {},                /** [.extent] */

        1, 1,                                   /** [.mipLevels], [.arrayLayers] */
        VK_SAMPLE_COUNT_1_BIT,
        
        VK_IMAGE_TILING_OPTIMAL,                /** [.tiling], \todo */
        VK_IMAGE_USAGE_TRANSFER_DST_BIT |       /** [.usage] : keep when you gonna do vkCmdCopyBufferToImage [a.k.a Texture Inputs], also SEE `VK_IMAGE_USAGE_SAMPLED_BIT` */
        VK_IMAGE_USAGE_SAMPLED_BIT,              /** */
        
        VK_SHARING_MODE_EXCLUSIVE, 0, nullptr,  /** [.VkSharingMode] */
        VK_IMAGE_LAYOUT_UNDEFINED               /** [.initialLayout] */
    };
    static inline VkImageViewCreateInfo s_view_CI = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, nullptr, 
        0,                                      /** .imageCreateFlags   check: VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT  */
        nullptr, VK_IMAGE_VIEW_TYPE_2D,
        VK_FORMAT_UNDEFINED,

        {}, /** [.components] */
        {   /** [.subresourceRange] */
            VK_IMAGE_ASPECT_COLOR_BIT,     /** [.aspectMask] */
            0, 1, /** [.baseMipLevel], [.levelCount] */
            0, 1  /** [.baseArrayLayer], [.layerCount] */
        }
    };

    /** Doesn't Bind its very own memory.... doesn't work... cz vkCmdCopyBufferToImage.... is a must anyway */
    void amvkCreateImage2(VkFormat IMG_Format, uint32_t width, uint32_t height, BufferMK2 B) {
        if (S_amVK_DEVICE == nullptr) {amVK_LOG_EX("call amVK_ImgNBuf_Kernal::set_device(); before this.... ");}

        s_CI.format = IMG_Format;
        s_view_CI.format = IMG_Format;
        s_CI.extent = {width, height, 1};
        s_CI.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
        s_CI.tiling = VK_IMAGE_TILING_LINEAR;
        s_CI.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;

        VkResult res = vkCreateImage(S_amVK_DEVICE->D, &s_CI, nullptr, &this->IMG);
        
        s_view_CI.image = this->IMG;
        vkBindImageMemory(S_amVK_DEVICE->D, IMG, B.MEMORY.M, 0);

        res = vkCreateImageView(S_amVK_DEVICE->D, &this->s_view_CI, nullptr, &this->VIEW);

        s_CI.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        s_CI.tiling = VK_IMAGE_TILING_OPTIMAL;
    }

    inline void amvkCreateImage(VkFormat IMG_Format, uint32_t width, uint32_t height) {
        s_CI.format = IMG_Format;
        s_view_CI.format = IMG_Format;
        s_CI.extent = {width, height, 1};
        this->_amvkCreateImage(false);
    }
    /**
     * you might not want to modify this static CI.... in that case we have this option 
     * use this if you want faster perf.... say like when you wanna create same type of image thousands of times.... cz this function doesn't modify Format & extent everytime 
     * 
     * USES: \fn s_amVK_D->BindImageMemoryOpt(),   but BindImageMemory() has to be called befre the 'opt' once.
     *          so call \fn amvkDestroyImage() once before.
     * 
     * \param last_format_n_extent_n_other_stuffs: VkDeviceMemory size is assumed to be same inside \fn BindImageMemoryOpt
     */
    void _amvkCreateImage(bool last_format_n_extent_n_other_stuffs = false) {
        if (S_amVK_DEVICE == nullptr) {amVK_LOG_EX("call BufferMK2::set_device(); before this.... ");}

        VkResult res = vkCreateImage(S_amVK_DEVICE->D, &s_CI, nullptr, &this->IMG);
        if (last_format_n_extent_n_other_stuffs)
            MEMORY = S_amVK_DEVICE->BindImageMemoryOpt(this->IMG);   //allocate & bind new memory for image.
        else 
            MEMORY = S_amVK_DEVICE->BindImageMemory(this->IMG);

        s_view_CI.image = this->IMG;
        vkCreateImageView(S_amVK_DEVICE->D, &this->s_view_CI, nullptr, &this->VIEW);
    }

    /** 
     * \param CreateInfo: VkImageCreateInfo*
     * \param view_CreateInfo: VkImageViewCreateInfo*
     * \param amVK_D: def-value ImageMK2::s_amVK_D   [use set_device(); or s_amVK_D is nullptr]
    */
    void amvkCreateImage(VkImageCreateInfo *CreateInfo, VkImageViewCreateInfo *view_CreateInfo, amVK_DeviceMK2 *amVK_D = S_amVK_DEVICE) {
        if (amVK_D == nullptr) {amVK_LOG_EX("call BufferMK2::set_device(); before.... this   or pass amVK_DeviceMK2 as param");}

        VkResult res = vkCreateImage(amVK_D->D, CreateInfo, nullptr, &this->IMG);
        MEMORY = amVK_D->BindImageMemory(this->IMG);     //allocate & bind new memory for image.

        view_CreateInfo->image = this->IMG;
        vkCreateImageView(amVK_D->D, view_CreateInfo, nullptr, &this->VIEW);
    }

    inline void amvkDestroyImage(amVK_DeviceMK2 *amVK_D = S_amVK_DEVICE) {
        vkDestroyImageView(amVK_D->D, this->VIEW,   nullptr);
        vkDestroyImage(    amVK_D->D, this->IMG,    nullptr);
        vkFreeMemory(      amVK_D->D, this->MEMORY, nullptr);
    }

    static inline bool amvkDestroyImage(VkImage img, VkImageView view, VkDeviceMemory memory, amVK_DeviceMK2 *amVK_D = S_amVK_DEVICE) {
        vkDestroyImageView(amVK_D->D, view,   nullptr);
        vkDestroyImage(    amVK_D->D, img,    nullptr);
        vkFreeMemory(      amVK_D->D, memory, nullptr);
        return true;
    }
};

/** 
 * About Tiling.... \todo DOCS, Docs Docs
 * 
 *  Tiling is very important. Tiling describes how the data for the texture is arranged in the GPU. 
 *  For improved performance, GPUs do not store images as 2d arrays of pixels, 
 *  but instead use complex custom formats, unique to the GPU brand and even models. 
 *  VK_IMAGE_TILING_OPTIMAL tells vulkan to let the driver decide how the GPU arranges the memory of the image. 
 *  If you use VK_IMAGE_TILING_OPTIMAL, it won’t be possible to read the data from CPU or to write it without changing its tiling first 
 *  (it’s possible to change the tiling of a texture at any point, but this can be a costly operation). 
 *  The other tiling we can care about is VK_IMAGE_TILING_LINEAR, which will store the image as a 2d array of pixels. 
 *  While LINEAR tiling will be a lot slower, it will allow the cpu to safely write and read from that memory.   
 * 
 *      - vblanco20-1 
 */


















#undef S_amVK_DEVICE
