#ifndef amVK_IMG_MEM_BUF_H
#define amVK_IMG_MEM_BUF_H

#include "vulkan/vulkan.h"

#include "amVK_IN.hh"
#include "amVK_Device.hh"


typedef struct amVK_Image {
    VkImage             _img;
    VkImageView     _imgView;
    VmaAllocation allocation;
} imgBuf;

imgBuf new_image(amVK_DeviceMK2 *D, VkImageCreateInfo *dImage_CI);

/** 
 * NOTE: We didn't want to exclusively add any member such as  'amVK_DeviceMK2', 'size' or 'usage' or 'data'
 *    Cz it was intended to be a member of other classes as MESH/OBJECT     [not to be a pointer]
 * RECOMMENDED: you should have amVK_Buffer as a Member of some SuperSeeding kinda class   e.g.  MESH/OBJECT   kinda stuffs
 *      + Using stuffs as Function Params means less chance of Memory errors
 */
class amVK_Buffer
{
  public:
    VkBuffer _buffer          = nullptr;
    VmaAllocation _allocation = nullptr;

    /** Create/Allocate NEW Buffer on GPU (CPU_TO_GPU)    & Upload/memcpy data into it */
    /** \param device is needed for    device->allocator     however you can think of BUFFERs as per Device.... so this param is not that bad idea*/
    void upload_new_gpu(amVK_DeviceMK2 *device, size_t buf_size, VkBufferUsageFlags buf_usage, void *buf_data);
    void destroy(amVK_DeviceMK2 *device);
    bool alloc_GPU(amVK_DeviceMK2 *device, size_t buf_size, VkBufferUsageFlags buf_usage);   //CPU_TO_GPU
};

#endif //amVK_IMG_MEM_BUF_H