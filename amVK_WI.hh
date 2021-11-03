#ifndef amVK_WI_H
#define amVK_WI_H

#include "vulkan/vulkan.h"


#include "amVK_IN.hh"
#include "amVK_Device.hh"
#include "amVK_RenderPass.hh"
//#include "amVK_ImgMemBuf.hh" [in .cpp]

/** so we use this cut version to catch your eyes on these settings and save sm bytes too */
typedef struct amVK_SurfaceCaps_GEN2 { 
  uint32_t                         maxImageArrayLayers;
  VkSurfaceTransformFlagsKHR       supportedTransforms;
  VkSurfaceTransformFlagBitsKHR    currentTransform;
  VkCompositeAlphaFlagsKHR         supportedCompositeAlpha;
  VkImageUsageFlags                supportedUsageFlags;
} amVK_SurfaceCaps;

/** 
 *             ███████╗██╗   ██╗██████╗ ███████╗ █████╗  ██████╗███████╗███╗   ███╗██╗  ██╗██████╗ 
 *   ▄ ██╗▄    ██╔════╝██║   ██║██╔══██╗██╔════╝██╔══██╗██╔════╝██╔════╝████╗ ████║██║ ██╔╝╚════██╗
 *    ████╗    ███████╗██║   ██║██████╔╝█████╗  ███████║██║     █████╗  ██╔████╔██║█████╔╝  █████╔╝
 *   ▀╚██╔▀    ╚════██║██║   ██║██╔══██╗██╔══╝  ██╔══██║██║     ██╔══╝  ██║╚██╔╝██║██╔═██╗ ██╔═══╝ 
 *     ╚═╝     ███████║╚██████╔╝██║  ██║██║     ██║  ██║╚██████╗███████╗██║ ╚═╝ ██║██║  ██╗███████╗
 *             ╚══════╝ ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝  ╚═╝ ╚═════╝╚══════╝╚═╝     ╚═╝╚═╝  ╚═╝╚══════╝
 * Stores extended data related to VkSurface.... cz RenderPass needed to filter present_modes, but amVK_WI requires a RenderPass. SO these couldn't be inside amVK_WI 
 * \brief
 * calling get_SurfaceFormats 2nd time gives NULL values.... (nvidia, win10)   main reason we had to create this struct to store the DATA
 *      [or maybe calling with same uint32_t pointer the 2nd time is what causes the problems....]
 * bcz the first call happens in amVK_RenderPassMK2::set_surfaceFormat
 *     2nd when create_Swapchain or fallback_the_info
 */
struct amVK_SurfaceMK2 {
  VkSurfaceKHR _S;
  VkPhysicalDevice _PD;

  /** 
   * \brief
   * cz we needed to do this in RenderPassMK2, fallbacks on B8G8R8A8/R8G8B8A8_SRGB 
   */
  VkSurfaceFormatKHR filter_SurfaceFormat(VkSurfaceFormatKHR what_to_filter);

  amVK_Array<VkSurfaceFormatKHR> surface_formats = {};
  amVK_Array<VkPresentModeKHR>     present_modes = {};

  amVK_SurfaceMK2(VkSurfaceKHR S, amVK_DeviceMK2 *D) : _S(S), _PD(D->_PD) {}
  amVK_SurfaceMK2(VkSurfaceKHR S, VkPhysicalDevice PD) : _S(S), _PD(PD) {}
  ~amVK_SurfaceMK2 () {delete[] surface_formats.data; delete[] present_modes.data;}


  uint32_t present_qFamily = 0xFFFFFFFF;  //UINT32_T_NULL, means like false, like not found yet
  bool found_present_qFamily = false;
  /**
   * Call for all surfaces created. otherwise    VUID-VkSwapchainCreateInfoKHR-surface-01270  will be hit
   */
  bool is_presenting_sup(void) {
    this->present_qFam();
    return found_present_qFamily;
  }
  /** 
   * \note saves in \var present_qFamily
   * \return presenting supportive qFamily index [of _PD] 
   */
  uint32_t present_qFam(void);

