#pragma once

#include "amVK_INK.hh"
#include "amVK_Device.hh"

/** 
 * \todo DO something like in here: https://github.com/inexorgame/vulkan-renderer/tree/master/include/inexor/vulkan-renderer/wrapper/command_buffer.hpp 
 * only here cz, you might wanna write faster code.... This doesn't tie any internal stuffs, not even amVK_CommandPool.... manage that yourself!
 * 
 * COMMANDPools itself should be multi-threaded... not CommandBuffers....
 */
class amVK_CommandBuf {
  public:
    VkCommandBuffer BUF;
    amVK_CommandBuf(VkCommandBuffer BUF) : BUF(BUF) {}
    ~amVK_CommandBuf() {}

    /** Not sure if we need this here as per OBJECT */
    VkCommandBufferBeginInfo info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr,
        0, nullptr  /** [.flags], [.pInheritanceInfo] secondary cmdbufs */
    };

    /**
     * If u r gonna record cmdbuf every frame, use VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
     * it’s best if Vulkan knows that this command will only execute once, as it can allow for great optimization by the driver.'
     * 
     * \todo secondary cmdbufs
     */
    bool Begin(VkCommandBufferUsageFlags flags) {
        info.flags = flags;

        VkResult res = vkBeginCommandBuffer(BUF, &info);
        if (res != VK_SUCCESS) {
            amVK_LOG_EX(amVK_Utils::vulkan_result_msg(res)); 
            return false;
        }
        return true;
    }

    /**
     * \param flags: Possible [VK.1.2] VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT
     */
    inline void Reset(VkCommandBufferResetFlags flags = 0) { vkResetCommandBuffer(BUF, flags); }
    inline void End(void)                                  {   vkEndCommandBuffer(BUF); }
};


/**
               █████╗ ███╗   ███╗██╗   ██╗██╗  ██╗         ██████╗ ██████╗ ███╗   ███╗███╗   ███╗ █████╗ ███╗   ██╗██████╗ ██████╗  ██████╗  ██████╗ ██╗     
    ▄ ██╗▄    ██╔══██╗████╗ ████║██║   ██║██║ ██╔╝        ██╔════╝██╔═══██╗████╗ ████║████╗ ████║██╔══██╗████╗  ██║██╔══██╗██╔══██╗██╔═══██╗██╔═══██╗██║     
     ████╗    ███████║██╔████╔██║██║   ██║█████╔╝         ██║     ██║   ██║██╔████╔██║██╔████╔██║███████║██╔██╗ ██║██║  ██║██████╔╝██║   ██║██║   ██║██║     
    ▀╚██╔▀    ██╔══██║██║╚██╔╝██║╚██╗ ██╔╝██╔═██╗         ██║     ██║   ██║██║╚██╔╝██║██║╚██╔╝██║██╔══██║██║╚██╗██║██║  ██║██╔═══╝ ██║   ██║██║   ██║██║     
      ╚═╝     ██║  ██║██║ ╚═╝ ██║ ╚████╔╝ ██║  ██╗███████╗╚██████╗╚██████╔╝██║ ╚═╝ ██║██║ ╚═╝ ██║██║  ██║██║ ╚████║██████╔╝██║     ╚██████╔╝╚██████╔╝███████╗
              ╚═╝  ╚═╝╚═╝     ╚═╝  ╚═══╝  ╚═╝  ╚═╝╚══════╝ ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚═╝     ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝╚═════╝ ╚═╝      ╚═════╝  ╚═════╝ ╚══════╝
 * ═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════
 *                                                                     - VkCommandPool -
 * ══════════════════════════════════════════════════════════════════════ HIGH LIGHTS ════════════════════════════════════════════════════════════════════════
 * a Good point to start: https://vkguide.dev/docs/chapter-1/vulkan_command_flow/
 * SHORT: Unlike OpenGL or DirectX pre-11, in Vulkan, all GPU commands have to go through a command buffer. 
 *            Command buffers are allocated from a Command Pool, and executed on Queues.
 *            The general flow to execute commands is:
 *              - You allocate a VkCommandBuffer from a VkCommandPool
 *              - You record commands into the command buffer, using VkCmdXXXXX functions.
 *              - You submit the command buffer into a VkQueue, which starts executing the commands.
 *       [- vblanco20-1]
 * 
 * \todo But you could create Multiple CmdBufs on different Threads [NOTE: vkQueueSubmit is not thread-safe, only one thread can push the commands at a time.]
 *       CMD_Pool should be multithreaded https://stackoverflow.com/questions/38318818/multi-thread-rendering-vs-command-pools
 *                                        https://stackoverflow.com/questions/53438692/creating-multiple-command-pools-per-thread-in-vulkan
 *       \see VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT]
 * \todo store the qFamily?
 * \todo add pNext & flags support
 * \todo CLEAN-UP Allocated buffers.... [its bullshit now]
 * \todo check else_if inside CONSTRUCTOR..... we are having to delete BUFs array.... shitty performance penalty
 * \todo Re-Allocate SUPPORT?
 */
