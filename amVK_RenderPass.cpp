#define amVK_RENDERPASS_CPP
#include "amVK_RenderPass.hh"
#include "amVK_Device.hh"

/**
 *              █████╗ ███╗   ███╗██╗   ██╗██╗  ██╗        ██████╗ ███████╗███╗   ██╗██████╗ ███████╗██████╗ ██████╗  █████╗ ███████╗███████╗
 *   ▄ ██╗▄    ██╔══██╗████╗ ████║██║   ██║██║ ██╔╝        ██╔══██╗██╔════╝████╗  ██║██╔══██╗██╔════╝██╔══██╗██╔══██╗██╔══██╗██╔════╝██╔════╝
 *    ████╗    ███████║██╔████╔██║██║   ██║█████╔╝         ██████╔╝█████╗  ██╔██╗ ██║██║  ██║█████╗  ██████╔╝██████╔╝███████║███████╗███████╗
 *   ▀╚██╔▀    ██╔══██║██║╚██╔╝██║╚██╗ ██╔╝██╔═██╗         ██╔══██╗██╔══╝  ██║╚██╗██║██║  ██║██╔══╝  ██╔══██╗██╔═══╝ ██╔══██║╚════██║╚════██║
 *     ╚═╝     ██║  ██║██║ ╚═╝ ██║ ╚████╔╝ ██║  ██╗███████╗██║  ██║███████╗██║ ╚████║██████╔╝███████╗██║  ██║██║     ██║  ██║███████║███████║
 *             ╚═╝  ╚═╝╚═╝     ╚═╝  ╚═══╝  ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═══╝╚═════╝ ╚══════╝╚═╝  ╚═╝╚═╝     ╚═╝  ╚═╝╚══════╝╚══════╝
 * ══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════
 *                                              - RENDER PASSES [Ties to (Swapchain) FrameBuffer] -
 * ═══════════════════════════════════════════════════════════════ HIGH LIGHTS ══════════════════════════════════════════════════════════════
 * For Now we only impl. 1 Subpass as Desktop GPUs
 * 
 * [MOTTO: its more about the SubPasses and not about'RenderPass' hype that you get from hearing the name]
 * 
 * [vblanco20-1] [from: https://vkguide.dev/docs/chapter-1/vulkan_renderpass/]
 *     In Vulkan, all of the rendering happens inside a VkRenderPass. 
 *     It is not possible to do rendering commands outside of a renderpass, 
 *     but it is possible to do   Compute commands without them.
 * 
 * MUST-READ: https://gpuopen.com/learn/vulkan-renderpasses/      [They explain whats actually goin on, Explains SubPasses]
 * 
 *     The renderpass is a concept that only exists in Vulkan. 
 *     It’s there because it allows the driver to know more about the state of the images you render.  
 *     [HOW? Bcz of SubPasses, (thats only if... you do use them)]
 * 
 * \see create_framebuffers in amVK_WI
 * 
 * \in main module page.... "This file in Individual.... cz ties to ONLY the Swapchain Framebuffers & Multipass.... (Deffered Rendering & Tiled rendering)"
 *                              and as this is a new concept in VULKAN, this deserved its very own page for now
 *                              most people tends to get confused over this so i created a Individual File for this....
 *                              People uses classes when they will be using many SIMILAR OBEJCTS, but in our this case, we just want to MAKE sure that this CONEPT of RenderPass catches Eyes 😉
 *                           make sure that this->_amVK_D matches with the    amVK_WI->_amVK_D
*/

/**
  \│/  ┌┬┐┌─┐┬  ┬  ┌─┐┌─┐
  ─ ─  │││├─┤│  │  │ ││  
  /│\  ┴ ┴┴ ┴┴─┘┴─┘└─┘└─┘
 */
void amVK_RenderPassMK2::calc_n_malloc(void) {
    if (color_attachment) {attachment_descs.n++; attachment_refs.n++;}
    if (depth_attachment) {attachment_descs.n++; attachment_refs.n++;}
    if (single_subpass) {subpasses.n++;}

    memory_allocation_malloc:
    {
        if (attachment_descs.data != nullptr) {free(attachment_descs.data);}

        void *test = malloc(attachment_descs.n * sizeof(VkAttachmentDescription)
                           +attachment_refs.n * sizeof(VkAttachmentReference)
                           +subpasses.n *sizeof(VkSubpassDescription));

       attachment_descs.data = static_cast   <VkAttachmentDescription *> (test);
        attachment_refs.data = reinterpret_cast<VkAttachmentReference *> (attachment_descs.data + attachment_descs.n);
              subpasses.data = reinterpret_cast <VkSubpassDescription *> ( attachment_refs.data + attachment_refs.n);
    }
}

