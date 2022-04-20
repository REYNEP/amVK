#pragma once

#include "amVK_INK.hh"
#include "amVK_Device.hh"
/** #include "amVK_WI.hh" in .cpp before \fn set_surfaceFormat */


struct amVK_SurfaceMK2;


#define amVK_RenderPass amVK_RenderPassMK2
/**
 *             ██████╗ ███████╗███╗   ██╗██████╗ ███████╗██████╗ ██████╗  █████╗ ███████╗███████╗        ███╗   ███╗██╗  ██╗██████╗ 
 *   ▄ ██╗▄    ██╔══██╗██╔════╝████╗  ██║██╔══██╗██╔════╝██╔══██╗██╔══██╗██╔══██╗██╔════╝██╔════╝        ████╗ ████║██║ ██╔╝╚════██╗
 *    ████╗    ██████╔╝█████╗  ██╔██╗ ██║██║  ██║█████╗  ██████╔╝██████╔╝███████║███████╗███████╗        ██╔████╔██║█████╔╝  █████╔╝
 *   ▀╚██╔▀    ██╔══██╗██╔══╝  ██║╚██╗██║██║  ██║██╔══╝  ██╔══██╗██╔═══╝ ██╔══██║╚════██║╚════██║        ██║╚██╔╝██║██╔═██╗ ██╔═══╝ 
 *     ╚═╝     ██║  ██║███████╗██║ ╚████║██████╔╝███████╗██║  ██║██║     ██║  ██║███████║███████║███████╗██║ ╚═╝ ██║██║  ██╗███████╗
 *             ╚═╝  ╚═╝╚══════╝╚═╝  ╚═══╝╚═════╝ ╚══════╝╚═╝  ╚═╝╚═╝     ╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝╚═╝     ╚═╝╚═╝  ╚═╝╚══════╝
 * ════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════
 *                                            - RENDER PASSES [(Swapchain) FrameBuffer ties to this] -
 * ════════════════════════════════════════════════════════════ HIGH LIGHTS ═══════════════════════════════════════════════════════════
 * \brief
 * MSAA [MultiSampling], Depth Stencil, Tiled Rendering, MultiPass [Deferred Rendering]
 * 
 * [MOTTO: its more about the SubPasses and not about'RenderPass' hype that you get from hearing the name]
 * 
 * 
 * \note Attachments are what Ties RENDERPASS & Framebuffers Together
 * This class is more like a Struct and is used like that, LINKS: with amVK_WI [ \todo why ]
 * 
 * \see \fn set_attachments() for Attachment Related Docs
 * \see \fn set_attachment_refs() for attachment Reference related Docs 
 * 
 * \see create_framebuffers in amVK_WI
 * 
 * attachment_refs are How subpass knows about the Attachments    & ties with the_info.pAttachments index
 * 
 * 
 * 
 * ══════════════════════════════════════════════════════════ EXT HIGH LIGHTS: ═════════════════════════════════════════════════════════
 * \note For Now we only impl. 1 Subpass as Desktop GPUs \todo MUST: 
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
 *        Now whaat can someone mean by 'state of the images'?   
 *        well if you have ever deffered rendering.... or Multiple Passes in Blender.... like Mist, Diffuse, Specular, Volume
 *        its like, in OpenGL People used to implement multiple FRAMEBUFFERS or IMAGEs for each of these Passes.... but this theory of PASSES is slightly by a bit different
 *          cz this one usually has like DEPTH or STENCIL as the other Passes.... then only if the Game Engine author wants the other Passes like from blender to be like that....
 *        
 *        Now each of these passes can depend on each other.... so comes again the idea of States.... if a pass is Done or is in Rendering process.... thats it, yeah....
 * 
 * \see create_framebuffers in amVK_WI
 * 
 * \in main module page.... "This file in Individual.... cz 
 *                              \note it ties to ONLY the Swapchain Framebuffers & Multipass.... (Deffered Rendering & Tiled rendering)   [& slightly to Pipeline a bit]"
 *                              and as this is a new concept in VULKAN, this deserved its very own page for now
 *                           most people tends to get confused over this so i created a Individual File for this....
 *                              People uses classes when they will be using many SIMILAR OBEJCTS, but in our this case, we just want to MAKE sure that this CONCEPT of RenderPass catches Eyes 😉
 *                           make sure that this->_amVK_D matches with the    amVK_WI->_amVK_D
 * 
 * WHY Ties to Pipeline: https://stackoverflow.com/a/55992644   \todo Go Furthur, Invertigator....
 */
class amVK_RenderPassMK2 {
  public:
    VkRenderPass           RP = nullptr;
    amVK_DeviceMK2    *amVK_D = nullptr;

    VkSampleCountFlagBits            samples = VK_SAMPLE_COUNT_1_BIT;         /** [MSAA MultiSample] \todo Can depthAttachment use DIfferent Sample numebr? */
    VkFormat           final_imageFormat     = amVK_SWAPCHAIN_IMAGE_FORMAT;   /** \see amVK_Types.hh */
    VkColorSpaceKHR    final_imageColorSpace = amVK_SWAPCHAIN_IMAGE_COLORSPACE;

    // depth_format = VK_FORMAT_D32_SFLOAT;

    bool     color_attachment = true;
    bool     depth_attachment = false; /** [DepthAttachment, Depth Stecil] */
    bool       single_subpass = true;  /** For now this is the Only Support.   You can PUSH_BACK your own subpasses EXPLICITLY tho thats what MK2 is about right? */

