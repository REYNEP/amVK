#pragma once
#include "amVK_Device.hh"
#include "amVK_CMD.hh"
#include "amVK_Sync.hh"

#define REY_EnzoFernandez

class Enzo_Fernandez {
    public:
 /**amVK_DeviceMK2     *amVK_D;      USE 'CMD_Pool.amVK_D' */
    amVK_CommandPool           CMDPool;
    amVK_Fence                  QFence;   /**  'Queue Fence' for vkQueueSubmit  */
    VkCommandBuffer       ActiveCMDBuf = nullptr;
    VkCommandBufferBeginInfo CMDBuf_BI = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr,
        0, nullptr          /** [.flags], [.pInheritanceInfo] secondary cmdbufs */
    };

  protected:
    Enzo_Fernandez(const Enzo_Fernandez&) = delete;             //Brendan's Solution
    Enzo_Fernandez& operator=(const Enzo_Fernandez&) = delete;  //Brendan's Solution

  public:
    Enzo_Fernandez(amVK_DeviceMK2 *amVK_D, uint8_t CMDBuf_n) : CMDPool(amVK_D, CMDBuf_n), QFence(amVK_D) {}
   ~Enzo_Fernandez() {}
   





    /**
     * \param flags: If u r gonna record cmdbuf every frame, use VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
     * 
     *     it’s best if Vulkan knows that this command will only execute once, as it can allow for great optimization by the driver.'
     * 
     * \todo secondary cmdbufs
     * \todo choose qFamily automatically based on TYPE... will require support on amVKDevice side
     */
    void Enzo_ThePlan(uint32_t qFamily, VkCommandBufferUsageFlags CMDBuf_BeginFlags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VkFenceCreateFlags FenceFlags = 0) {
        CMDPool.Create(qFamily);
        CMDPool.Alloc_BUFs();
        ActiveCMDBuf = CMDPool.BUFs.data[0];

        CMDBuf_BI.flags = CMDBuf_BeginFlags;

        QFence.Create(FenceFlags);
        Set_BarrierQFamily(qFamily);
    }

    bool Activate_CMDBuf(uint32_t n) {if (n <= CMDPool.BUFs.size()) ActiveCMDBuf = CMDPool.BUFs.data[n];}

    bool    Begin_CMDBuf(VkCommandBufferUsageFlags flags) {
        VkResult res = vkBeginCommandBuffer(ActiveCMDBuf, &CMDBuf_BI);
        if (res != VK_SUCCESS) {amVK_LOG_EX(amVK_Utils::vulkan_result_msg(res)); return false;}
        return true;
    }
    /**
     * \param flags: Possible [VK.1.2] VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT
     */
    inline void Reset_CMDBuf(VkCommandBufferResetFlags flags = 0) {vkResetCommandBuffer(ActiveCMDBuf, flags);}
    inline void End_CMDBuf(void) {                                   vkEndCommandBuffer(ActiveCMDBuf);       }




  public:
    VkSubmitInfo              Enzo_SUB = {VK_STRUCTURE_TYPE_SUBMIT_INFO, nullptr,
        0, nullptr,         /**  waitSemaphore  */
        nullptr,            /** piepelineWaitStage related to sema */
        1, &ActiveCMDBuf,
        0, nullptr          /** signalSemaphore */
    };
    inline bool Set_SubmitSemaphores(uint8_t wait_n, VkSemaphore *to_wait, uint8_t signal_n, VkSemaphore *to_signal) {
        Enzo_SUB.waitSemaphoreCount = wait_n;
        Enzo_SUB.pWaitSemaphores = to_wait;
        Enzo_SUB.signalSemaphoreCount = signal_n;
        Enzo_SUB.pSignalSemaphores = to_signal;
        return true;
    }
    inline bool Set_SubmitSemaphores(amVK_Array<VkSemaphore> toWaitON, amVK_Array<VkSemaphore>toSignal) {
        Enzo_SUB.waitSemaphoreCount = toWaitON.neXt;
        Enzo_SUB.pWaitSemaphores = toWaitON.data;
        Enzo_SUB.signalSemaphoreCount = toSignal.neXt;
        Enzo_SUB.pSignalSemaphores = toSignal.data;
        return true;
    }
    inline bool QSubmit(VkQueue Q) {
        VkResult res = vkQueueSubmit(Q, 1, &Enzo_SUB, QFence.FENCE);
        if (res != VK_SUCCESS) {amVK_LOG_EX(amVK_Utils::vulkan_result_msg(res)); return false;}

        vkWaitForFences(CMDPool.amVK_D->D, 1, &QFence.FENCE, true, 1000000000);
        vkResetFences(CMDPool.amVK_D->D, 1, &QFence.FENCE);
        return true;
    }

    static inline VkImageMemoryBarrier s_toTransfer = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER, nullptr, 
        0, VK_ACCESS_TRANSFER_WRITE_BIT, /** [.srcAccessMark], [.dstAccessMask] */
        VK_IMAGE_LAYOUT_UNDEFINED, 
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        0, 0,   /** [.srcQueueFamilyIndex], [.dstQueueFamilyIndex] 
                 * its safe to update this to the qFamily that the CmdPool is gonna be created for
                 */

        /** [.image] & [.subresourceRange] */
        nullptr,
        {
            VK_IMAGE_ASPECT_COLOR_BIT,     /** [.aspectMask] */
            0, 1,       /** [.baseMipLevel],   [.levelCount] */
            0, 1        /** [.baseArrayLayer], [.layerCount] */
        }
    };
    static inline VkImageMemoryBarrier s_toReadable = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER, nullptr, 
        VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, /** [.srcAccessMark], [.dstAccessMask] */
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        0, 0,   /** [.srcQueueFamilyIndex], [.dstQueueFamilyIndex] 
                 * its safe to update this to the qFamily that the CmdPool is gonna be created for
                 */

        /** [.image] & [.subresourceRange] */
        nullptr,
        s_toTransfer.subresourceRange
    };
    static inline void Set_BarrierQFamily(uint32_t xd) {
        s_toReadable.srcQueueFamilyIndex = xd;
        s_toReadable.dstQueueFamilyIndex = xd;

        s_toTransfer.srcQueueFamilyIndex = xd;
        s_toTransfer.dstQueueFamilyIndex = xd;
    }

    static inline VkBufferImageCopy s_copyRegion = {
        0, 0, 0, /** [.bufferOffset], [.bufferRowLength], [.bufferImageHeight]  */
        {
            VK_IMAGE_ASPECT_COLOR_BIT,     /** [.aspectMask] */
            0,          /** [.mipLevel]                      */
            0, 1        /** [.baseArrayLayer], [.layerCount] */
        },

        /** [.imageOffset], [.imageExtent] */
        {0, 0, 0},
        {100, 100, 1}
    };

    /** 
     * NOTE: BUFFER CAN"T BE less than IMG_SIZE
     * https://stackoverflow.com/a/65987352 :- Reading this feels like.... a Barrier only applies to this 1 Time Submit below.... this Barrier doesn't affect in other CmdBufs
     *    just Assume what I said was true, then start reading articles.... things will make more sense
     *    Say you got a lot of images to upload.... using vkCmdCopyBufferToImage.... now GPU is Highly Parrallelized, things could overlap one another. thats why you gotta barrier just like you would do a MUTEX */
    void CopyBufferToImage(VkImage IMG, VkBuffer BUFFER, uint32_t width, uint32_t height) {
        s_toTransfer.image = IMG;
        vkCmdPipelineBarrier(ActiveCMDBuf, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &s_toTransfer);

        /** copy the buffer into the image */
        s_copyRegion.imageExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1};
        vkCmdCopyBufferToImage(ActiveCMDBuf, BUFFER, IMG, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &s_copyRegion);

        s_toReadable.image = IMG;
        vkCmdPipelineBarrier(ActiveCMDBuf, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &s_toReadable);
    }
};