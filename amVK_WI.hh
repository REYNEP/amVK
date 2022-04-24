#pragma once

#include "amVK_INK.hh"
#include "amVK_Device.hh"
#include "amVK_RenderPass.hh"


/**
 * ════════════════════════════════════════════════════════════════════
 *                          - amVK_Surface -
 * ═══════════════════════════ HIGHLIGHTS ═════════════════════════════
 * \brief
 *    This is one of the Ties between amVK_RenderPass & amVK_WI    (RenderPass n' Swapchain)
 *    The other one is ImageAttachments + their Format & Colorspace
 * 
 *    also \fn amVK_WI_MK2::create_Swapchain or \fn fallback_the_info needs surface information....
 * 
 * \history
 *   calling get_SurfaceFormats 2nd time smtimes gave me NULL values.... (nvidia, win10) [which I didn't see happen after october 2021]
 *      main reason we had to create this struct to store the DATA
 *      [.... 🤔 or maybe calling with SAME uint32_t pointer the 2nd time is what causes the problems....] (but this Didn't seem to be the Case after October too....)
 * 
 * if present_qFam is not called.... but surface is used to create VkSwapchainKHR.... then VUID-VkSwapchainCreateInfoKHR-surface-01270 is raised
 */
class amVK_SurfaceMK2 {
 public:
  VkSurfaceKHR S;
  VkPhysicalDevice PD;

 public:
  amVK_SurfaceMK2(VkSurfaceKHR S,  amVK_DeviceMK2  *D, bool uKnowWhatURDoing = false);
  amVK_SurfaceMK2(VkSurfaceKHR S, VkPhysicalDevice PD) : S(S), PD(PD) {}
  ~amVK_SurfaceMK2 () {}
  /** TODO: delete[] surface_formats.data; delete[] present_modes.data; */

 protected:
  amVK_SurfaceMK2           (const amVK_SurfaceMK2&) = delete;  // Brendan's Solution
  amVK_SurfaceMK2& operator=(const amVK_SurfaceMK2&) = delete;  // Brendan's Solution


 public:
  VkPhysicalDevice select_DisplayDevice(void);              /** based on   present_qFam(),     chooses bestOne      */
  uint32_t       present_qFam(bool re = true);              /** based on   vkGetPhysicalDeviceSurfaceSupportKHR()   */
  uint32_t       present_qFamily = UINT32_T_NULL;
  inline bool is_present_sup(void) {
    return this->present_qFam(false) == UINT32_T_NULL ? false : true;
  }


  void get_SurfaceFormats(bool print = false);              /**    called in     filter_SurfaceFormat()             */
  void get_PresentModes(void);                              /**    called in     amVK_WI_MK2::fallback_the_info     */
  amVK_Array<VkSurfaceFormatKHR> surface_formats = {};
  amVK_Array<VkPresentModeKHR>     present_modes = {};
  VkSurfaceCapabilitiesKHR          surface_caps = {};      /** connected to    amVK_WI_MK2::the_info   stuffs....  */

  /** \todo different update module, on resize only.... */
  VkSurfaceCapabilitiesKHR     *get_SurfaceCaps(void) {
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PD, S, &surface_caps);
    return &surface_caps;
  }


 public:
  VkFormat         final_imageFormat     = amVK_SWAPCHAIN_IMAGE_FORMAT;           /**      [a.k.a VkSurfaceFormatKHR]       \see amVK_Types.hh     */
  VkColorSpaceKHR  final_imageColorSpace = amVK_SWAPCHAIN_IMAGE_COLORSPACE;       /**   You Can Modify before   amVK_WI_MK2::_default_the_info()   */
  bool _filtered = false;

  /** fallbacks on B8G8R8A8/R8G8B8A8_SRGB... \todo improve.... maybe return a val;ue too */
  void filter_SurfaceFormat(void);
};












/**
 *  ╻ ╻   ╻┏┳┓┏━╸   ╺┳┓┏━┓╺┳╸┏━┓   ┏┳┓╻┏ ┏━┓
 *  ╺╋╸   ┃┃┃┃┃╺┓    ┃┃┣━┫ ┃ ┣━┫   ┃┃┃┣┻┓┏━┛
 *  ╹ ╹   ╹╹ ╹┗━┛╺━╸╺┻┛╹ ╹ ╹ ╹ ╹╺━╸╹ ╹╹ ╹┗━╸
 * _alloc -ated   in   amVK_WI_MK2::post_create_swapchain()
 *    _free -ed   in   amVK_WI_MK2::destroy()
 * 
 * \note not used outside amVK_WI_MK2.
 */