class amVK_CommandPool {
    public:
    static inline VkCommandPoolCreateInfo     s_CI = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, nullptr, 0, 0};
    static inline VkCommandBufferAllocateInfo s_AI = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, nullptr,
        nullptr, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1
    };
    amVK_DeviceMK2      *amVK_D = nullptr;
    VkCommandPool          POOL = nullptr;
    uint32_t          m_qFamily = 0;
    amVK_ArrayDYN<VkCommandBuffer> BUFs;        /** For Triple buffering someone might just allocate 3.... so we have 4 by default in CONSTRUCTOR */

    protected:
    amVK_CommandPool(const amVK_CommandPool&) = delete;             //Brendan's Solution
    amVK_CommandPool& operator=(const amVK_CommandPool&) = delete;  //Brendan's Solution



    public:
    bool Create(uint32_t qFamily, VkCommandPoolCreateFlags flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT) {
        s_CI.queueFamilyIndex = qFamily;
        m_qFamily = qFamily;

        /** we also want the pool to allow for resetting of individual command buffers 
         *  Lets us use   vkResetCommandBuffer() later On     [ VUID-vkResetCommandBuffer-commandBuffer-00046 ]*/
        s_CI.flags = flags;

        VkResult res = vkCreateCommandPool(amVK_D->D, &s_CI, nullptr, &POOL);
        if (res != VK_SUCCESS) {amVK_LOG_EX(amVK_Utils::vulkan_result_msg(res)); return false;}
        return true;
    }

    inline void Reset(VkCommandPoolResetFlags flags = 0) {
        //clear the command pool. This will free the command buffer too
        vkResetCommandPool(amVK_D->D, POOL, flags);
    }

    /** Don't Use Destructor, use this.... & also delete this object instance of amVK_CommandPool */
    void Destroy(void) {
        Reset();
        vkFreeCommandBuffers(amVK_D->D, POOL, BUFs.neXt, BUFs.data);
        BUFs._delete();
        vkDestroyCommandPool(amVK_D->D, POOL, nullptr); 
        POOL = nullptr;
    }





    public:
    /**
     * \param BUFs_n: [DEFAULT: 4] [MAX:256] 
     */
    amVK_CommandPool(amVK_DeviceMK2 *D, uint8_t BUFs_n = 4): BUFs(BUFs_n) {
        if (D == nullptr) {amVK_SET_activeD(amVK_D);}
        else {amVK_CHECK_DEVICE(D, amVK_D);}

        if (BUFs_n == 0) {
            amVK_LOG_EX("param uint8_t n == 0, Defauly choosing 1");
            BUFs_n = 1;
        } else if (BUFs_n > 256) {
            amVK_LOG_EX("[PARAM] n:- " << static_cast<uint32_t>(BUFs_n) << "TooBig [MAX:256]");
            BUFs_n = 256;
            BUFs._delete();
            BUFs = amVK_ArrayDYN<VkCommandBuffer>(256);
        }
    }
    ~amVK_CommandPool() {if (POOL != nullptr) Destroy();}




    /** \todo use LEVEL_SECONDARY, which are used in Extreme MultiThreading Optimized scenarios  
     *   \see VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT] */
    bool alloced = false;
    VkCommandBuffer *Alloc_BUFs(void) {
        if (alloced) {
            amVK_LOG_EX("Already Allocated buffers once on this command pool....");
            return nullptr;
        }
        if (POOL == nullptr) {
            amVK_LOG_EX("You haven't even created the CommandPool yet, dude....");
        }
        
        alloced = true;
        s_AI.commandPool = POOL;

        VkResult res = vkAllocateCommandBuffers(amVK_D->D, &s_AI, BUFs.data);

        if (res != VK_SUCCESS) {amVK_LOG_EX(amVK_Utils::vulkan_result_msg(res)); return nullptr;}
        return BUFs.data;
    }
};
