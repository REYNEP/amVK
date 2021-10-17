#ifndef amVK_DEVICE_CPP
#define amVK_DEVICE_CPP
#ifdef amVK_AMD_VMA_SUP
    #define VMA_IMPLEMENTATION
#endif
#include "amVK_Device.hh"
#define IMPL_VEC_amVK_DEVICE  /** \see   class vec_amVK_Device   in amVK_Utils.hh */
#include "amVK_Common.hh"
#include "amVK.hh"

/** Used INTERNALLY amVK_WI::is_present_sup() only for now */
uint32_t amVK_Device::_present_qFam(VkSurfaceKHR S) {
    //if (!found_present_qFamily || (present_qFamily == 0xFFFFFFFF)) {
    // dif window, (has) dif surface can have same device   but checking sup for 1 surface  gives  'Validation Error: [ VUID-VkSwapchainCreateInfoKHR-surface-01270 ]'    
        int32_t PD_index = HEART_CX->PD_to_index(_PD);
        amVK_Array<VkQueueFamilyProperties> qFAM_list = HEART_CX->PD.qFamily_lists[PD_index];
    //
        VkBool32 res = false;
        for (int i = 0, lim = qFAM_list.n; i < lim; i++) {
            vkGetPhysicalDeviceSurfaceSupportKHR(_PD, i, S, &res);
    //
            if (res == true) {
                present_qFamily = i; 
                found_present_qFamily = true;
            }
        }
    //}
    //
    if (present_qFamily == 0xFFFFFFFF) {
        LOG_EX("Couldn't Find any qFamily on PhysicalDevice that supports Presentation to given surface....");
    }
    //
    return present_qFamily;
}   //
/** VALID: only \if you used  amVK_DevicePreset_Flags */
uint32_t amVK_Device::get_graphics_qFamily(void) {amASSERT(!_MODS); return _MODS->_graphics_qFAM;}
VkQueue amVK_Device::get_graphics_queue(void) {amASSERT(!_MODS); VkQueue Q = nullptr; vkGetDeviceQueue(_D, _MODS->_graphics_qFAM, qIndex_MK2, &Q); return Q;}









/**
 * |-----------------------------------------|
 *            - COMMAND BUFFER -
 * |-----------------------------------------|
 * a Good point to start: https://vkguide.dev/docs/chapter-1/vulkan_command_flow/
 * SHORT: Unlike OpenGL or DirectX pre-11, in Vulkan, all GPU commands have to go through a command buffer. 
 *            Command buffers are allocated from a Command Pool, and executed on Queues.
 *            The general flow to execute commands is:
 *              - You allocate a VkCommandBuffer from a VkCommandPool
 *              - You record commands into the command buffer, using VkCmdXXXXX functions.
 *              - You submit the command buffer into a VkQueue, which starts executing the commands.
 *       [- vblanco20-1]
*/

VkCommandPool amVK_Device::init_commandpool(uint32_t qFamily) {
    VkCommandPoolCreateInfo cmdPool_info = {};
        cmdPool_info.sType =  VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmdPool_info.pNext = nullptr;
	    cmdPool_info.queueFamilyIndex = qFamily;
	/** we also want the pool to allow for resetting of individual command buffers 
     *  Lets us use   vkResetCommandBuffer() later On     [ VUID-vkResetCommandBuffer-commandBuffer-00046 ]
     *  */
	    cmdPool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkResult res = vkCreateCommandPool(_D, &cmdPool_info, nullptr, &_CmdPool);
    if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res)); return nullptr;}
    return _CmdPool;
}

/**
 * TODO: You can Allocate multiple cmdBuffer at once
 * \param n: Number of CommandBuffers to alloc
 * \return pointer to the Array of VkCommandBuffer
 */
VkCommandBuffer *amVK_Device::alloc_cmdBuf(uint32_t n) {
    VkCommandBufferAllocateInfo cmdAllocInfo = {};
        cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdAllocInfo.pNext = nullptr;

        /** commands will be made from our _commandPool */
        cmdAllocInfo.commandPool = _CmdPool;
        /** we will allocate 1 command buffer */
        cmdAllocInfo.commandBufferCount = n;
        /** command level is Primary [We not gonna use SECONDARY, which are used in Extreme MultiThreading Optimized scenarios  \see VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT] */
        cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    VkCommandBuffer *cmdBuffer = static_cast<VkCommandBuffer *> (calloc(n, sizeof(VkCommandBuffer)));
    VkResult res = vkAllocateCommandBuffers(_D, &cmdAllocInfo, cmdBuffer);
    if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res)); return nullptr;}
    return cmdBuffer;
}

/**
 * Start Recording into a Particular COMMAND-BUFFER [For 1 Time usage or Multiple-Time usage]
 */
void amVK_Device::begin_cmdBuf(VkCommandBuffer cmdBuf, bool oneTime) {
    /** begin the command buffer recording. */
    VkCommandBufferBeginInfo cmdBeginInfo = {};
        cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmdBeginInfo.pNext = nullptr;
    /** Inheritance info on a command buffer is used for secondary command buffers */
        cmdBeginInfo.pInheritanceInfo = nullptr;
    /** 
     * As we are going to be recording the command buffer every frame, 
     * it’s best if Vulkan knows that this command will only execute once, as it can allow for great optimization by the driver.
     */
    if (oneTime) {
        cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    } else {
        cmdBeginInfo.flags = 0;     //Otherwise, no Need for Optimization ig....
    }

    VkResult res = vkBeginCommandBuffer(cmdBuf, &cmdBeginInfo);
    if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res)); return;}
}

















/**
 * |-----------------------------------------|
 *            - SYNCRONIZATION -
 * |-----------------------------------------|
*/
VkFence amVK_Device::create_fence(void) {
	VkFenceCreateInfo fenceCreateInfo = {};
	    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	    fenceCreateInfo.pNext = nullptr;

	/** so we can wait on it before using it on a GPU command (for the first frame) */
	    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;


    VkFence fence;
	VkResult res = vkCreateFence(_D, &fenceCreateInfo, nullptr, &fence);
    if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res)); return nullptr;}
    return fence;
}

VkSemaphore amVK_Device::create_semaphore(void) {
    /** for the semaphores we don't need any flags */
	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	    semaphoreCreateInfo.pNext = nullptr;
	    semaphoreCreateInfo.flags = 0;

    VkSemaphore sema;
	VkResult res = vkCreateSemaphore(_D, &semaphoreCreateInfo, nullptr, &sema);
    if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res)); return nullptr;}
    return sema;
}








/**
 * |-----------------------------------------|
 *    - AMD VMA (Vulkan Memory Allocator) -
 * |-----------------------------------------|
*/
#include "amVK.hh"

VmaAllocator amVK_Device::init_VMA(void) {
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = _PD;
    allocatorInfo.device = _D;
    allocatorInfo.instance = amVK_CX::instance;
    vmaCreateAllocator(&allocatorInfo, &_allocator);

    return _allocator;
}

#endif //#ifndef amVK_DEVICE_CPP