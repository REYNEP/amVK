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
    uint32_t qFam;  /** Graphics [a.k.a Draw] QueueFamily [qFamily is as per PhysicalDevice (vulkan)] */
    static inline VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;   /** does smth related to synchronization for SubmitInfo SEMAs*/
    VkSubmitInfo _whatever;
    VkQueue _Q;

    amVK_Fence _queueFence;     /** Don't get confused by trying to relate qFam to this.... its like qFam (PhysicalDevice QueueFamily) is like a Group of Queues.... */
    amVK_Semaphore _presentSemaphore;
    amVK_Semaphore  _renderSemaphore;

    amVK_CommandPool _cmdPool;
    amVK_CommandBuf  _cmdBuf;
    /** This can be optimized as hell, I guess.... */
    amVK_RD(amVK_DeviceMK2 *amVK_D) : 
        qFam( amVK_D->get_graphics_qFamily() ),
        _cmdPool( amVK_CommandPool(qFam, amVK_D) ),
        _cmdBuf(  amVK_CommandBuf (*(_cmdPool.AllocBufs(1)))),

        _queueFence(VK_FENCE_CREATE_SIGNALED_BIT, amVK_D),
        _presentSemaphore(amVK_D),
         _renderSemaphore(amVK_D),

        _Q(amVK_D->get_graphics_queue()),
        _whatever({VK_STRUCTURE_TYPE_SUBMIT_INFO, nullptr,
            1, &_presentSemaphore._SEMA, /** wait */
            &waitStage,
            1, &_cmdBuf._BUF,
            1, &_renderSemaphore._SEMA   /** signal */
        })
    {

    }

    /** Current selected RenderWindow */
    static inline amVK_WI_MK2 *last_WI = nullptr;

    /** 
     * Call this, then vkCmdSetViewport, vkCmdSetScissor, 
     *   then  all the vkCmdDraw....
     * Finally EndRecord_N_Submit()
     *   -- calls, amVK_WI_MK2::Present()   [a.k.a vkQueuePresentKHR]
     * 
     * \param WI: Select the window/framebuffer to render to
     */
    void Render_BeginRecord(amVK_WI_MK2 *WI) {
        this->last_WI = WI;
        _queueFence.wait();
        _queueFence.reset();
        WI->AcquireNextImage(_presentSemaphore._SEMA, 1000000000);

        _cmdBuf.Reset(0);
        _cmdBuf.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        WI->Begin_RenderPass(_cmdBuf._BUF, VK_SUBPASS_CONTENTS_INLINE);  // AcquireNextImage() before it.... Framebuffer selected here
    }
    void EndRecord_N_Submit(void) {
        vkCmdEndRenderPass(_cmdBuf._BUF);
        _cmdBuf.End();

        VkResult res = vkQueueSubmit(_Q, 1, &_whatever, _queueFence._FENCE);  /** fenceOne blocks till vkCmd*s finishes [Will get RESET on its own ig] */
        if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res)); return;}

        last_WI->Present(_renderSemaphore._SEMA);   // Finally Present to the Window, a.k.a SwapBuffers or Refresh Screen
    }

  protected:
    amVK_RD(const amVK_RD&) = delete;             //Brendan's Solution
    amVK_RD& operator=(const amVK_RD&) = delete;  //Brendan's Solution
};