  /** 
   * \brief imageUsageFlags, maxImageArrayLayers [stereoscope3D Stuff], Transform & CompositeAlpha 
   * Doesn't have the same bug as the other get_ funcs below
   */
  amVK_SurfaceCaps get_SurfaceCapabilities_II(void);

  void get_PresentModes(void);
  /** calling get_SurfaceFormats 2nd time gives NULL values.... (nvidia, win10)   main reason we had to create this struct to store the DATA */
  void get_SurfaceFormats(bool print = false);
};



/**
 *  ╻ ╻   ╻┏┳┓┏━╸   ╺┳┓┏━┓╺┳╸┏━┓   ┏┳┓╻┏ ┏━┓
 *  ╺╋╸   ┃┃┃┃┃╺┓    ┃┃┣━┫ ┃ ┣━┫   ┃┃┃┣┻┓┏━┛
 *  ╹ ╹   ╹╹ ╹┗━┛╺━╸╺┻┛╹ ╹ ╹ ╹ ╹╺━╸╹ ╹╹ ╹┗━╸
 */
typedef struct SwapchainData_GEN3 {
  VkFramebuffer   *framebufs = nullptr;
  bool imageless_framebuffer = false;

  VkImageView   *attachments = {};        /** VkImageView[n][attach_n]   includes views created from        swap_imgs  */
  VkImage            *images = nullptr;   /**     VkImage[attach_n][n]   includes vkGetSwapchainImagesKHR  [swap_imgs] */
  inline VkImage     *_ptr_img(   uint8_t framebuf_i, uint8_t attach_i) {return (     images + (attach_i * framebuf_n) + framebuf_i);}
  inline VkImageView *_ptr_attach(uint8_t framebuf_i, uint8_t attach_i) {return (attachments + (framebuf_i * attach_n) + attach_i);}

  inline bool check_index(        uint8_t framebuf_i, uint8_t attach_i) {
    bool ok = true;
    if (framebuf_i > framebuf_n) {LOG_EX("Requested: " << framebuf_i << "th framebuffer from " << this->framebuf_n                       << " [return nullptr]"); ok = false;}
    if (  attach_i > attach_n)   {LOG_EX("Requested: " << attach_i   << "th   atachment from " << this->attach_n << " per framebuffer. " << " [return nullptr]"); ok = false;}
    return ok;
  }

  uint8_t framebuf_n = 0;    /** [also: img_n]       the_info.minImageCount        \see Default_the_info */
  uint8_t   attach_n = 0;    /** [per: framebuf]     _amVK_RP->attachment_descs.n  \see Default_the_info */


  bool swap_imgs_are_color_attach = true;
  uint8_t color_index = 0;                /** _amVK_RP->color_index */
  uint8_t swap_attach_index = 0;          /** default: will be used for colorAttachment */
  

  bool alloc_called = false;
  bool alloc(void); //malloc
  bool _free(void) {
    if (alloc_called) {free(attachments); alloc_called = false; return true;} 
    else {LOG_EX("alloc_called == false"); return false;}
  }
} IMG_DATA_MK2;

#define PTR_FRAMEBUF_ATTACHMENTS(PTR_IMG_DATA_MK2, framebuf_i)                  PTR_IMG_DATA_MK2->_ptr_attach(framebuf_i, 0); \
    if (!PTR_IMG_DATA_MK2->            check_index(framebuf_i, 0))           { LOG_EX("PTR_FRAMEBUF_ATTACHMENTS called with out of bounds values, there will be vulkan errors"); }

#define PTR_IMG(PTR_IMG_DATA_MK2,                  framebuf_i, attach_i)        PTR_IMG_DATA_MK2->_ptr_img(   framebuf_i, attach_i); \
    if (!PTR_IMG_DATA_MK2->            check_index(framebuf_i, attach_i))    { LOG_EX("PTR_FRAMEBUF_ATTACHMENTS called with out of bounds values, there will be vulkan errors"); }

