#pragma once
/** 
 * Well, the Semaphores, Fences & other synchronization stuffs just got its helping functions.... 
 * I dont think this part of amVK is something that you should keep using in a Big project.... its just here to serve as a Booster & template....
 *      and also for the reason that it might help out Vulkan Beginners
 * 
 * \note For now theres no amVK_Sync.cpp everything is here....
 */
#include "amVK_INK.hh"
#include "amVK_Device.hh"

/** 
 * \todo docs, docs & docs.... thats mainly why I created this file... 
 */
class amVK_Semaphore {
  public:
    amVK_DeviceMK2 *amVK_D;
    VkSemaphore SEMA;
    static inline VkSemaphoreCreateInfo s_CI = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, nullptr, 0};

    /** \todo add pNext & flags support  */
    amVK_Semaphore(amVK_DeviceMK2 *D = nullptr) : amVK_D(D) {
        if (D == nullptr) {amVK_SET_activeD(amVK_D);}
        else {amVK_CHECK_DEVICE(D, amVK_D);}

        VkResult res = vkCreateSemaphore(amVK_D->D, &s_CI, nullptr, &SEMA);
        if (res != VK_SUCCESS) {amVK_LOG_EX(amVK_Utils::vulkan_result_msg(res));}
    }
    ~amVK_Semaphore() {}

    bool destroy(void) {
        vkDestroySemaphore(amVK_D->D, SEMA, nullptr);
        return true;
    }
};


/**
 * \todo docs, docs & docs.... thats mainly why I created this file... 
 * Every fench only waits for SINGLE vkCmd** command [Reset the fench before using again ref: https://vkguide.dev/docs/chapter-1/vulkan_mainloop_code/] 
 */
class amVK_Fence {
  public:
    amVK_DeviceMK2 *amVK_D;
    VkFence FENCE;
    static inline VkFenceCreateInfo s_CI = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr, 0};

    /**
     * \param flags: VK_FENCE_CREATE_SIGNALED_BIT - means you wait & reset on it for the first time after creation)
     * \todo add pNext & flags support  
     */
    amVK_Fence(VkFenceCreateFlags flags, amVK_DeviceMK2 *D = nullptr) : amVK_D(D) {
        if (D == nullptr) {amVK_SET_activeD(amVK_D);}
        else {amVK_CHECK_DEVICE(D, amVK_D);}

        s_CI.flags = flags;

        VkResult res = vkCreateFence(amVK_D->D, &s_CI, nullptr, &FENCE);
        if (res != VK_SUCCESS) {amVK_LOG_EX(amVK_Utils::vulkan_result_msg(res));}
    }
    ~amVK_Fence() {}

    bool destroy(void) {
        vkDestroyFence(amVK_D->D, FENCE, nullptr);
        return true;
    }

    bool wait(uint64_t nanosecs = 1000000000) {
        VkResult res = vkWaitForFences(amVK_D->D, 1, &FENCE, true, nanosecs);
        if (res != VK_SUCCESS) {
            amVK_LOG_EX(amVK_Utils::vulkan_result_msg(res));
            
            if (res == VK_TIMEOUT) {
                amVK_LOG_EX("vkWaitForFences(), TimeOut... see stackTrace above    return false;");
            }
            return false;
        }
        else {
            return true;
        }
    }

    bool reset(void) {
        VkResult res = vkResetFences(amVK_D->D, 1, &FENCE);
        if (res != VK_SUCCESS) { amVK_LOG_EX(amVK_Utils::vulkan_result_msg(res)); return false; }
        return true;
    }
};