/**
  \│/  ┌─┐┬ ┬┬─┐┌─┐┌─┐┌─┐┌─┐╔═╗┌─┐┬─┐┌┬┐┌─┐┌┬┐
  ─ ─  └─┐│ │├┬┘├┤ ├─┤│  ├┤ ╠╣ │ │├┬┘│││├─┤ │ 
  /│\  └─┘└─┘┴└─└  ┴ ┴└─┘└─┘╚  └─┘┴└─┴ ┴┴ ┴ ┴ 
 */
#include "amVK_WI.hh"
void amVK_RenderPassMK2::set_surfaceFormat(void) {
    demoSurfaceExt->get_SurfaceFormats();
    amVK_Array<VkSurfaceFormatKHR> surfaceFormats = demoSurfaceExt->surface_formats;
    for (int i = 0; i < surfaceFormats.n; i++) {
        bool found_it = false;
        for (int i = 0; i < surfaceFormats.n; i++) {
            if ((surfaceFormats.data[i].format == final_imageFormat)&&
                (surfaceFormats.data[i].colorSpace == final_imageColorSpace)) {
                found_it = true;
                break;
            }
        }
    }
}

/**
  \│/  ┌─┐┌┬┐┌┬┐┌─┐┌─┐┬ ┬┌┬┐┌─┐┌┐┌┌┬┐┌─┐
  ─ ─  ├─┤ │  │ ├─┤│  ├─┤│││├┤ │││ │ └─┐
  /│\  ┴ ┴ ┴  ┴ ┴ ┴└─┘┴ ┴┴ ┴└─┘┘└┘ ┴ └─┘
 */
void amVK_RenderPassMK2::set_attachments(void) {
    /**
   * █▀▀ █▀█ █░░ █▀█ █▀█  ▄▀█ ▀█▀ ▀█▀ ▄▀█ █▀▀ █░█ █▀▄▀█ █▀▀ █▄░█ ▀█▀ 
   * █▄▄ █▄█ █▄▄ █▄█ █▀▄  █▀█ ░█░ ░█░ █▀█ █▄▄ █▀█ █░▀░█ ██▄ █░▀█ ░█░ 
     */
    amVK_ARRAY_PUSH_BACK(attachment_descs) = {0,   /** [.flags] - not needed for now */
        /** [.format]       - should be linked with/same as swapchainCI.imageFormat */
        final_imageFormat,

        /** [.samples]
         * OVERALL MultiSample Count... e.g. MSAA x8 MSAA x4    from your good ol' games 
         * yes, overall, cz in Pipeline, you got MultiSample settings too! */
        samples,

        /** [.loadOp]
         * TECHNICAL-SPECS: we Clear when this attachment is loaded 
         *      amVK-SPECS: on Nvidia-Drivers, VK_ATTACHMENT_LOAD_OP_DONT_CARE ends up making a MOVING CUBE to a BRUSH */
        VK_ATTACHMENT_LOAD_OP_CLEAR,

        /** [.storeOp]
         * TECHNICAL-SPECS: we keep the attachment stored when the renderpass ends... - v-blanco   
         *        VK-SPECS: [OP_STORE means contents generated during the render pass and within the render area areare written to memory]
         *      amVK-SPECS: Try out VK_ATTACHMENT_STORE_OP_DONT_CARE.... you will see glitches 😉 */
        VK_ATTACHMENT_STORE_OP_STORE,

        /** [.stencilLoadOp, .stencilStoreOp]  -  This is only used for depth_attachment */
        VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VK_ATTACHMENT_STORE_OP_DONT_CARE,

        /** [.initialLayout]   - we don't know or care about the starting layout of the attachment */
        VK_IMAGE_LAYOUT_UNDEFINED,
        /** [.finalLayout]     - after the renderpass ends, the image has to be on a layout ready for display */
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };


    /**
   * █▀▄ █▀▀ █▀█ ▀█▀ █░█  ▄▀█ ▀█▀ ▀█▀ ▄▀█ █▀▀ █░█ █▀▄▀█ █▀▀ █▄░█ ▀█▀ 
   * █▄▀ ██▄ █▀▀ ░█░ █▀█  █▀█ ░█░ ░█░ █▀█ █▄▄ █▀█ █░▀░█ ██▄ █░▀█ ░█░ 
     */
    if (depth_attachment) { amVK_ARRAY_PUSH_BACK(attachment_descs) = {0,
        VK_FORMAT_D32_SFLOAT, samples,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,       /** FOR: depth */
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE,   /** FOR: stencil */

        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    }; }


    /**
      ╻ ╻   ┏┓╻┏━┓╺┳╸┏━╸    ╻  ┏━┓╻ ╻┏━┓╻ ╻╺┳╸
      ╺╋╸   ┃┗┫┃ ┃ ┃ ┣╸ ╹   ┃  ┣━┫┗┳┛┃ ┃┃ ┃ ┃ 
      ╹ ╹   ╹ ╹┗━┛ ╹ ┗━╸╹   ┗━╸╹ ╹ ╹ ┗━┛┗━┛ ╹ 
    

     * The Vulkan driver will perform the layout transitions for us when using the renderpass. 
     * If we weren’t using a renderpass (drawing from compute shaders) we would need to do the same transitions explicitly.
     * 
     * NOTE: IMPORTANT:
     * The image life will go something like this:
     *    UNDEFINED -> RenderPass Begins -> Subpass 0 begins (Layout Transition to Attachment Optimal   [IMPLICIT] ) 
     * -> Subpass 0 renders -> Subpass 0 ends -> Renderpass Ends (Layout Transitions to Present Source  [IMPLICIT] )
     */
}


