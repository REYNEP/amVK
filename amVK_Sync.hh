#ifndef amVK_SYNC_HH
#define amVK_SYNC_HH
/** 
 * Well, the Semaphores, Fences & other synchronization stuffs just got its helping functions.... 
 * I dont think this part of amVK is something that you should keep using in a Big project.... its just here to serve as a Booster & template....
 *      and also for the reason that it might help out Vulkan Beginners
 * 
 * \note For now theres no amVK_Sync.cpp everything is here....
 */
#include "amVK_IN.hh"
#include "amVK_Device.hh"

/** 
 * \todo docs, docs & docs.... thats mainly why I created this file... 
 */
class amVK_Semaphore {
  public:
    amVK_DeviceMK2 *_amVK_D;
    VkSemaphore _SEMA;

    /** \todo add pNext & flags support  */
    amVK_Semaphore(amVK_DeviceMK2 *D = nullptr) : _amVK_D(D) {
        if (D == nullptr) {amVK_SET_activeD(_amVK_D);}
        else {amVK_CHECK_DEVICE(D, _amVK_D);}

        VkSemaphoreCreateInfo CI = {};
            CI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            CI.pNext = nullptr;
            CI.flags = 0;

        VkResult res = vkCreateSemaphore(_amVK_D->_D, &CI, nullptr, &_SEMA);
        if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res));}
    }
    ~amVK_Semaphore() {}

    bool destroy(void) {
        vkDestroySemaphore(_amVK_D->_D, _SEMA, nullptr);
        return true;
    }
};


/**
 * \todo docs, docs & docs.... thats mainly why I created this file... 
 * Every fench only waits for SINGLE vkCmd** command [Reset the fench before using again ref: https://vkguide.dev/docs/chapter-1/vulkan_mainloop_code/] 
 */
class amVK_Fence {
  public:
    amVK_DeviceMK2 *_amVK_D;
    VkFence _FENCE;

    /**
     * \param flags: defult: VK_FENCE_CREATE_SIGNALED_BIT - means you wait & reset on it for the first time after creation)
     * \todo add pNext & flags support  
     */
    amVK_Fence(VkFenceCreateFlags flags = VK_FENCE_CREATE_SIGNALED_BIT, amVK_DeviceMK2 *D = nullptr) : _amVK_D(D) {
        if (D == nullptr) {amVK_SET_activeD(_amVK_D);}
        else {amVK_CHECK_DEVICE(D, _amVK_D);}

        VkFenceCreateInfo CI = {};
            CI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            CI.pNext = nullptr;
            CI.flags = flags;

        VkResult res = vkCreateFence(_amVK_D->_D, &CI, nullptr, &_FENCE);
        if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res));}
    }
    ~amVK_Fence() {}

    bool destroy(void) {
        vkDestroyFence(_amVK_D->_D, _FENCE, nullptr);
        return true;
    }

    bool wait(uint64_t nanosecs = 1000000000) {
        VkResult res = vkWaitForFences(_amVK_D->_D, 1, &_FENCE, true, nanosecs);
        if (res != VK_SUCCESS) {
            LOG_EX(amVK_Utils::vulkan_result_msg(res));
            
            if (res == VK_TIMEOUT) {
                LOG_DBG("vkWaitForFences(), TimeOut... see stackTrace above    return false;");
            }
            return false;
        }
        else {
            return true;
        }
    }

    bool reset(void) {
        VkResult res = vkResetFences(_amVK_D->_D, 1, &_FENCE);
        if (res != VK_SUCCESS) { LOG_EX(amVK_Utils::vulkan_result_msg(res)); return false; }
        return true;
    }
};

#endif //amVK_SYNC_HH