#define PTR_ATTACH(PTR_IMG_DATA_MK2,               framebuf_i, attach_i)        PTR_IMG_DATA_MK2->_ptr_attach(framebuf_i, attach_i); \
    if (!PTR_IMG_DATA_MK2->            check_index(framebuf_i, attach_i))    { LOG_EX("PTR_FRAMEBUF_ATTACHMENTS called with out of bounds values, there will be vulkan errors"); }




















/**
 *        █████╗ ███╗   ███╗██╗   ██╗██╗  ██╗        ██╗    ██╗██╗
 *       ██╔══██╗████╗ ████║██║   ██║██║ ██╔╝        ██║    ██║██║
 *       ███████║██╔████╔██║██║   ██║█████╔╝         ██║ █╗ ██║██║
 *       ██╔══██║██║╚██╔╝██║╚██╗ ██╔╝██╔═██╗         ██║███╗██║██║
 *       ██║  ██║██║ ╚═╝ ██║ ╚████╔╝ ██║  ██╗███████╗╚███╔███╔╝██║
 *       ╚═╝  ╚═╝╚═╝     ╚═╝  ╚═══╝  ╚═╝  ╚═╝╚══════╝ ╚══╝╚══╝ ╚═╝
 * ═════════════════════════════════════════════════════════════════════
 *                 - Window Integration/Interface [WI] -
 * ═══════════════════════════ HIGH LIGHTS ═════════════════════════════
 * \brief
 * SwapChain, FIFO (V-Sync), Presentation, Frame-Uncap (IMMEDIATE), Tripple-Buffering [or Double Buffer], Presentation, Frame-Uncap (IMMEDIATE)
 * WI, like... WSI: Window 'System' Integration 😉 [smth we gotta have Per Window. So we name it Window Interface/Integration]
 * 
 * \fn create_swapchain     [Recreate Swapchain too] ['if _swapchain = nullptr' - considered swapchainFirstTime]
 * \fn create_Attachments_n_FrameBufs
 * \var the_info, [MODS:] you should set values to it as you wish, but do it once.... before the first creation, 
 *                only .imageExtent and .oldSwapchain needs to be updated [in \fn createSwapchain] for reCreate
 *                \todo support for changes in reCreate
 * 
 * \note For now ColorAttachment is default - images created by swapchain is used for that
 * \todo explain/doc IMG, IMGview, SWAPCHAIN, FRAMEBUFFER & RenderPass
 * \todo change LOG_EX to BackTrace
 * \todo turn of LOG messages after adding COMPLETE Modifiable way.... a.k.a MK3
 * TODO: Test Buffering MULTI-THREADING....   + TODO: TEST how muich memory consumption per IMAGE
 * 
 * IMMEDIATE https://discord.com/channels/427551838099996672/427833316536746005/861359982469447720
 * IMAGE-COUNT How many to be SAFE: https://discord.com/channels/427551838099996672/427803486269210627/885268738247647263
 */
class amVK_WI_MK2 {
 public:
  const char           *_window;            /** Name your Window, DUH! */
  amVK_DeviceMK2          *_amVK_D;
  amVK_RenderPassMK2  *_amVK_RP;            /** [IN] used for AttachmentCreation & swapchainCI.imageFormat/ColorSpace, cz RenderPass colorAttachment.imageFormat has to match this TOO! */
  VkSurfaceKHR         _surface = nullptr;  /** [IN] */
  amVK_SurfaceMK2      *_amVK_S = nullptr;  /** [IN] \todo intention, we should delete after first swapchain/RenderPass Creation */

  VkSwapchainKHR    _swapchain = nullptr;   /** [OUT], not meant to be modified by you */
  VkExtent2D           _extent = {};        /** [OUT], from \fn createSwapchain, updated as you reCreate */


