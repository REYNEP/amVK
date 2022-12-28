#pragma once

/** a Prototype Renderer... */

#include "amVK_CMD.hh"
#include "amVK_Sync.hh"
#include "amVK_WI.hh"
#include "Enzo_Fernandez.hh"

#define amVK_RD Lautaro_Martinez 

/**
 * REMEMBER This is just a Wrapper around ENZO_FERNANDEZ
 * 
 * amVK_RendererT1, I usually name Test or Demo stuffs with a 'T' and a Number
 * [see Docs in amVK_Renderer.hh]
 * GOAL: amVK is not a Renderer.... you are supposed to make your own using the stuffs that amVK has
 *       People are actually gonna get confused to see that amVK doesn't have any renderer.... 
 *       so we got this file.... to short describe how Rendering in Vulkan works
 * 
 * \brief
 * How you render in Vulkan....
 *      - After you have created a amVK_Renderpass, amVK_WI & amVK_WI::Create_Swapchain.... now you can go ahead and Render in your MainLoop
 *      - in Vulkan, GPU commands e.g. VkCmdDraw, VkCmdCopyBufferToImage, has to be Recorded into a CommandBuffer.... then you simply submit that CommandBuffer to an actual VkQueue on the GPU
 * 
 * NOTE: Render_BeginRecord & EndRecord_N_Submit is supposed to be inside MainLoop.... that means, yes, we're recording the CommandBuffer everyFrame
 *       TARGET for U: make your own 'Draw'/'Renderer' class that records once but.... submits & presents many times
 *
 * This class is supposed to serve as a Default thing to just Boost you up.... you should create your own 'Draw' / 'Render' Class!
 */
class Lautaro_Martinez: public Enzo_Fernandez {     /** More like Lautaro is getting that PASS from Enzo.... not like... Lautaro INHERIts Enzo.... */
  public:
    VkQueue Q = nullptr;
    amVK_Semaphore PresentSEMA;
    amVK_Semaphore  RenderSEMA;
    static inline VkPipelineStageFlags s_waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;   /**   does smth related to synchronization for SubmitInfo SEMAs  */
    static inline amVK_WI_MK2           *s_last_WI = nullptr;                                         /** Current selected RenderWindow, \see \fn Render_BeginRecord() */

  protected:
    Lautaro_Martinez(const Lautaro_Martinez&) = delete;             //Brendan's Solution
    Lautaro_Martinez& operator=(const Lautaro_Martinez&) = delete;  //Brendan's Solution

  public:
    Lautaro_Martinez(amVK_DeviceMK2 *amVK_D): Enzo_Fernandez(amVK_D, 4), PresentSEMA(amVK_D), RenderSEMA(amVK_D) {}
   ~Lautaro_Martinez() {}

    /** NOTE: Call Enzo_ThePlan too */
    inline void Lautaro_ThePlan(VkQueue Queue) {
        Q = Queue;
        PresentSEMA.Create();
        RenderSEMA.Create();
        Enzo_Fernandez::Set_SubmitSemaphores(1, &PresentSEMA.SEMA, 1, &RenderSEMA.SEMA);
        Enzo_Fernandez::Enzo_SUB.pWaitDstStageMask = &s_waitStage;
    }

    


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
        Enzo_Fernandez::QFence.Wait();
        Enzo_Fernandez::QFence.Reset();
        WI->AcquireNextImage(PresentSEMA.SEMA, 1000000000);

        Enzo_Fernandez::Reset_CMDBuf(0);
        Enzo_Fernandez::Begin_CMDBuf(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        WI->Begin_RenderPass(Enzo_Fernandez::ActiveCMDBuf, VK_SUBPASS_CONTENTS_INLINE);  // AcquireNextImage() before it.... Framebuffer selected here
    }
    void EndRecord_N_Submit(void) {
        vkCmdEndRenderPass(Enzo_Fernandez::ActiveCMDBuf);
        Enzo_Fernandez::End_CMDBuf();

        VkResult res = vkQueueSubmit(Q, 1, &(Enzo_Fernandez::Enzo_SUB), Enzo_Fernandez::QFence.FENCE);  /** fenceOne blocks till vkCmd*s finishes [Will get RESET on its own ig] */
        if (res != VK_SUCCESS) {amVK_LOG_EX(amVK_Utils::vulkan_result_msg(res)); return;}

        s_last_WI->PresentNextImage(RenderSEMA.SEMA);   // Finally Present to the Window, a.k.a SwapBuffers or Refresh Screen
    }

    void Just_Acquire(amVK_WI_MK2 *WI) {
        this->s_last_WI = WI;
        Enzo_Fernandez::QFence.Wait();
        Enzo_Fernandez::QFence.Reset();
        WI->AcquireNextImage(PresentSEMA.SEMA, 1000000000);
    }
    void Just_Submit(void) {
        VkResult res = vkQueueSubmit(Q, 1, &(Enzo_Fernandez::Enzo_SUB), Enzo_Fernandez::QFence.FENCE);  /** fenceOne blocks till vkCmd*s finishes [Will get RESET on its own ig] */
        if (res != VK_SUCCESS) {amVK_LOG_EX(amVK_Utils::vulkan_result_msg(res)); return;}

        s_last_WI->PresentNextImage(RenderSEMA.SEMA);   // Finally Present to the Window, a.k.a SwapBuffers or Refresh Screen
    }
};
