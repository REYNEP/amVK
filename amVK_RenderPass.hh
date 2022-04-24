#pragma once

#include "amVK_INK.hh"
#include "amVK_Device.hh"


VkImageUsageFlags  image_layout_2_usageflags(VkImageLayout finalLayout);
VkImageAspectFlags image_layout_2_aspectMask(VkImageLayout finalLayout);


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
 *    RenderPassMK2 is where you choose Format & ColorSpace.... 
 *    amVK_WI takes RenderPass in... & uses those formats at `the_info` & at \fn create_Attachments()
 * 
 *  - amVK_Surface is 1 of the ties between amVK_RenderPass & amVK_WI    (RenderPass n' Swapchain)
 *  - The other one is ImageAttachments + their Format & Colorspace
 * 
 *  you can use same RenderPass for different Windows
 *  it may seem that RenderPass is as per vkSurface.
 *  But, its not like that. 
 *  In an OS, its likely that different windows, will have same kinda surface
 *  in here, we only need to check if the device supports presenting to the surface
 *  & if for choosing surfaceFormat, a.k.a ImageFormat & imageColorSpace
 * 
 * TAGS:
 * MSAA [MultiSampling], Depth Stencil, Tiled Rendering, MultiPass [Deferred Rendering], DepthAttachment,
 * 
 * \note you can do     m_attachment_descs.n = 10;    before    konfigurieren().... and it will work
 * 
 * SUBPASS-NOTE: For now single_subpass is the Only Support.   You can PUSH_BACK your own subpasses EXPLICITLY tho thats what MK2 is about right?
 * 
 * color_index: For now, we create Color Attachment anyway,      but amVK_WI::create_framebuffer depends on this
 * depth_index: For now, only these 2 are implicitly supported.... & amVK_WI::create_framebuffer depends on this
 * 
 * \todo: FIX: image_layout_2_usageflags() & image_layout_2_aspectMask()   currently only supports 1 TYPE....
 * \todo make all amvk classes to have malloc as in that exact name
 * \todo MORE: Automated default stuffs sooooon!
 * \todo Can depthAttachment use DIfferent [MSAA] Sample numebr? 
 * \todo add_subpass  [for now USE: push_back for \var attachment_refs]
 * \todo add support for extra allocation parameters in konfigurieren
 * \note For Now we only impl. 1 Subpass as Desktop GPUs \todo MUST:
 */
class amVK_RenderPassMK2 {
    public:
    VkRenderPass                        RP = nullptr;
    amVK_DeviceMK2                 *amVK_D = nullptr;
    amVK_SurfaceMK2        *demoSurfaceExt = nullptr;  /** Needed for choosing a SurfaceFormat.... */


    public:
    /** calls konfigurieren [if not called] */
    bool Create_RenderPass(void) {
        if (m_attachment_descs.data == nullptr) {konfigurieren();}
        return _createRenderPass();
    }
    bool _createRenderPass(void);
    bool Destroy_RenderPass(void);

    amVK_RenderPassMK2(amVK_SurfaceMK2 *S, amVK_DeviceMK2 *D = nullptr, uint8_t attachments_n = 0, uint8_t subpasses_n = 0);
    ~amVK_RenderPassMK2() {}

    protected:
    amVK_RenderPassMK2           (const amVK_RenderPassMK2&) = delete;  // Brendan's Solution
    amVK_RenderPassMK2& operator=(const amVK_RenderPassMK2&) = delete;  // Brendan's Solution





    public:
    VkSampleCountFlagBits          samples = VK_SAMPLE_COUNT_1_BIT;                 /** [MSAA MultiSample] */

                           // depth_format = VK_FORMAT_D32_SFLOAT;
        bool              color_attachment = true;
        bool              depth_attachment = false;
        bool                single_subpass = true;


    /** [INIT] Called in Create_RenderPass, if not called already by user */
    void konfigurieren(void) {calc_n_alloc(); set_attachments(); set_attachment_refs(); set_subpasses();}





    private:
    bool is_malloced = false;
    void calc_n_alloc(void);
    /**
     * filters   final_image_format   &   final_image_colorSpace
     *    IMPL: demoSurfaceExt.filter_SurfaceFormat()
     */
    void set_attachments(void);
    void set_attachment_refs(void);
    void set_subpasses(void);


    public:
    /** 
     * 0xFF = Error code, 
     * variable mainly used for \fn set_subpasses
     * variable         set in  \fn set_attachment_refs
     */
    uint8_t depth_index = 0xFF;
    uint8_t color_index = 0;
    amVK_Array<VkAttachmentDescription>  m_attachment_descs = {};   /** Modifiable before calling \fn konfigurieren() */
    amVK_Array<VkAttachmentReference>     m_attachment_refs = {};   /** e.g. m_attachment_descs.n = 2; */
    amVK_Array<VkSubpassDescription>            m_subpasses = {};





    public:
    void add_attachmentGEN2(VkFormat image_format, VkImageLayout finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 
                                                   VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, 
                                                 VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE) {
        if (m_attachment_descs.is_filled()) {
            amVK_LOG_EX("you are trynna push back more elements than you asked for....");
            return;
        }
        amVK_ARRAY_PUSH_BACK(m_attachment_descs) = {0,
          image_format, samples,
          loadOp, storeOp,

          /** [.stencilLoadOp, .stencilStoreOp]  -  This is only used for depth_attachment, \see \fn set_attachments() for DepthStencil */ 
          VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,

          VK_IMAGE_LAYOUT_UNDEFINED,
          finalLayout
        };
    }
};
