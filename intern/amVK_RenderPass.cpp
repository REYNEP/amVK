#define amVK_RENDERPASS_CPP
#include "amVK_RenderPass.hh"

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

        _malloced = true;
    }
}

bool amVK_RenderPassMK2::clean_mods(void) {
    if(_malloced) {free(attachment_descs.data);}
    return true;
}

/**
  \│/  ┌─┐┬ ┬┬─┐┌─┐┌─┐┌─┐┌─┐╔═╗┌─┐┬─┐┌┬┐┌─┐┌┬┐
  ─ ─  └─┐│ │├┬┘├┤ ├─┤│  ├┤ ╠╣ │ │├┬┘│││├─┤ │ 
  /│\  └─┘└─┘┴└─└  ┴ ┴└─┘└─┘╚  └─┘┴└─┴ ┴┴ ┴ ┴ 
 */
#include "amVK_WI.hh"
void amVK_RenderPassMK2::set_surfaceFormat(void) {
    VkSurfaceFormatKHR _final = demoSurfaceExt->filter_SurfaceFormat({final_imageFormat, final_imageColorSpace});

    final_imageFormat = _final.format;
    final_imageColorSpace = _final.colorSpace;
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
