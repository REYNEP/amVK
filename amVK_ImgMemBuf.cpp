#include "amVK_ImgMemBuf.hh"
#include "amVK_Logger.hh"

/** TODO: INVESTIGATE: What did AMD show us as 256MB here? https://www.youtube.com/watch?v=zSG6dPq57P8&t=308s
 *  TODO: have size and usage as member variables of class   [maybe even buf_data?]
 * Some options like size could and should be used as Function PARAMS... change of less runtime errors */
void amVK_Buffer::upload_new_gpu(amVK_Device *device, size_t buf_size, VkBufferUsageFlags buf_usage, void *buf_data) {
    //allocate vertex buffer
    VkBufferCreateInfo buf_CI = {};
        buf_CI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        //this is the total size, in bytes, of the buffer we are allocating    [NOTE: REYNEP   Does 8/16Byte Alignment happen?]
        buf_CI.size = buf_size;
        //eg. VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
        buf_CI.usage = buf_usage;

    /** [TODO: Not Hardcore this?]   let the VMA library know that this data should be writeable by CPU, but also readable by GPU */
    VmaAllocationCreateInfo alloc_CI = {};
        alloc_CI.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;



    //allocate the buffer
    VkResult res = vmaCreateBuffer(device->_allocator, 
        &buf_CI,     &alloc_CI, 
        &_buffer, &_allocation, nullptr
    );

    if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res)); LOG("vmaCreateBuffer failed. inside upload_new_gpu. This is a WIP");}



    void* data;
    /** What we intended to do all along   WHY_UNMAP or confused by any chance    \see amVK_ImgMemBuf.md */
      /** To push data into a VkBuffer, we need to map it first. Mapping a buffer will give us a pointer (data here), and then we can write into it. */
        vmaMapMemory(device->_allocator, _allocation, &data);
    /** Now that we got a memory spot for our vertex data, we copy from our _vertices vector into this GPU-readable data.
     *  To copy the data, we use memcpy directly. Note that it’s not necessary to use memcpy, but in many implementations memcpy will be the fastest way to copy a chunk of memory.
        *  Maybe we could Tryc creating Our very own MEMBPY Library, that also works with VMA */
        memcpy(data, buf_data, buf_size);
    /**  When we are done with the writing, we unmap the data. It is possible to keep the pointer mapped and not unmap it immediately, 
        *   but that is an advanced technique mostly used for streaming data, which we don’t need right now.
        *   Mapping and then unmapping the pointer lets the driver know that the write is finished, and will be safer. */
        vmaUnmapMemory(device->_allocator, _allocation);
}

void amVK_Buffer::destroy(amVK_Device *device) {
    vmaDestroyBuffer(device->_allocator, _buffer, _allocation);
}


bool amVK_Buffer::alloc_GPU(amVK_Device *device, size_t buf_size, VkBufferUsageFlags buf_usage) {
    //allocate vertex buffer
    VkBufferCreateInfo buf_CI = {};
        buf_CI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        //this is the total size, in bytes, of the buffer we are allocating    [NOTE: REYNEP   Does 8/16Byte Alignment happen?]
        buf_CI.size = buf_size;
        //eg. VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
        buf_CI.usage = buf_usage;

    /** [TODO: Not Hardcore this?]   let the VMA library know that this data should be writeable by CPU, but also readable by GPU */
    VmaAllocationCreateInfo alloc_CI = {};
        alloc_CI.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;



    //allocate the buffer
    VkResult res = vmaCreateBuffer(device->_allocator, 
        &buf_CI,     &alloc_CI, 
        &_buffer, &_allocation, nullptr
    );

    if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res)); LOG("vmaCreateBuffer failed. inside upload_new_gpu. This is a WIP"); return false;}
    return true;
}