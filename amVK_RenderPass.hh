#ifndef amVK_RENDERPASS
#define amVK_RENDERPASS
#include "vulkan/vulkan.h"

#define amVK_LOGGER_BLI_ASSERT
#include "amVK_Logger.hh"
/** \todo Gonna separte amVK_CX stuffs. into another file.... or just the amVK Comm */
#include "amVK.hh"
#ifndef amVK_DEVICE_HH
  class amVK_Device;
#endif


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
*/
class amVK_RenderPass { /** TREAT this like a STRUCT, that you will have to Pass on to other amVK_Classes.... for Auto mods */
  public:
    VkRenderPass     _RP = nullptr;
    amVK_Device *_amVK_D = nullptr;

    amVK_RenderPass(amVK_Device *D = nullptr) : _amVK_D(D) { 
      if (D == nullptr) {amVK_SET_activeD(_amVK_D);}
      else {amVK_CHECK_DEVICE(D, _amVK_D);}
    }

    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;     /** [MSAA MultiSample] \todo Can depthAttachment use DIfferent Sample numebr? */
    VkFormat         image_format = amVK_SWAPCHAIN_IMAGE_FORMAT;
    // depth_format = VK_FORMAT_D32_SFLOAT;
};


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
 */
class amVK_RenderPassMK2 : public amVK_RenderPass {
  public:
    bool     color_attachment = true;
    bool     depth_attachment = false; /** [DepthAttachment, Depth Stecil] */
    bool       single_subpass = true;  /** For now this is the Only Support.   You can PUSH_BACK your own subpasses EXPLICITLY tho thats what MK2 is about right? */

    uint8_t       color_index = 0;     /** For now, we create Color Attachment anyway */
    uint8_t       depth_index = 0xFF;  /** 0xFF = Error code, variable mainly used for set_subpasses */

    amVK_Array<VkAttachmentDescription>   attachments = {};
    amVK_Array<VkAttachmentReference> attachment_refs = {};  /** refs are How subpass knows about the Attachments */
    amVK_Array<VkSubpassDescription>        subpasses = {};

    amVK_RenderPassMK2(amVK_Device *D = nullptr) : amVK_RenderPass(D) {amASSERT(!_amVK_D);}
    /** MK2 MODIFY */
    void configure_n_malloc(void);  //Increments attachment.n  and such amVK_Arrays
    void do_everything(void) {if (attachments.data == nullptr) {configure_n_malloc();}; set_attachments(); set_attachment_refs(); set_subpasses();}

    VkRenderPass create(void) {
      if (attachments.data == nullptr) {do_everything();}
      VkRenderPassCreateInfo the_info = {};
        the_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        the_info.attachmentCount = attachments.n;
        the_info.pAttachments = attachments.data;
        the_info.subpassCount = subpasses.n;
        the_info.pSubpasses = subpasses.data;

      VkResult res = vkCreateRenderPass(_amVK_D->_D, &the_info, nullptr, &_RP);
      return _RP;
    }

    /** CLEAN-UP  [DESTRUCTOR] */
    ~amVK_RenderPassMK2() {}
    bool clean_mods(void) {return true;} /** TODO: +  use in configure_n_malloc */

  /** Only call these functions once.... by calling do_everything these can do the basic Setup for you.
   * then you need to PUSH_BACK or MODIFY yourself. thats What MK2 is all about 
   * but you would need to   set values to    'amVK_Array' like attachments.n   before calling do_everything().... cz theres a MALLOC happens at first */
  private:
    void set_attachments(void);
    void set_attachment_refs(void); //Note: \see refs_done
    void set_subpasses(void);
};

#endif //amVK_RENDERPASS