class IMG_DATA_MK2 {
 public:
  VkFramebuffer   *framebufs = nullptr;
  VkClearValue  *clearValues = nullptr;                     /**  attach_n  */
  bool imageless_framebuffer = false;                       /**     cz all devices might not have VK_KHR_IMAGELESS_FRAMEBUFFER support    */

  bool swap_imgs_are_color_attach = true;
  uint8_t swap_attach_index = 0;                            /**  swapchain creates default attachments, will be used for colorAttachment  */
  uint8_t color_index = 0;                                  /**     can be dif. from the above one.... TODO: _amVK_RP->color_index....    */

  uint8_t framebuf_n = 0;                                   /**  a.k.a:  the_info.minImageCount                    \see Default_the_info  */
  uint8_t   attach_n = 0;                                   /**  [per: framebuf]     _amVK_RP->attachment_descs.n  \see Default_the_info  */



  /** 
   * could've had an array of ImageMK2.... 
   * but 'attachments' are sorted in a cool way.... 
   * so that it can be passed to Framebuffer.pAttachments directly....
   *    still, these are created using ImageMK2.... [destroyed too]
   */
  VkImageView   *attachments = {};                          /** VkImageView[n][attach_n]   includes views created from        swap_imgs    */
  VkImage            *images = nullptr;                     /**     VkImage[attach_n][n]   includes vkGetSwapchainImagesKHR  [swap_imgs]   */
  VkDeviceMemory        *mem = nullptr;                     /**            [attach_n][n]   Doesn't include swapchain ones....              */
                                                            /**                                  n = framebuf_n                            */

  inline VkImage        *i_ptr_img(uint8_t framebuf_i, uint8_t attach_i) {return (     images + (attach_i * framebuf_n) + framebuf_i);}
  inline VkDeviceMemory *i_ptr_mem(uint8_t framebuf_i, uint8_t attach_i) {return (        mem + (attach_i * framebuf_n) + framebuf_i);}
  inline VkImageView *i_ptr_attach(uint8_t framebuf_i, uint8_t attach_i) {return (attachments + (framebuf_i * attach_n) + attach_i);}
  //Use macros below this struct, if you wanna access vars above

  inline bool check_index(        uint8_t framebuf_i, uint8_t attach_i) {
    bool ok = true;
    if (framebuf_i > framebuf_n) {amVK_LOG_EX("Requested: " << framebuf_i << "th framebuffer from " << this->framebuf_n                       << " [return nullptr]"); ok = false;}
    if (  attach_i > attach_n)   {amVK_LOG_EX("Requested: " << attach_i   << "th   atachment from " << this->attach_n << " per framebuffer. " << " [return nullptr]"); ok = false;}
    return ok;
  }

  

  bool alloc_called = false;
  bool i_alloc(void);
  bool i_free(void) {
    if (alloc_called) {free(attachments); alloc_called = false; return true;} 
    else {amVK_LOG_EX("alloc_called == false"); return false;}
  }
};


/** USE: VkImageView *framebuffer_attachments = PTR_ATTACH(2, 0); */
#define PTR_FRAMEBUF_ATTACHMENTS(PTR_WI, framebuf_i)                  PTR_WI->_ptr_attach(framebuf_i, 0); \
    if (!PTR_WI->IMGs.       check_index(framebuf_i, 0))           { amVK_LOG_EX("PTR_FRAMEBUF_ATTACHMENTS called with out of bounds values, there will be vulkan errors"); }

#define PTR_IMG(PTR_WI,                  framebuf_i, attach_i)        PTR_WI->_ptr_img(   framebuf_i, attach_i); \
    if (!PTR_WI->IMGs.       check_index(framebuf_i, attach_i))    { amVK_LOG_EX("PTR_FRAMEBUF_ATTACHMENTS called with out of bounds values, there will be vulkan errors"); }

#define PTR_ATTACH(PTR_WI,               framebuf_i, attach_i)        PTR_WI->_ptr_attach(framebuf_i, attach_i); \
    if (!PTR_WI->IMGs.       check_index(framebuf_i, attach_i))    { LOG_EX("PTR_FRAMEBUF_ATTACHMENTS called with out of bounds values, there will be vulkan errors"); }















