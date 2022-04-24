#include "amVK_WI.hh"
#include "amVK_RenderPass.hh"

/**
  \│/  ┌─┐┬─┐┌─┐┌─┐┌┬┐┌─┐
  ─ ─  │  ├┬┘├┤ ├─┤ │ ├┤ 
  /│\  └─┘┴└─└─┘┴ ┴ ┴ └─┘
 */
amVK_RenderPassMK2::amVK_RenderPassMK2(amVK_SurfaceMK2 *S, amVK_DeviceMK2 *D, uint8_t attachments_n, uint8_t subpasses_n) {
     m_attachment_descs.n = attachments_n;
      m_attachment_refs.n = attachments_n;
            m_subpasses.n = subpasses_n;
            
    if (D == nullptr) {amVK_SET_activeD(amVK_D);}
    else {amVK_CHECK_DEVICE(D, amVK_D);}
    amASSERT(!S);

    demoSurfaceExt = S;
    if (S->S == nullptr) {
        amVK_LOG_EX("[param amVK_SurfaceMK2 S->S] is nullptr");
        demoSurfaceExt = nullptr;
    }
    if (HEART->SPD.index(S->PD) == UINT32_T_NULL) {
        amVK_LOG_EX("[param amVK_SurfaceMK2 S->PD] is nullptr");
        demoSurfaceExt = nullptr;
    }
    if (S->PD != D->PD) {
        amVK_LOG_EX("[param amVK_SurfaceMK2 S->PD != D->PD]");
        demoSurfaceExt = nullptr;
    }
}

/** called by \fn Create_RenderPass(), after \fn konfigurieren() */
bool amVK_RenderPassMK2::_createRenderPass(void) {
    VkRenderPassCreateInfo the_info = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO, nullptr, 0,
        m_attachment_descs.n, m_attachment_descs.data,
               m_subpasses.n, m_subpasses.data,
        0, nullptr      /** SubpassDependency */
    };

    VkResult res = vkCreateRenderPass(amVK_D->D, &the_info, nullptr, &RP);

    if (res) { amVK_LOG_EX(amVK_Utils::vulkan_result_msg(res) << "vkCreateRenderPass() failed!"); return false; }
    _LOG0("RenderPass created! Yes, Time TRAVEL!!!!");
    return true;
}

bool amVK_RenderPassMK2::Destroy_RenderPass(void) {
    vkDestroyRenderPass(amVK_D->D, RP, nullptr);
    if(is_malloced) {free(m_attachment_descs.data);}
    return true;
}







/**
  \│/  ┌┬┐┌─┐┬  ┬  ┌─┐┌─┐
  ─ ─  │││├─┤│  │  │ ││  
  /│\  ┴ ┴┴ ┴┴─┘┴─┘└─┘└─┘
 */
void amVK_RenderPassMK2::calc_n_alloc(void) {
    if (color_attachment) {m_attachment_descs.n++; m_attachment_refs.n++;}
    if (depth_attachment) {m_attachment_descs.n++; m_attachment_refs.n++;}
    if (single_subpass)          {m_subpasses.n++;}

    memory_allocation_malloc:
    {
        if (m_attachment_descs.data != nullptr) {free(m_attachment_descs.data);}

        void *test = malloc(m_attachment_descs.n * sizeof(VkAttachmentDescription)
                           +m_attachment_refs.n * sizeof(VkAttachmentReference)
                           +m_subpasses.n *sizeof(VkSubpassDescription));

       m_attachment_descs.data = static_cast   <VkAttachmentDescription *> (test);
        m_attachment_refs.data = reinterpret_cast<VkAttachmentReference *> (m_attachment_descs.data + m_attachment_descs.n);
              m_subpasses.data = reinterpret_cast <VkSubpassDescription *> ( m_attachment_refs.data + m_attachment_refs.n);

        is_malloced = true;
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
    demoSurfaceExt->filter_SurfaceFormat();
    
    amVK_ARRAY_PUSH_BACK(m_attachment_descs) = {0,   /** [.flags] - not needed for now */
        /** [.format]       - should be linked with/same as swapchainCI.imageFormat */
        demoSurfaceExt->final_imageFormat,

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

        /** [.initialLayout]   - we don't know or care about the starting ImageLayout of the attachment */
        VK_IMAGE_LAYOUT_UNDEFINED,
        /** [.finalLayout]     - after the renderpass ends, the image has to be on a ImageLayout ready for display */
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };


    /**
   * █▀▄ █▀▀ █▀█ ▀█▀ █░█  ▄▀█ ▀█▀ ▀█▀ ▄▀█ █▀▀ █░█ █▀▄▀█ █▀▀ █▄░█ ▀█▀ 
   * █▄▀ ██▄ █▀▀ ░█░ █▀█  █▀█ ░█░ ░█░ █▀█ █▄▄ █▀█ █░▀░█ ██▄ █░▀█ ░█░ 
     */
    if (depth_attachment) { amVK_ARRAY_PUSH_BACK(m_attachment_descs) = {0,
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
    

     * The Vulkan driver will perform the ImageLayout transitions for us when using the renderpass. 
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
    amVK_ARRAY_PUSH_BACK(m_attachment_refs) = { 
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
        amVK_ARRAY_PUSH_BACK(m_attachment_refs) = {depth_index, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
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
    amVK_ARRAY_PUSH_BACK(m_subpasses) = {
        0,                                      /** .flag */
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        0, nullptr,                             /** Input Attachments */
        1, &m_attachment_refs[color_index],     /** Color Attachment  */
        nullptr, nullptr,                       /** Depth & Resolve   */
        0, nullptr                              /** Preserved Attach. */
    };

    VkSubpassDescription *xD = m_subpasses.data + m_subpasses.neXt - 1;
    if (depth_attachment) {
        xD->pDepthStencilAttachment = &m_attachment_refs[depth_index];
    }
}









/** \todo FIX this.... i think only 1/2 flag will work in this way  used for amVK_WI_MK2::create_framebufs & create_attachment */
VkImageUsageFlags image_layout_2_usageflags(VkImageLayout finalLayout) {
    switch (finalLayout)
    {
        /** These might means smth else... that what I thought these means... */
        case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:    return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:    return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:                      return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL:                       return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:              return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:               return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:                               return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }
}

/** aspectMask is similar to the usageFlags from the image. It’s about what this image is used for. */
VkImageAspectFlags image_layout_2_aspectMask(VkImageLayout finalLayout) {
    switch (finalLayout)
    {
        /** These might means smth else... that what I thought these means... */
        case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:    return VK_IMAGE_ASPECT_DEPTH_BIT;
        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:    return VK_IMAGE_ASPECT_DEPTH_BIT;
        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:                      return VK_IMAGE_ASPECT_DEPTH_BIT;
        case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL:                       return VK_IMAGE_ASPECT_DEPTH_BIT;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:              return VK_IMAGE_ASPECT_DEPTH_BIT;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:               return VK_IMAGE_ASPECT_DEPTH_BIT;

        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:                               return VK_IMAGE_ASPECT_COLOR_BIT;
    }
}