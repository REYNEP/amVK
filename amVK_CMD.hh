#ifndef amVK_CMD_HH
#define amVK_CMD_HH

#include "amVK_IN.hh"
#include "amVK_Device.hh"

/** 
 * \todo DO something like in here: https://github.com/inexorgame/vulkan-renderer/tree/master/include/inexor/vulkan-renderer/wrapper/command_buffer.hpp 
 * only here cz, you might wanna write faster code.... This doesn't tie any internal stuffs, not even amVK_CommandPool.... manage that yourself!
 */
class amVK_CommandBuf {
  public:
    VkCommandBuffer _BUF;
    amVK_CommandBuf(VkCommandBuffer BUF) : _BUF(BUF) {}
    ~amVK_CommandBuf() {}


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

        VkResult res = vkBeginCommandBuffer(_BUF, &info);
        if (res != VK_SUCCESS) {
            LOG_EX(amVK_Utils::vulkan_result_msg(res)); 
            return false;
        }
        return true;
    }

    /**
     * \param flags: Possible [VK.1.2] VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT
     */
    inline void Reset(VkCommandBufferResetFlags flags = 0) {
        vkResetCommandBuffer(_BUF, flags);
    }

    inline void End(void) {
        vkEndCommandBuffer(_BUF);
    }
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
 * \todo store the qFamily?
 */
class amVK_CommandPool {
  public:
    amVK_DeviceMK2 *_amVK_D;
    VkCommandPool _POOL;
    amVK_ArrayDYN<VkCommandBuffer> _BUFs = amVK_ArrayDYN<VkCommandBuffer>(4);  /** For Triple buffering someone might just allocate 3.... so we have 4 */



    /** \todo add pNext & flags support, \todo Add support for Multi-Threaded CommandPool    [Start Here: https://stackoverflow.com/questions/53438692/creating-multiple-command-pools-per-thread-in-vulkan]*/
    amVK_CommandPool(uint32_t qFamily, amVK_DeviceMK2 *D = nullptr) : _amVK_D(D) {
        if (D == nullptr) {amVK_SET_activeD(_amVK_D);}
        else {amVK_CHECK_DEVICE(D, _amVK_D);}

        VkCommandPoolCreateInfo CI = {};
            CI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            CI.pNext = nullptr;
            CI.queueFamilyIndex = qFamily;
        /** we also want the pool to allow for resetting of individual command buffers 
         *  Lets us use   vkResetCommandBuffer() later On     [ VUID-vkResetCommandBuffer-commandBuffer-00046 ]*/
            CI.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        VkResult res = vkCreateCommandPool(_amVK_D->_D, &CI, nullptr, &_POOL);
        if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res));}
    }
    ~amVK_CommandPool() {}



    /** \todo use LEVEL_SECONDARY, which are used in Extreme MultiThreading Optimized scenarios  
     * \see VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT] */
    VkCommandBuffer *AllocBufs(uint8_t n) {
        if (n == 0) {
            LOG_EX("param uint8_t n == 0, Defauly choosing 1");
            n = 1;
        } else {
            //LOG("AllocBufs:- " << static_cast<uint32_t>(n));
        }
        VkCommandBufferAllocateInfo I = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, nullptr,
            _POOL, VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            n
        };

        if (_BUFs.neXt + n >= _BUFs.n) {_BUFs.resize();}
        uint32_t new_ones = _BUFs.neXt;
        VkResult res = vkAllocateCommandBuffers(_amVK_D->_D, &I, &_BUFs.data[new_ones]);
        _BUFs.neXt += n;

        if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res)); return nullptr;}
        return &_BUFs.data[new_ones];
    }


    /**
     * Don't Use Destructor, use this.... & also delete this object instance of amVK_CommandPool
     */
    void destroy(void){
        vkFreeCommandBuffers(_amVK_D->_D, _POOL, _BUFs.neXt, _BUFs.data);
        _BUFs._delete();
        vkDestroyCommandPool(_amVK_D->_D, _POOL, nullptr);
    }
};

#endif //amVK_CMD_HH