void amVK_RenderPassMK2::set_attachment_refs(void) {
    uint32_t refs_done = 0;
    /**
   * █▀▀ █▀█ █░░ █▀█ █▀█  ▄▀█ ▀█▀ ▀█▀ ▄▀█ █▀▀ █░█ █▀▄▀█ █▀▀ █▄░█ ▀█▀ 
   * █▄▄ █▄█ █▄▄ █▄█ █▀▄  █▀█ ░█░ ░█░ █▀█ █▄▄ █▀█ █░▀░█ ██▄ █░▀█ ░█░ 
     */
    amVK_ARRAY_PUSH_BACK(attachment_refs) = { 
        color_index, /** [.attachment]    - index number of VkRenderPassCreateInfo.pAttachments that this References to */
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        /** 
         * Seems like you could NOT use any other format.... cz 
         * 1. swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT   that matches with the Layout above
         * 2. VkImageLayout docs says VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL should be used for images created with   [point 1]
         * 3. REF: VUID-VkAttachmentReference-layout-00857
         * 4. Other than that if you keep searching for where to use leftover layouts [& NOT-TO], You'll find those cant atleast be used here....
         */
    };
    refs_done++;


    /**
   * █▀▄ █▀▀ █▀█ ▀█▀ █░█  ▄▀█ ▀█▀ ▀█▀ ▄▀█ █▀▀ █░█ █▀▄▀█ █▀▀ █▄░█ ▀█▀ 
   * █▄▀ ██▄ █▀▀ ░█░ █▀█  █▀█ ░█░ ░█░ █▀█ █▄▄ █▀█ █░▀░█ ██▄ █░▀█ ░█░ 
     */
    if (depth_attachment) {
        depth_index = refs_done;
        amVK_ARRAY_PUSH_BACK(attachment_refs) = {depth_index, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
    }
}


void amVK_RenderPassMK2::set_subpasses(void) {
    /**
      ╻ ╻   ┏━┓╻ ╻┏┓ ┏━┓┏━┓┏━┓┏━┓
      ╺╋╸   ┗━┓┃ ┃┣┻┓┣━┛┣━┫┗━┓┗━┓
      ╹ ╹   ┗━┛┗━┛┗━┛╹  ╹ ╹┗━┛┗━┛
     * Now that our main image target is defined, we need to add a subpass that will render into it.
     * we are going to create 1 subpass, which is the minimum you can do
     */
    VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &attachment_refs[color_index];

    if (depth_attachment) {
        subpass.pDepthStencilAttachment = &attachment_refs[depth_index];
    }

    amVK_ARRAY_PUSH_BACK(subpasses) = subpass;
}
















/**    Without RENDERPASS

Microsoft Windows [Version 10.0.19042.1237]
(c) Microsoft Corporation. All rights reserved.

X:\Current\RTR>cd amVK 

X:\Current\RTR\amVK>rtr
amVK Engine Version 0.0.1b 


15 Instance extensions supported      

All the Instance Extensions:-
VK_KHR_device_group_creation
VK_KHR_display
VK_KHR_external_fence_capabilities    
VK_KHR_external_memory_capabilities   
VK_KHR_external_semaphore_capabilities
VK_KHR_get_display_properties2
VK_KHR_get_physical_device_properties2
VK_KHR_get_surface_capabilities2
VK_KHR_surface
VK_KHR_surface_protected_capabilities
VK_KHR_win32_surface
VK_EXT_debug_report
VK_EXT_debug_utils
VK_EXT_swapchain_colorspace
VK_NV_external_memory_capabilities


SurfaceExts:-
VK_KHR_surface
VK_KHR_win32_surface


Enabled Instance Extensions:-
VK_KHR_surface
VK_KHR_win32_surface
VK_EXT_debug_report
VK_EXT_debug_utils


All Available Layers:- 
VK_LAYER_NV_optimus
VK_LAYER_NV_nomad_release_public_2021_3_1
VK_LAYER_NV_GPU_Trace_release_public_2021_3_1
VK_LAYER_NV_nsight-sys
VK_LAYER_OW_OVERLAY
VK_LAYER_OW_OBS_HOOK
VK_LAYER_VALVE_steam_overlay
VK_LAYER_VALVE_steam_fossilize
VK_LAYER_OBS_HOOK
VK_LAYER_EOS_Overlay
VK_LAYER_EOS_Overlay
VK_LAYER_LUNARG_api_dump
VK_LAYER_LUNARG_device_simulation
VK_LAYER_LUNARG_gfxreconstruct
VK_LAYER_KHRONOS_synchronization2
VK_LAYER_KHRONOS_validation
VK_LAYER_LUNARG_monitor
VK_LAYER_LUNARG_screenshot


Enabled Validation Layers:-
VK_LAYER_KHRONOS_validation


VkInstance Created!
amVK_CX::heart set!


All the Physical Devices:-
NVIDIA GeForce RTX 3060 Ti


BenchMarking NVIDIA GeForce RTX 3060 Ti
GPU SELECTED:- 'NVIDIA GeForce RTX 3060 Ti' Going in For vkCreateDevice
amVK_DeviceMods(amVK_DP_GRAPHICS, 'Physical Device [0000011706E85470] :- 'NVIDIA GeForce RTX 3060 Ti' ')
VkDevice Created! Yessssss, Time Travel! 

Re/Creating swapchain
Swapchain Extent: (984, 681)
------ LIST OF SUPPORTED PRESENTATION-IMAGE FORMATS ------
format: 44, colorSpace: 0
format: 50, colorSpace: 0
format: 97, colorSpace: 1000104002
format: 64, colorSpace: 1000104008
format: 64, colorSpace: 0


create_swapchain PASSED!
UNASSIGNED-GeneralParameterError-RequiredParameter(ERROR / SPEC): msgNum: -1711571459 - Validation Error: [ UNASSIGNED-GeneralParameterError-RequiredParameter ] Object 0: handle = 0x11706e7b518, type = VK_OBJECT_TYPE_DEVICE; | MessageID = 0x99fb7dfd | 
vkCreateFramebuffer: required parameter pCreateInfo->renderPass specified as VK_NULL_HANDLE
    Objects: 1
        [0] 0x11706e7b518, type: 3, name: NULL
VUID-VkFramebufferCreateInfo-renderPass-parameter(ERROR / SPEC): msgNum: -1734171534 - Validation Error: [ VUID-VkFramebufferCreateInfo-renderPass-parameter ] Object 0: handle = 0x11704cf96c0, type = VK_OBJECT_TYPE_INSTANCE; | MessageID = 0x98a2a472 | 
Invalid VkRenderPass Object 0x0. The Vulkan spec states: renderPass must be a valid VkRenderPass handle (https://vulkan.lunarg.com/doc/view/1.2.170.0/windows/1.2-extensions/vkspec.html#VUID-VkFramebufferCreateInfo-renderPass-parameter)
    Objects: 1
        [0] 0x11704cf96c0, type: 1, name: NULL
UNASSIGNED-GeneralParameterError-RequiredParameter(ERROR / SPEC): msgNum: -1711571459 - Validation Error: [ UNASSIGNED-GeneralParameterError-RequiredParameter ] Object 0: handle = 0x11706e7b518, type = VK_OBJECT_TYPE_DEVICE; | MessageID = 0x99fb7dfd | 
vkCreateFramebuffer: required parameter pCreateInfo->renderPass specified as VK_NULL_HANDLE
    Objects: 1
        [0] 0x11706e7b518, type: 3, name: NULL
VUID-VkFramebufferCreateInfo-renderPass-parameter(ERROR / SPEC): msgNum: -1734171534 - Validation Error: [ VUID-VkFramebufferCreateInfo-renderPass-parameter ] Object 0: handle = 0x11704cf96c0, type = VK_OBJECT_TYPE_INSTANCE; | MessageID = 0x98a2a472 | 
Invalid VkRenderPass Object 0x0. The Vulkan spec states: renderPass must be a valid VkRenderPass handle (https://vulkan.lunarg.com/doc/view/1.2.170.0/windows/1.2-extensions/vkspec.html#VUID-VkFramebufferCreateInfo-renderPass-parameter)
    Objects: 1
        [0] 0x11704cf96c0, type: 1, name: NULL
UNASSIGNED-GeneralParameterError-RequiredParameter(ERROR / SPEC): msgNum: -1711571459 - Validation Error: [ UNASSIGNED-GeneralParameterError-RequiredParameter ] Object 0: handle = 0x11706e7b518, type = VK_OBJECT_TYPE_DEVICE; | MessageID = 0x99fb7dfd | 
vkCreateFramebuffer: required parameter pCreateInfo->renderPass specified as VK_NULL_HANDLE
    Objects: 1
        [0] 0x11706e7b518, type: 3, name: NULL
VUID-VkFramebufferCreateInfo-renderPass-parameter(ERROR / SPEC): msgNum: -1734171534 - Validation Error: [ VUID-VkFramebufferCreateInfo-renderPass-parameter ] Object 0: handle = 0x11704cf96c0, type = VK_OBJECT_TYPE_INSTANCE; | MessageID = 0x98a2a472 | 
Invalid VkRenderPass Object 0x0. The Vulkan spec states: renderPass must be a valid VkRenderPass handle (https://vulkan.lunarg.com/doc/view/1.2.170.0/windows/1.2-extensions/vkspec.html#VUID-VkFramebufferCreateInfo-renderPass-parameter)
    Objects: 1
        [0] 0x11704cf96c0, type: 1, name: NULL
Swapchain & FrameBuffer Created
Re/Creating swapchain
Swapchain Extent: (984, 681)
------ LIST OF SUPPORTED PRESENTATION-IMAGE FORMATS ------
format: 44, colorSpace: 0
format: 50, colorSpace: 0
format: 97, colorSpace: 1000104002
format: 64, colorSpace: 1000104008
format: 64, colorSpace: 0


create_swapchain PASSED!
UNASSIGNED-GeneralParameterError-RequiredParameter(ERROR / SPEC): msgNum: -1711571459 - Validation Error: [ UNASSIGNED-GeneralParameterError-RequiredParameter ] Object 0: handle = 0x11706e7b518, type = VK_OBJECT_TYPE_DEVICE; | MessageID = 0x99fb7dfd | 
vkCreateFramebuffer: required parameter pCreateInfo->renderPass specified as VK_NULL_HANDLE
    Objects: 1
        [0] 0x11706e7b518, type: 3, name: NULL
VUID-VkFramebufferCreateInfo-renderPass-parameter(ERROR / SPEC): msgNum: -1734171534 - Validation Error: [ VUID-VkFramebufferCreateInfo-renderPass-parameter ] Object 0: handle = 0x11704cf96c0, type = VK_OBJECT_TYPE_INSTANCE; | MessageID = 0x98a2a472 | 
Invalid VkRenderPass Object 0x0. The Vulkan spec states: renderPass must be a valid VkRenderPass handle (https://vulkan.lunarg.com/doc/view/1.2.170.0/windows/1.2-extensions/vkspec.html#VUID-VkFramebufferCreateInfo-renderPass-parameter)
    Objects: 1
        [0] 0x11704cf96c0, type: 1, name: NULL
UNASSIGNED-GeneralParameterError-RequiredParameter(ERROR / SPEC): msgNum: -1711571459 - Validation Error: [ UNASSIGNED-GeneralParameterError-RequiredParameter ] Object 0: handle = 0x11706e7b518, type = VK_OBJECT_TYPE_DEVICE; | MessageID = 0x99fb7dfd | 
vkCreateFramebuffer: required parameter pCreateInfo->renderPass specified as VK_NULL_HANDLE
    Objects: 1
        [0] 0x11706e7b518, type: 3, name: NULL
VUID-VkFramebufferCreateInfo-renderPass-parameter(ERROR / SPEC): msgNum: -1734171534 - Validation Error: [ VUID-VkFramebufferCreateInfo-renderPass-parameter ] Object 0: handle = 0x11704cf96c0, type = VK_OBJECT_TYPE_INSTANCE; | MessageID = 0x98a2a472 | 
Invalid VkRenderPass Object 0x0. The Vulkan spec states: renderPass must be a valid VkRenderPass handle (https://vulkan.lunarg.com/doc/view/1.2.170.0/windows/1.2-extensions/vkspec.html#VUID-VkFramebufferCreateInfo-renderPass-parameter)
    Objects: 1
        [0] 0x11704cf96c0, type: 1, name: NULL
UNASSIGNED-GeneralParameterError-RequiredParameter(ERROR / SPEC): msgNum: -1711571459 - Validation Error: [ UNASSIGNED-GeneralParameterError-RequiredParameter ] Object 0: handle = 0x11706e7b518, type = VK_OBJECT_TYPE_DEVICE; | MessageID = 0x99fb7dfd | 
vkCreateFramebuffer: required parameter pCreateInfo->renderPass specified as VK_NULL_HANDLE
    Objects: 1
        [0] 0x11706e7b518, type: 3, name: NULL
VUID-VkFramebufferCreateInfo-renderPass-parameter(ERROR / SPEC): msgNum: -1734171534 - Validation Error: [ VUID-VkFramebufferCreateInfo-renderPass-parameter ] Object 0: handle = 0x11704cf96c0, type = VK_OBJECT_TYPE_INSTANCE; | MessageID = 0x98a2a472 | 
Invalid VkRenderPass Object 0x0. The Vulkan spec states: renderPass must be a valid VkRenderPass handle (https://vulkan.lunarg.com/doc/view/1.2.170.0/windows/1.2-extensions/vkspec.html#VUID-VkFramebufferCreateInfo-renderPass-parameter)
    Objects: 1
        [0] 0x11704cf96c0, type: 1, name: NULL
RenderPass wasn't Given
RenderPass wasn't Given
INSIDE LOAD
Shapes: 1
SHAPE 0 Faces: 968 sfv: 2904
OUTSIDE LOAD


the monkey is now in GPU-accessible memory



 ResizED....
0
Re/Creating swapchain
Swapchain Extent: (984, 681)
------ LIST OF SUPPORTED PRESENTATION-IMAGE FORMATS ------
format: 44, colorSpace: 0
format: 50, colorSpace: 0
format: 97, colorSpace: 1000104002
format: 64, colorSpace: 1000104008
format: 64, colorSpace: 0


amVK_WI._IMGs.list isn't nullptr....   seems like its already ALLOCATED!!!!       [we not allocating]
create_swapchain PASSED!
UNASSIGNED-GeneralParameterError-RequiredParameter(ERROR / SPEC): msgNum: -1711571459 - Validation Error: [ UNASSIGNED-GeneralParameterError-RequiredParameter ] Object 0: handle = 0x11706e7b518, type = VK_OBJECT_TYPE_DEVICE; | MessageID = 0x99fb7dfd | 
vkCreateFramebuffer: required parameter pCreateInfo->renderPass specified as VK_NULL_HANDLE
    Objects: 1
        [0] 0x11706e7b518, type: 3, name: NULL
VUID-VkFramebufferCreateInfo-renderPass-parameter(ERROR / SPEC): msgNum: -1734171534 - Validation Error: [ VUID-VkFramebufferCreateInfo-renderPass-parameter ] Object 0: handle = 0x11704cf96c0, type = VK_OBJECT_TYPE_INSTANCE; | MessageID = 0x98a2a472 | 
Invalid VkRenderPass Object 0x0. The Vulkan spec states: renderPass must be a valid VkRenderPass handle (https://vulkan.lunarg.com/doc/view/1.2.170.0/windows/1.2-extensions/vkspec.html#VUID-VkFramebufferCreateInfo-renderPass-parameter)
    Objects: 1
        [0] 0x11704cf96c0, type: 1, name: NULL
UNASSIGNED-GeneralParameterError-RequiredParameter(ERROR / SPEC): msgNum: -1711571459 - Validation Error: [ UNASSIGNED-GeneralParameterError-RequiredParameter ] Object 0: handle = 0x11706e7b518, type = VK_OBJECT_TYPE_DEVICE; | MessageID = 0x99fb7dfd | 
vkCreateFramebuffer: required parameter pCreateInfo->renderPass specified as VK_NULL_HANDLE
    Objects: 1
        [0] 0x11706e7b518, type: 3, name: NULL
VUID-VkFramebufferCreateInfo-renderPass-parameter(ERROR / SPEC): msgNum: -1734171534 - Validation Error: [ VUID-VkFramebufferCreateInfo-renderPass-parameter ] Object 0: handle = 0x11704cf96c0, type = VK_OBJECT_TYPE_INSTANCE; | MessageID = 0x98a2a472 | 
Invalid VkRenderPass Object 0x0. The Vulkan spec states: renderPass must be a valid VkRenderPass handle (https://vulkan.lunarg.com/doc/view/1.2.170.0/windows/1.2-extensions/vkspec.html#VUID-VkFramebufferCreateInfo-renderPass-parameter)
    Objects: 1
        [0] 0x11704cf96c0, type: 1, name: NULL
UNASSIGNED-GeneralParameterError-RequiredParameter(ERROR / SPEC): msgNum: -1711571459 - Validation Error: [ UNASSIGNED-GeneralParameterError-RequiredParameter ] Object 0: handle = 0x11706e7b518, type = VK_OBJECT_TYPE_DEVICE; | MessageID = 0x99fb7dfd | 
vkCreateFramebuffer: required parameter pCreateInfo->renderPass specified as VK_NULL_HANDLE
    Objects: 1
        [0] 0x11706e7b518, type: 3, name: NULL
VUID-VkFramebufferCreateInfo-renderPass-parameter(ERROR / SPEC): msgNum: -1734171534 - Validation Error: [ VUID-VkFramebufferCreateInfo-renderPass-parameter ] Object 0: handle = 0x11704cf96c0, type = VK_OBJECT_TYPE_INSTANCE; | MessageID = 0x98a2a472 | 
Invalid VkRenderPass Object 0x0. The Vulkan spec states: renderPass must be a valid VkRenderPass handle (https://vulkan.lunarg.com/doc/view/1.2.170.0/windows/1.2-extensions/vkspec.html#VUID-VkFramebufferCreateInfo-renderPass-parameter)
    Objects: 1
        [0] 0x11704cf96c0, type: 1, name: NULL
Re/Creating swapchain
Swapchain Extent: (984, 681)
------ LIST OF SUPPORTED PRESENTATION-IMAGE FORMATS ------
format: 44, colorSpace: 0
format: 50, colorSpace: 0
format: 97, colorSpace: 1000104002
format: 64, colorSpace: 1000104008
format: 64, colorSpace: 0


amVK_WI._IMGs.list isn't nullptr....   seems like its already ALLOCATED!!!!       [we not allocating]
create_swapchain PASSED!
UNASSIGNED-GeneralParameterError-RequiredParameter(ERROR / SPEC): msgNum: -1711571459 - Validation Error: [ UNASSIGNED-GeneralParameterError-RequiredParameter ] Object 0: handle = 0x11706e7b518, type = VK_OBJECT_TYPE_DEVICE; | MessageID = 0x99fb7dfd | 
vkCreateFramebuffer: required parameter pCreateInfo->renderPass specified as VK_NULL_HANDLE
    Objects: 1
        [0] 0x11706e7b518, type: 3, name: NULL
VUID-VkFramebufferCreateInfo-renderPass-parameter(ERROR / SPEC): msgNum: -1734171534 - Validation Error: [ VUID-VkFramebufferCreateInfo-renderPass-parameter ] Object 0: handle = 0x11704cf96c0, type = VK_OBJECT_TYPE_INSTANCE; | MessageID = 0x98a2a472 | 
Invalid VkRenderPass Object 0x0. The Vulkan spec states: renderPass must be a valid VkRenderPass handle (https://vulkan.lunarg.com/doc/view/1.2.170.0/windows/1.2-extensions/vkspec.html#VUID-VkFramebufferCreateInfo-renderPass-parameter)
    Objects: 1
        [0] 0x11704cf96c0, type: 1, name: NULL
UNASSIGNED-GeneralParameterError-RequiredParameter(ERROR / SPEC): msgNum: -1711571459 - Validation Error: [ UNASSIGNED-GeneralParameterError-RequiredParameter ] Object 0: handle = 0x11706e7b518, type = VK_OBJECT_TYPE_DEVICE; | MessageID = 0x99fb7dfd | 
vkCreateFramebuffer: required parameter pCreateInfo->renderPass specified as VK_NULL_HANDLE
    Objects: 1
        [0] 0x11706e7b518, type: 3, name: NULL
VUID-VkFramebufferCreateInfo-renderPass-parameter(ERROR / SPEC): msgNum: -1734171534 - Validation Error: [ VUID-VkFramebufferCreateInfo-renderPass-parameter ] Object 0: handle = 0x11704cf96c0, type = VK_OBJECT_TYPE_INSTANCE; | MessageID = 0x98a2a472 | 
Invalid VkRenderPass Object 0x0. The Vulkan spec states: renderPass must be a valid VkRenderPass handle (https://vulkan.lunarg.com/doc/view/1.2.170.0/windows/1.2-extensions/vkspec.html#VUID-VkFramebufferCreateInfo-renderPass-parameter)
    Objects: 1
        [0] 0x11704cf96c0, type: 1, name: NULL
UNASSIGNED-GeneralParameterError-RequiredParameter(ERROR / SPEC): msgNum: -1711571459 - Validation Error: [ UNASSIGNED-GeneralParameterError-RequiredParameter ] Object 0: handle = 0x11706e7b518, type = VK_OBJECT_TYPE_DEVICE; | MessageID = 0x99fb7dfd | 
vkCreateFramebuffer: required parameter pCreateInfo->renderPass specified as VK_NULL_HANDLE
    Objects: 1
        [0] 0x11706e7b518, type: 3, name: NULL
VUID-VkFramebufferCreateInfo-renderPass-parameter(ERROR / SPEC): msgNum: -1734171534 - Validation Error: [ VUID-VkFramebufferCreateInfo-renderPass-parameter ] Object 0: handle = 0x11704cf96c0, type = VK_OBJECT_TYPE_INSTANCE; | MessageID = 0x98a2a472 | 
Invalid VkRenderPass Object 0x0. The Vulkan spec states: renderPass must be a valid VkRenderPass handle (https://vulkan.lunarg.com/doc/view/1.2.170.0/windows/1.2-extensions/vkspec.html#VUID-VkFramebufferCreateInfo-renderPass-parameter)
    Objects: 1
        [0] 0x11704cf96c0, type: 1, name: NULL
Swapchain & FrameBuffer reCreated
UNASSIGNED-GeneralParameterError-RequiredParameter(ERROR / SPEC): msgNum: -1711571459 - Validation Error: [ UNASSIGNED-GeneralParameterError-RequiredParameter ] Object 0: handle = 0x11706e7b518, type = VK_OBJECT_TYPE_DEVICE; | MessageID = 0x99fb7dfd | 
vkCmdBeginRenderPass: required parameter pRenderPassBegin->renderPass specified as VK_NULL_HANDLE
    Objects: 1
        [0] 0x11706e7b518, type: 3, name: NULL
VUID-VkRenderPassBeginInfo-renderPass-parameter(ERROR / SPEC): msgNum: 1375448893 - Validation Error: [ VUID-VkRenderPassBeginInfo-renderPass-parameter ] Object 0: handle = 0x11704cf96c0, type = VK_OBJECT_TYPE_INSTANCE; | MessageID = 0x51fbaf3d | Invalid VkRenderPass Object 0x0. The Vulkan spec states: renderPass must be a valid VkRenderPass handle (https://vulkan.lunarg.com/doc/view/1.2.170.0/windows/1.2-extensions/vkspec.html#VUID-VkRenderPassBeginInfo-renderPass-parameter)
    Objects: 1
        [0] 0x11704cf96c0, type: 1, name: NULL
 */