  VkSwapchainCreateInfoKHR the_info = {};   /** [MOD-MAIN] Not many vars needs to be modified for a reCreate */
  /**
   * stuffs that I'ld tell ppl to use....
   * presentMode IMMEDIATE is checked if supported, if not, FIFO used
   * imageCount is currently incremented by   surfaceCapabilities.minImageCount  cz of   https://discord.com/channels/427551838099996672/427803486269210627/885268738247647263 
   * imageFormat & imageColorSpace  \see the comment-block before get**SurfaceFormats in \fn createSwapchain()
   * \note called in the CONSTRUCTOR
   */
  void Default_the_info() {
    the_info.presentMode    = VK_PRESENT_MODE_IMMEDIATE_KHR;    /** Uncapped Frames [Less Input Lag],  yet minImageCount should be  at least 2, theoritically
                                                                    FALLBACK: FIFO */
    the_info.minImageCount  = 2; /** + surfaceCaps.minImageCount    this is where you do buffering, e.g. Triple Buffering or VSync  \todo move & ext.docs on these 2
                                       [ \todo added anyway??]      uhh, number of FINAL 'whatever you send to the Display' ???? \see nExt option, that's \related !
                                       [ \see createSwapchain]      Again, framebuffer is just a Vulkan/GL/DX Object that has all the data related to it */

    the_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;  /** The images created by swapchain will be for the FINAL [Presentation] Framebuffer 
                                                                    Other options are TRANSFER_SRC/_DST.... but those can be said 'FINAL FrameBuffer' too 
                                                                    cz, then, you can just copy and send it to DISPLAY with porting throo DX12 or whatever.... 
                                                                        Why did I say 'FrameBuffer'? \see RenderPass colorAttachment.finalLayout haha 😉 */
    /** \todo VUID-VkSwapchainCreateInfoKHR-imageUsage-parameter */

    /** these two are configurable in amVK_RenderPassMK2....  You really shouldn't Change these two here for this the_info */
    the_info.imageFormat            = _amVK_RP->final_imageFormat;     /** Morelike, the GPU Wants to know what you wanna send to the DISPLAY */
    the_info.imageColorSpace        = _amVK_RP->final_imageColorSpace; /** RENDERPASS attachment description [amVK_RenderPass::final_image_format/colorspace] has to match these TOO! */

    the_info.surface                = _surface;
    if (the_info.sType == (VkStructureType)0) swapchain_CI_generic_mods();
  }
  /** not everything you see above is supported everywhere.... so we do smth like 'filtering' */
  void fallback_the_info(void);


  /**
   * \brief vkGetSwapchainImagesKHR()      
   *    also acts as reCreateSwapchain [.oldSwapchain used if exists]
   * 
   * \param check_the_info: calls fallback_the_info() if true. Enable 1 time after changing the_info
   * 
   * \note vkCreateSwapchain() creates images implicitly, this amVK Function gets those & creates ImageViews. [but thats in \fn post_create_swapchain]
   * 
   * \todo Different ImageFormat & ColorSpace Support other than sRGB
   * \todo More options support for generic_mods as it covers up DeviceGroup, SharingMode, Stereoscopic3D options
   */
  VkSwapchainKHR   create_Swapchain(bool check_the_info = false);
  void swapchain_CI_generic_mods(void);





  /**
   *   ╻ ╻   ┏━╸┏━┓┏┓╻┏━┓╺┳╸┏━┓╻ ╻┏━╸╺┳╸┏━┓┏━┓
   *   ╺╋╸   ┃  ┃ ┃┃┗┫┗━┓ ┃ ┣┳┛┃ ┃┃   ┃ ┃ ┃┣┳┛
   *   ╹ ╹   ┗━╸┗━┛╹ ╹┗━┛ ╹ ╹┗╸┗━┛┗━╸ ╹ ┗━┛╹┗╸
   * sets parameter vars to member vars and checks if valid or not
   * CALLS: \fn Default_the_info();
   * \param window: whatever name you choose to give to the Window....
   * \param device: DUH!!!!
   * \param surface: For now, \see amGHOST_Window::create_vulkan_surface() or create your own impl of vkCreateXXXSurfaceKHR()
   */
  amVK_WI_MK2(const char *window, amVK_SurfaceMK2 *S, amVK_RenderPassMK2 *RP, amVK_DeviceMK2 *D = nullptr);
  ~amVK_WI_MK2() {}
  bool destroy(void); /** If you want Docs, see inside amVK_WI.cpp, the impl.   \return false, if (!_swapchain) */





