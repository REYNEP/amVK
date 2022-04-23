#pragma once

/** a Prototype Renderer... */

#include "amVK_CMD.hh"
#include "amVK_Sync.hh"
#include "amVK_WI.hh"


/**
 * amVK_RendererT1, I usually name Test or Demo stuffs with a 'T' and a Number
 * [see Docs in amVK_Renderer.hh]
 * GOAL: amVK is not a Renderer.... you are supposed to make your own using the stuffs that amVK has
 *       People are actually gonna get confused to see that amVK doesn't have any renderer.... 
 *       so we got this file.... to short describe how Rendering in Vulkan works
 * 
 * \brief
 * How you render in Vulkan....
 *      - After you have created a amVK_Renderpass, amVK_WI & amVK_WI::create_Swapchain.... now you can go ahead and Render in your MainLoop
 *      - in Vulkan, GPU commands e.g. VkCmdDraw, VkCmdCopyBufferToImage, has to be Recorded into a CommandBuffer.... then you simply submit that CommandBuffer to an actual VkQueue on the GPU
 * 
 * NOTE: Render_BeginRecord & EndRecord_N_Submit is supposed to be inside MainLoop.... that means, yes, we're recording the CommandBuffer everyFrame
 *       TARGET for U: make your own 'Draw'/'Renderer' class that records once but.... submits & presents many times
 *
 * This class is supposed to serve as a Default thing to just Boost you up.... you should create your own 'Draw' / 'Render' Class!
 */
class amVK_RD {
  public:
    uint32_t m_qFam;  /** Graphics [a.k.a Draw] QueueFamily [qFamily is as per PhysicalDevice (vulkan)] */
    static inline VkPipelineStageFlags s_waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;   /** does smth related to synchronization for SubmitInfo SEMAs*/
    VkSubmitInfo m_whatever;
    VkQueue m_Q;

    amVK_Fence m_queueFence;     /** Don't get confused by trying to relate qFam to this.... its like qFam (PhysicalDevice QueueFamily) is like a Group of Queues.... */
    amVK_Semaphore m_presentSemaphore;
    amVK_Semaphore  m_renderSemaphore;

    amVK_CommandPool m_cmdPool;
    amVK_CommandBuf  m_cmdBuf;
    /** This can be optimized as hell, I guess.... */
    amVK_RD(amVK_DeviceMK2 *amVK_D) : 
        m_qFam( amVK_D->get_graphics_qFamily() ),
        m_cmdPool( amVK_CommandPool(m_qFam, amVK_D) ),
        m_cmdBuf(  amVK_CommandBuf (*(m_cmdPool.alloc_Bufs(1)))),

        m_queueFence(VK_FENCE_CREATE_SIGNALED_BIT, amVK_D),
        m_presentSemaphore(amVK_D),
         m_renderSemaphore(amVK_D),

        m_Q(amVK_D->get_graphics_queue()),
        m_whatever({VK_STRUCTURE_TYPE_SUBMIT_INFO, nullptr,
            1, &m_presentSemaphore.SEMA, /** wait */
            &s_waitStage,
            1, &m_cmdBuf.BUF,
            1, &m_renderSemaphore.SEMA   /** signal */
        })
    {

    }

    /** Current selected RenderWindow */
    static inline amVK_WI_MK2 *s_last_WI = nullptr;

    /** 
     * Call this, then vkCmdSetViewport, vkCmdSetScissor, 
     *   then  all the vkCmdDraw....
     * Finally EndRecord_N_Submit()
     *   -- calls, amVK_WI_MK2::Present()   [a.k.a vkQueuePresentKHR]
     * 
     * \param WI: Select the window/framebuffer to render to
     */
    void Render_BeginRecord(amVK_WI_MK2 *WI) {
        this->s_last_WI = WI;
        m_queueFence.wait();
        m_queueFence.reset();
        WI->AcquireNextImage(m_presentSemaphore.SEMA, 1000000000);

        m_cmdBuf.reset(0);
        m_cmdBuf.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        WI->Begin_RenderPass(m_cmdBuf.BUF, VK_SUBPASS_CONTENTS_INLINE);  // AcquireNextImage() before it.... Framebuffer selected here
    }
    void EndRecord_N_Submit(void) {
        vkCmdEndRenderPass(m_cmdBuf.BUF);
        m_cmdBuf.end();

        VkResult res = vkQueueSubmit(m_Q, 1, &m_whatever, m_queueFence.FENCE);  /** fenceOne blocks till vkCmd*s finishes [Will get RESET on its own ig] */
        if (res != VK_SUCCESS) {amVK_LOG_EX(amVK_Utils::vulkan_result_msg(res)); return;}

        s_last_WI->Present(m_renderSemaphore.SEMA);   // Finally Present to the Window, a.k.a SwapBuffers or Refresh Screen
    }

  protected:
    amVK_RD(const amVK_RD&) = delete;             //Brendan's Solution
    amVK_RD& operator=(const amVK_RD&) = delete;  //Brendan's Solution
};