/**
 *              █████╗ ███╗   ███╗██╗   ██╗██╗  ██╗        ██╗    ██╗██╗
 *             ██╔══██╗████╗ ████║██║   ██║██║ ██╔╝        ██║    ██║██║
 *             ███████║██╔████╔██║██║   ██║█████╔╝         ██║ █╗ ██║██║
 *             ██╔══██║██║╚██╔╝██║╚██╗ ██╔╝██╔═██╗         ██║███╗██║██║
 *             ██║  ██║██║ ╚═╝ ██║ ╚████╔╝ ██║  ██╗███████╗╚███╔███╔╝██║
 *             ╚═╝  ╚═╝╚═╝     ╚═╝  ╚═══╝  ╚═╝  ╚═╝╚══════╝ ╚══╝╚══╝ ╚═╝
 * ═════════════════════════════════════════════════════════════════════════════════
 * ═════════════════════════════════════════════════════════════════════════════════
 * 
 *      ███████╗██╗    ██╗ █████╗ ██████╗  ██████╗██╗  ██╗ █████╗ ██╗███╗   ██╗
 *      ██╔════╝██║    ██║██╔══██╗██╔══██╗██╔════╝██║  ██║██╔══██╗██║████╗  ██║
 *      ███████╗██║ █╗ ██║███████║██████╔╝██║     ███████║███████║██║██╔██╗ ██║
 *      ╚════██║██║███╗██║██╔══██║██╔═══╝ ██║     ██╔══██║██╔══██║██║██║╚██╗██║
 *      ███████║╚███╔███╔╝██║  ██║██║     ╚██████╗██║  ██║██║  ██║██║██║ ╚████║
 *      ╚══════╝ ╚══╝╚══╝ ╚═╝  ╚═╝╚═╝      ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝
 * 
 * ═════════════════════════════════════════════════════════════════════════════════
 *                       - Window Integration/Interface [WI] -
 * ══════════════════════════════════ HIGH LIGHTS ══════════════════════════════════
 * WI, like... WSI: Window 'System' Integration 😉
 * 
 * \todo TODO: CHANGE _fallback_the_info()
 * 
 * \todo fix, how 'graphics_queue' is actually implemented in amVK is seems like vague
 * \todo Multi-Window single Present support?
 * \todo Imageless FrameBuffer
 * \todo let choose arrayLayers & midLevels & imageType * pnext 
 * 
 * \todo Different ImageFormat & ColorSpace Support other than sRGB
 * 
 * \todo VkSurface Rotations & flips
 * \todo ImageArrayLayers (stereoscope3D)
 * \todo Pre/Post-Mul alpha  .... (VkSurfaceCapabilitiesKHR)
 * \todo DeviceGroup   [VK_SWAPCHAIN_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT_KHR]
 * \todo ImageSharingMode ON   [graphicsQFamily != presentationQFamily  VK_SHARING_MODE_CONCURRENT]
 * 
 * \todo support for 'the_info' changes in reCreate
 * \todo explain/doc IMG, IMGview, SWAPCHAIN, FRAMEBUFFER & RenderPass
 * 
 * \todo what if someone suddenly turns off imageless_framebuffer or increases the_info.minImageCount
 * 
 * TODO: Test Buffering MULTI-THREADING.... 
 * TODO: TEST how muich memory consumption per IMAGE
 * 
 * 
 * 
 *  THE_INFO DOCS:
 *    * IMMEDIATE: Uncapped Frames [Less Input Lag], minImageCount should be  at least 2, theoritically     FALLBACK: FIFO
      *      https://discord.com/channels/427551838099996672/427833316536746005/861359982469447720
      * 
      * IMAGES:
      *    \todo: dont want automated images created by swapchain?  (🤔 is there any hidden optimization)
      * minImageCount: [n_IMGs]
      *      surfaceCaps.minImageCount
      *      if you wanna do VSync or Double/Triple buffering, you need to create multiple images
      *      https://discord.com/channels/427551838099996672/427803486269210627/885268738247647263
      * imageUsage:
      *      Other options are TRANSFER_SRC/_DST.... but those can be said 'FINAL FrameBuffer' too 
      *      cz, then, you can just copy and send it to DISPLAY with porting throo DX12 or whatever.... 
      * imageFormat & imageColorSpace:
      *            configurable in amVK_RenderPassMK2....  You really shouldn't Change these two here for this the_info
      *            swapchain creates some images by default.... and to do so, it needs to know about the format & colorSpace.... (since we aren't passing the RenderPass itself)
      * 
      *            RENDERPASS attachment description [amVK_RenderPass::final_image_format/colorspace] has to match these TOO!
      *            \todo technically it doesn't need to match.... Renderpass has got nothing to do with imageFormats.... maybe not have those 2 variables there?
 * 
 * 
 * 
 * \note we dont Handle VkViewport in amVK.... cz stuffs like this should not be handled IMPLICTLY by amVK.... + amVK Uses DynamicViewport by default, [faster]
 * 
 * amVK_RP is used for AttachmentCreation & swapchainCI.imageFormat/ColorSpace, 
 * cz RenderPass colorAttachment.imageFormat has to match this TOO!
 */