  /**
   *             ███╗   ███╗██╗  ██╗██████╗ 
   *   ▄ ██╗▄    ████╗ ████║██║ ██╔╝╚════██╗
   *    ████╗    ██╔████╔██║█████╔╝  █████╔╝
   *   ▀╚██╔▀    ██║╚██╔╝██║██╔═██╗ ██╔═══╝ 
   *     ╚═╝     ██║ ╚═╝ ██║██║  ██╗███████╗
   *             ╚═╝     ╚═╝╚═╝  ╚═╝╚══════╝
   * 
   * TODO: imageless_framebuffer support
   * \todo let choose arrayLayers & midLevels & imageType * pnext 
   * 
   * MODS: IMGs.attachments, IMGs.framebufs, IMGs.images
   */
  IMG_DATA_MK2              IMGs = {};        /** sm ppl would like to call this 'attachments', but duh! this struct has everything related to those in it */

  /** 
   *   \│/  ╔═╗┬─┐┌─┐┌┬┐┌─┐╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐   ┬   ╔═╗┌┬┐┌┬┐┌─┐┌─┐┬ ┬┌┬┐┌─┐┌┐┌┌┬┐┌─┐
   *   ─ ─  ╠╣ ├┬┘├─┤│││├┤ ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘  ┌┼─  ╠═╣ │  │ ├─┤│  ├─┤│││├┤ │││ │ └─┐
   *   /│\  ╚  ┴└─┴ ┴┴ ┴└─┘╚═╝└─┘└  └  └─┘┴└─  └┘   ╩ ╩ ┴  ┴ ┴ ┴└─┘┴ ┴┴ ┴└─┘┘└┘ ┴ └─┘
   */
 private:
  /** IMGs.alloc(), & gets swapchain Images, creates ImagesViews from those */
  void post_create_swapchain(void);

 public:
  /** 
   * \todo MERGE with IMAGEUTIL, the implementation 
   * \note CALLS: _set_RenderPassClearVals()
   */
  void create_Attachments(void);
  void create_Framebuffers(void);

  uint32_t nExt_img = 0;
  /**
   * \param to_signal: Signaled after acquiring is done, i guess
   * \param timeout, vkAcquireNextImageKHR takes a timeout in nanosec. def: 1sec
   * \return index of IMGs.images that is available, don't worry, you will only need to pass this index later on to other vulkan funcs
   * 
   * SETS: \var nExt_img
   * \see \fn Present.... Uses \var nExt_img. 
   * 
   * \note Acquired image can only be freed once you call vkQueuePresent()
   */
  uint32_t AcquireNextImage(VkSemaphore to_signal, uint64_t timeout = 1000000000) {
    VkResult res = vkAcquireNextImageKHR(_amVK_D->_D, _swapchain, timeout, to_signal, nullptr, &nExt_img);
    if (res != VK_SUCCESS) {
        LOG_DBG("Couldn't AcquireNextImageKHR....");
        LOG_EX("")
        return UINT32_T_NULL;
    }
    return nExt_img;
  }

  /**
   * \todo fix, how 'graphics_queue' is actually implemented in amVK is seems like vague
   * \todo Multi-Window single Present support?
   */
  bool Present(VkSemaphore to_wait) {
    VkResult res;
    VkPresentInfoKHR presentInfo = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR, nullptr, 
        1, &to_wait,
        1, &_swapchain,
        &nExt_img,   /** 1    img per swapchain passed */
        &res         /** 1 result per swapchain passed */
    };
    vkQueuePresentKHR(_amVK_D->get_graphics_queue(), &presentInfo);