    uint8_t       color_index = 0;     /** For now, we create Color Attachment anyway,  but amVK_WI::create_framebuffer depends on this */
    uint8_t       depth_index = 0xFF;  /** 0xFF = Error code, variable mainly used for \fn set_subpasses, set in \fn set_attachment_refs */
    /** For now only these 2 are implicitly supported.... & amVK_WI::create_framebuffer depends on this */

    bool          is_malloced = false;
    amVK_Array<VkAttachmentDescription>  attachment_descs = {};
    amVK_Array<VkAttachmentReference>     attachment_refs = {};
    amVK_Array<VkSubpassDescription>            subpasses = {};

    /** Needed for choosing a SurfaceFormat.... */
    amVK_SurfaceMK2 *demoSurfaceExt = nullptr;
    amVK_RenderPassMK2(amVK_SurfaceMK2 *S, amVK_DeviceMK2 *D = nullptr) : demoSurfaceExt(S) {
      if (D == nullptr) {amVK_SET_activeD(amVK_D);}
      else {amVK_CHECK_DEVICE(D, amVK_D);}
      amASSERT(!S);
    }
    /** MK2 MODIFY */

    /** \todo make all amvk classes to have malloc as in that exact name */
    void calc_n_alloc(void);  //Increments attachment.n  and such amVK_Arrays
    void konfigurieren(void) {/** Do calc_n_alloc() first */set_surfaceFormat(); set_attachments(); set_attachment_refs(); set_subpasses();}

    VkRenderPass create(void) {
      if (attachment_descs.data == nullptr) {calc_n_alloc(); konfigurieren();}
      
      VkRenderPassCreateInfo the_info = {};
        the_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        the_info.attachmentCount = attachment_descs.n;
        the_info.pAttachments = attachment_descs.data;
        the_info.subpassCount = subpasses.n;
        the_info.pSubpasses = subpasses.data;

      VkResult res = vkCreateRenderPass(amVK_D->D, &the_info, nullptr, &RP);
      _LOG0("RenderPass created! Yes, Time TRAVEL!!!!");
      amASSERT(res != VK_SUCCESS);
      return RP;
    }

    /** CLEAN-UP / DESTRUCTOR */
    ~amVK_RenderPassMK2() {}
    bool destroy(void) {vkDestroyRenderPass(amVK_D->D, RP, nullptr); clean_mods(); return true;}
    bool clean_mods(void);

  /** Only call these functions once.... by calling konfigurieren() these can do the basic Setup for you.
   * then you need to PUSH_BACK or MODIFY yourself. thats What MK2 is all about 
   * but you would need to   set values to    'amVK_Array' like attachment_descs.n   before calling konfigurieren().... cz theres a MALLOC happens at first */
  private:
    void set_surfaceFormat(void);   /** validate/fallback-set   \var final_image_format & final_image_colorSpace,  calls \fn amVK_SurfaceMK2::filter_SurfaceFormat on demoSurfaceExt */
    void set_attachments(void);
    void set_attachment_refs(void); /** \see refs_done variable */
    void set_subpasses(void);
    /** MORE: Automated default stuffs sooooon! */

  /** push_back exts, you should maintain Memory. to determine how many filled, use   \var amVK_Array.neXt */
  public:
    void add_attachmentGEN2(VkFormat image_format, VkImageLayout finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 
                                                   VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, 
                                                 VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE) {
      amVK_ARRAY_PUSH_BACK(attachment_descs) = {0,
        image_format, samples,
        loadOp, storeOp,

        /** [.stencilLoadOp, .stencilStoreOp]  -  This is only used for depth_attachment, \see \fn set_attachments() for DepthStencil */ 
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,

        VK_IMAGE_LAYOUT_UNDEFINED,
        finalLayout
      };
    }
    /** USE: push_back for \var attachment_refs */
    void add_subpass(void) {return;}  /** \todo */
};

#ifdef amVK_RENDERPASS_CPP
/** \todo FIX this.... i think only 1/2 flag will work in this way  used for amVK_WI_MK2::create_framebufs & create_attachment */
VkImageUsageFlags image_layout_2_usageflags(VkImageLayout finalLayout) {
  switch (finalLayout)
  {
    /** These might means smth else... that what I thought these means... */
    case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL: return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL: return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL: return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL:  return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL: return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:  return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  }
}

/** aspectMask is similar to the usageFlags from the image. It’s about what this image is used for. */
VkImageAspectFlags image_layout_2_aspectMask(VkImageLayout finalLayout) {
  switch (finalLayout)
  {
    /** These might means smth else... that what I thought these means... */
    case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL: return VK_IMAGE_ASPECT_DEPTH_BIT;
    case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL: return VK_IMAGE_ASPECT_DEPTH_BIT;
    case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL: return VK_IMAGE_ASPECT_DEPTH_BIT;
    case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL:  return VK_IMAGE_ASPECT_DEPTH_BIT;

    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL: return VK_IMAGE_ASPECT_DEPTH_BIT;
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:  return VK_IMAGE_ASPECT_DEPTH_BIT;

    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: return VK_IMAGE_ASPECT_COLOR_BIT;
  }
}
#else 
  VkImageUsageFlags  image_layout_2_usageflags(VkImageLayout finalLayout);
  VkImageAspectFlags image_layout_2_aspectMask(VkImageLayout finalLayout);
#endif // amVK_RENDERPASS_CPP