class amVK_WI_MK2 {
 public:
  const char           *m_windowName;
  amVK_DeviceMK2             *amVK_D;
  amVK_SurfaceMK2            *amVK_S;
  amVK_RenderPassMK2         *amVK_RP;

 public:
  /** calls \fn Default_the_info(); */
  amVK_WI_MK2(const char *window, amVK_SurfaceMK2 *S, amVK_RenderPassMK2 *RP, amVK_DeviceMK2 *D = nullptr);
  ~amVK_WI_MK2() {}
  
  void Destroy_Swapchain(void);                           /**           \return false, if  !swapchain             */
  bool CleanUp_Swapchain(void);                           /**           doesn't do vkDestroySwapchain             */

 protected:
  amVK_WI_MK2           (const amVK_WI_MK2&) = delete;    /**                Brendan's Solution                   */
  amVK_WI_MK2& operator=(const amVK_WI_MK2&) = delete;    /**                Brendan's Solution                   */




 public:
  VkSwapchainCreateInfoKHR the_info = {};                 /** modify before _fallback_the_info / create_Swapchain */
  bool konfigurieren(void) {_fallback_the_info();}        /**   [internally    _fallback_the_info    is called]   */

  void _fallback_the_info(void);                          /** not evrythng in _default_the_info is sup. everywhere*/
  void _default_the_info() {                              /**            called in     CONSTRUCTOR()              */
    the_info.presentMode            = VK_PRESENT_MODE_IMMEDIATE_KHR;
    the_info.oldSwapchain           = nullptr;

    the_info.surface                = amVK_S->S;
    the_info.minImageCount          = 2;
    the_info.imageUsage             = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    the_info.imageFormat            = amVK_S->final_imageFormat;
    the_info.imageColorSpace        = amVK_S->final_imageColorSpace;
    the_info.imageExtent            = {};

    // Generic Options
    the_info.imageArrayLayers       = 1;

    the_info.sType                  = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    the_info.pNext                  = nullptr;
    the_info.flags                  = 0;

    the_info.imageSharingMode       = VK_SHARING_MODE_EXCLUSIVE;
    the_info.queueFamilyIndexCount  = 0;
    the_info.pQueueFamilyIndices    = nullptr;

    the_info.preTransform           = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    the_info.compositeAlpha         = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    the_info.clipped                = VK_FALSE;
  }






 public:
  VkSwapchainKHR          swapchain = nullptr;            /**            plz, dont modify this 😶          */
  VkExtent2D                 extent = {};                 /**            updated as you reCreate           */
  IMG_DATA_MK2                 IMGs = {};                 /** untouched before \fn post_create_swapchain() */
  uint32_t                   n_IMGs = 0;                  /** the_info.minImageCount += amVK_S->surface_caps.minImageCount  [create_Swapchain] */

  /**
   * \also [Recreate Swapchain]
   * \param check_the_info: calls fallback_the_info()    [if (this->swapchain || check_the_info)]
   */
  VkSwapchainKHR   create_Swapchain(bool check_the_info = true);

 private:
  /** 
   * IMGs._alloc() 
   * gets swapchain Images [vkGetSwapchainImagesKHR]
   * creates ImagesViews from those
   * \note vkCreateSwapchain() creates images implicitly
   */
  void post_create_swapchain(void);

 public:
  void create_Attachments(void);
  void create_Framebuffers(void);
  /** 
   * CALLED IN: \fn post_create_swapchain();
   * \todo support for more than 2 Attachments.... 
   * \todo ImageLess Framebuffer Support
   */
  void set_RenderPassClearVals(void);



 public:
  uint32_t nExt_img = 0;    /**     index into IMGs.images    */
  VkPresentInfoKHR     present_info = {
    VK_STRUCTURE_TYPE_PRESENT_INFO_KHR, nullptr,
    1, nullptr,
    1, &swapchain,
    &nExt_img,              /** 1    img per swapchain passed */
    nullptr                 /** 1 result per swapchain passed */
  };
  VkRenderPassBeginInfo rpInfo = {
    VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO, nullptr,
    amVK_RP->RP, nullptr,   /**         [.framebuffer]        */
    {}, 0, nullptr          /**   [.renderArea], clearValues  */
  };

  /**
   * \param to_signal: after acquiring is done, ig
   * \param timeout: def: 0.1sec
   * \return nExt_img
   */
  uint32_t AcquireNextImage(VkSemaphore to_signal, uint64_t timeout = 100000000);
  /** 
   * \param idk: I Dont Know much about this.... 
   */
  void Begin_RenderPass(VkCommandBuffer cmdBuf, VkSubpassContents idk = VK_SUBPASS_CONTENTS_INLINE);
  /** 
   * in VK, acquired images is locked till its 'Presented' 
   */
  bool Present(VkSemaphore to_wait);
};