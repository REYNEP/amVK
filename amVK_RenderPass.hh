#ifndef amVK_RENDERPASS_H
#define amVK_RENDERPASS_H
#include "vulkan/vulkan.h"

#define amVK_LOGGER_BLI_ASSERT
#include "amVK_Common.hh"
#include "amVK_Device.hh"


/**
 *              █████╗ ███╗   ███╗██╗   ██╗██╗  ██╗        ██████╗ ███████╗███╗   ██╗██████╗ ███████╗██████╗ ██████╗  █████╗ ███████╗███████╗
 *   ▄ ██╗▄    ██╔══██╗████╗ ████║██║   ██║██║ ██╔╝        ██╔══██╗██╔════╝████╗  ██║██╔══██╗██╔════╝██╔══██╗██╔══██╗██╔══██╗██╔════╝██╔════╝
 *    ████╗    ███████║██╔████╔██║██║   ██║█████╔╝         ██████╔╝█████╗  ██╔██╗ ██║██║  ██║█████╗  ██████╔╝██████╔╝███████║███████╗███████╗
 *   ▀╚██╔▀    ██╔══██║██║╚██╔╝██║╚██╗ ██╔╝██╔═██╗         ██╔══██╗██╔══╝  ██║╚██╗██║██║  ██║██╔══╝  ██╔══██╗██╔═══╝ ██╔══██║╚════██║╚════██║
 *     ╚═╝     ██║  ██║██║ ╚═╝ ██║ ╚████╔╝ ██║  ██╗███████╗██║  ██║███████╗██║ ╚████║██████╔╝███████╗██║  ██║██║     ██║  ██║███████║███████║
 *             ╚═╝  ╚═╝╚═╝     ╚═╝  ╚═══╝  ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═══╝╚═════╝ ╚══════╝╚═╝  ╚═╝╚═╝     ╚═╝  ╚═╝╚══════╝╚══════╝
 * ══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════
 *                                            - RENDER PASSES [(Swapchain) FrameBuffer ties to this] -
 * ═══════════════════════════════════════════════════════════════ HIGH LIGHTS ══════════════════════════════════════════════════════════════
 * \brief
 * MSAA [MultiSampling], Depth Stencil, Tiled Rendering, MultiPass [Deferred Rendering]
 * 
 * \note Attachments are what Ties RENDERPASS & Framebuffers Together
 * This class is more like a Struct and is used like that, LINKS: with amVK_WI [ \todo why ]
*/
class amVK_RenderPassMK1 { /** TREAT this like a STRUCT, that you will have to Pass on to other amVK_Classes.... for Auto mods */
  public:
    VkRenderPass           _RP = nullptr;
    amVK_Device       *_amVK_D = nullptr;

    amVK_RenderPassMK1(amVK_Device *D = nullptr) : _amVK_D(D) { 
      if (D == nullptr) {amVK_SET_activeD(_amVK_D);}
      else {amVK_CHECK_DEVICE(D, _amVK_D);}
    }

    VkSampleCountFlagBits            samples = VK_SAMPLE_COUNT_1_BIT;         /** [MSAA MultiSample] \todo Can depthAttachment use DIfferent Sample numebr? */
    VkFormat           final_imageFormat     = amVK_SWAPCHAIN_IMAGE_FORMAT;   /** \see amVK_Types.hh */
    VkColorSpaceKHR    final_imageColorSpace = amVK_SWAPCHAIN_IMAGE_COLORSPACE;

    // depth_format = VK_FORMAT_D32_SFLOAT;
};






struct amVK_SurfaceMK2;
#define amVK_RenderPass amVK_RenderPassMK2
/**
 *             ███╗   ███╗██╗  ██╗██████╗ 
 *   ▄ ██╗▄    ████╗ ████║██║ ██╔╝╚════██╗
 *    ████╗    ██╔████╔██║█████╔╝  █████╔╝
 *   ▀╚██╔▀    ██║╚██╔╝██║██╔═██╗ ██╔═══╝ 
 *     ╚═╝     ██║ ╚═╝ ██║██║  ██╗███████╗
 *             ╚═╝     ╚═╝╚═╝  ╚═╝╚══════╝
 * 
 * \see \fn set_attachments() for Attachment Related Docs
 * \see \fn set_attachment_refs() for attachment Reference related Docs 
 * 
 * attachment_refs are How subpass knows about the Attachments    & ties with the_info.pAttachments index
 */
class amVK_RenderPassMK2 : public amVK_RenderPassMK1 {
  public:
    bool     color_attachment = true;
    bool     depth_attachment = false; /** [DepthAttachment, Depth Stecil] */
    bool       single_subpass = true;  /** For now this is the Only Support.   You can PUSH_BACK your own subpasses EXPLICITLY tho thats what MK2 is about right? */

    uint8_t       color_index = 0;     /** For now, we create Color Attachment anyway,  but amVK_WI::create_framebuffer depends on this */
    uint8_t       depth_index = 0xFF;  /** 0xFF = Error code, variable mainly used for \fn set_subpasses, set in \fn set_attachment_refs */
    /** For now only these 2 are implicitly supported.... & amVK_WI::create_framebuffer depends on this */

    amVK_Array<VkAttachmentDescription>  attachment_descs = {};
    amVK_Array<VkAttachmentReference>     attachment_refs = {};
    amVK_Array<VkSubpassDescription>            subpasses = {};

    amVK_SurfaceMK2 *demoSurfaceExt = nullptr;
    amVK_RenderPassMK2(amVK_SurfaceMK2 *S, amVK_Device *D = nullptr) : amVK_RenderPassMK1(D), demoSurfaceExt(S) {amASSERT(!_amVK_D); amASSERT(!S);}
    /** MK2 MODIFY */
    void calc_n_malloc(void);  //Increments attachment.n  and such amVK_Arrays
    void konfigurieren(void) {/** Do calc_n_malloc() first */set_surfaceFormat(); set_attachments(); set_attachment_refs(); set_subpasses();}

    VkRenderPass create(void) {
      if (attachment_descs.data == nullptr) {calc_n_malloc(); konfigurieren();}
      VkRenderPassCreateInfo the_info = {};
        the_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        the_info.attachmentCount = attachment_descs.n;
        the_info.pAttachments = attachment_descs.data;
        the_info.subpassCount = subpasses.n;
        the_info.pSubpasses = subpasses.data;

      VkResult res = vkCreateRenderPass(_amVK_D->_D, &the_info, nullptr, &_RP);
      amASSERT(res != VK_SUCCESS);
      return _RP;
    }

    /** CLEAN-UP  [DESTRUCTOR] */
    ~amVK_RenderPassMK2() {}
    bool clean_mods(void) {return true;} /** TODO: +  use in calc_n_malloc */

  /** Only call these functions once.... by calling konfigurieren() these can do the basic Setup for you.
   * then you need to PUSH_BACK or MODIFY yourself. thats What MK2 is all about 
   * but you would need to   set values to    'amVK_Array' like attachment_descs.n   before calling konfigurieren().... cz theres a MALLOC happens at first */
  private:
    void set_surfaceFormat(void);   /** validate/fallback-set   \var final_image_format & final_image_colorSpace   from the ones supported for \var demo_surface */
    void set_attachments(void);
    void set_attachment_refs(void); /** \see refs_done variable */
    void set_subpasses(void);
    /** MORE: Automated default stuffs sooooon! */

  /** push_back exts, you should maintain Memory. to determine how many filled, use   \var amVK_Array.next_add_where */
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

#endif //amVK_RENDERPASS_H