    if (res != VK_SUCCESS) {
        LOG_DBG("vkQueuePresentKHR() failed.... Serious bug....");
        LOG_EX("");
        return false;
    }
    return true;
  }


  /** 
   *   \│/  ╦═╗┌─┐┌┐┌┌┬┐┌─┐┬─┐╔═╗┌─┐┌─┐┌─┐  ╔╗ ┌─┐┌─┐┬┌┐┌  ╦┌┐┌┌─┐┌─┐
   *   ─ ─  ╠╦╝├┤ │││ ││├┤ ├┬┘╠═╝├─┤└─┐└─┐  ╠╩╗├┤ │ ┬││││  ║│││├┤ │ │
   *   /│\  ╩╚═└─┘┘└┘─┴┘└─┘┴└─╩  ┴ ┴└─┘└─┘  ╚═╝└─┘└─┘┴┘└┘  ╩┘└┘└  └─┘
   */
  VkClearValue *clearValues;
  /** 
   * CALLED IN: \fn create_Attachments();
   * \todo support for more than 2 Attachments.... 
   * \todo ImageLess Framebuffer Support
   */
  void _set_RenderPassClearVals(void) {
    clearValues = new VkClearValue[_amVK_RP->attachment_descs.n]; 

    clearValues[_amVK_RP->color_index] = { 0.0f, 0.0f, 0.0f, 1.0f }; /** Color */

    if (_amVK_RP->attachment_descs.n >= 2) {
      clearValues[_amVK_RP->depth_index] = { 1.f }; /** Depth */
    }
    /** More Attachment Support soon */

    rpInfo.clearValueCount = _amVK_RP->attachment_descs.n;
    rpInfo.pClearValues = clearValues;
  }
  VkRenderPassBeginInfo rpInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO, nullptr,
    _amVK_RP->_RP, nullptr /** [.framebuffer] */
  };

  /**
   * \param idk: I Dont Know much about this....
   * \note CALL: AcquireNextImage() before this function.... or it wont work
   * Why is this not in amVK_RenderPass? because.... vkCmdBeginRenderPass doesn't exactly mean what it seems at first
   *    the BeginInfo here ties to this window's FrameBuffer....  and thats it, the reason
   *
   * vkCmdEndRenderPass just takes 1 param.... VkCommandBuffer, so we dont have a func for that
   */
  void Begin_RenderPass(VkCommandBuffer cmdBuf, VkSubpassContents idk = VK_SUBPASS_CONTENTS_INLINE) {
      rpInfo.framebuffer = IMGs.framebufs[nExt_img];
      rpInfo.renderArea = { /** its same data type as scissor*/
          {0, 0}, /** [.offset] (x, y) */
          _extent
      };
      
      vkCmdBeginRenderPass(cmdBuf, &rpInfo, idk);
  }
};


/** 
   ╻ ╻   ╻┏┳┓┏━╸   ╺┳┓┏━┓╺┳╸┏━┓   ┏━┓╻  ╻  ┏━┓┏━╸
   ╺╋╸   ┃┃┃┃┃╺┓    ┃┃┣━┫ ┃ ┣━┫   ┣━┫┃  ┃  ┃ ┃┃  
   ╹ ╹   ╹╹ ╹┗━┛╺━╸╺┻┛╹ ╹ ╹ ╹ ╹   ╹ ╹┗━╸┗━╸┗━┛┗━╸
 */
#ifdef amVK_WI_CPP
  bool IMG_DATA_MK2::alloc(void) {  //malloc
    if (alloc_called) {
      LOG("amVK_WI.IMGs.alloc_called == true;  seems like its already ALLOCATED!!!!       [we not allocating] ");
      return false;
    } 
    if (!framebuf_n || !attach_n) {
      LOG("IMGs.attach_i == 0   or   IMGs.attach_n == 0      [Nothing to malloc for, set those two (& possibly other values too)]");
      return false;
    }

    uint32_t framebuf_size = (imageless_framebuffer) ? sizeof(VkImage) : sizeof(VkImage) * framebuf_n;
    void *xd = calloc(sizeof(VkImageView), /* attachments + images */ (framebuf_n * attach_n * 2) + framebuf_size);
    
    attachments = reinterpret_cast<  VkImageView *> (xd);
    images =      reinterpret_cast<      VkImage *> (attachments + (framebuf_n * attach_n));
    framebufs =   reinterpret_cast<VkFramebuffer *> (     images + (framebuf_n * attach_n));

    alloc_called = true;
  }
#endif

#endif //amVK_WI_H