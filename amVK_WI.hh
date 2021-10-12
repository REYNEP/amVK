#ifndef amVK_WI_H
#define amVK_WI_H
#include "vulkan/vulkan.h"
#include "amVK_Device.hh"
#include "amVK_RenderPass.hh"
// #include "amVK_Common.hh" [in .cpp]


/** 
 * \note
 * [You dont need to Need it rn if you are really new to VULKAN and didn't come here to make a COMPOSITOR or color sensitive stuffs
 * just Collapse this DOC and the STRUCT....]
 * 
 * \page
 * This time the doc is OK: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkColorSpaceKHR.html
 * & https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#data-format
 * But again for smone who doesn't know shit about COLORSPACE already, its a SHIT
 * 
 * In older versions of this extension VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT was misnamed VK_COLOR_SPACE_DCI_P3_LINEAR_EXT.
 * 
 * VK_EXT_swapchain_colorspace - '_EXT'    
 * VK_AMD_display_native_hdr   - '_AMD'
 * 
 * I think, 
 * LINEAR EOTF can be different for colorspace with different WhitePoint / ColorGamut
 *    But still that would be a LINEAR SLOPE....
 * 
 * The first one is VK_COLOR_SPACE_SRGB_NONLINEAR_KHR  -  amVK_SRGB
 * If a GPU has Presentation Support, it will have Support for this TOO   - by me
 * 
 * in the VKSpecs page, DCI-P3 is marked as Name	
 *    Red Primary {1.000, 0.000},	Green Primary	{0.000, 1.000},  Blue Primary {0.000, 0.000}, White-point {0.3333, 0.3333}
 *
 * 
 * 
 * 
 * Now if you see the wiki, thats not DCI-P3 Primaries.... so whats happening here??? 
 *  The fact is all colorspaces are encoded in RGB.... while DCI-P3 is encoded in XYZ values....
 *    what that means is, in RGB encoded spaces, RGB(0, 0, 1.0f) is gonna be like    at {0.150, 0.060} in XYZ....
 *      so that {0.150, 0.060} is like a MAX blue it can be.  [in XYZ]
 *    just like that, RGB encoded ones simply have a BOUND, binded to them, which we see as COLOR-PRIMARIES....
 *      but for encoding we would have to write {0, 0, 1.0}.... right?
 *      if it was in XYZ, we would instead have to write {0.150, 0.060}, 
 * 
 *    but hmmm... 🤔, what are those values relative to.... those {0.150, 0.060}???
 *      yeees, right, those values, {0.150, 0.060} are relative to that gamut-graphs that you have seen... that CIE XYZ ColorSpace, 
 *        and the entire colors in that Graph fits in 🛆{(0,0), (1,0), (0,1)}
 *        so, simply, XYZ encoded values are BOUND by or rathermore Relative to that Graph-Coordinates.... 😅
 * 
 *  Here the main fact is WHAT's RELATIVE to WHAT?
 *    and all RGB ColorSpaces are relative to a smaller XYZ-triangle, & is encoded relative to that smaller Triangle inside that Graph
 *    but not DCI-P3, as its encoded in XYZ, its encoded values are relative to the GRAPH-Coordinates....
 *    Now that is why VulkanSpecs indicated by those Primary-Values....
 * 
 * 
 * 
 * 
 * RESC: to get you started
 * start here: https://www.youtube.com/watch?v=m9AT7H4GGrA & https://www.youtube.com/watch?v=4nYGJI0r2-0
 *    Check Sobotka's Files out for FILMIC Blender.... the OCIO ones.... OCIO is cool, it was developed by Sony Pictures ImageWorks,
 *    the First Blender Filmic: https://github.com/sobotka/filmic-blender
 *    This summerizes, sm difference stuffs: http://www.wesleyknapp.com/blog/hdr
 * 
 * Search for how the Camera does it, a bit about its history. get familiar with EOTF, OETF [Optical-Electronic Transfer Function]..... For better Dynamic range after Encoding at lower bits, the idea of OETF was created
 *      Its like, Our eyes are SENSITIVE to Darker Shades.... our eyes can tell slightest difference in color, but for SunLit Bright ares in Midday, its bit less precise.... the more the light, the less our eyes can differentiate
 *      . now to give more BITS to the darker low Luminosity ares, OETF is used, 
 * 
 * Then check these ones out:
 *    https://hg2dc.com/      [The hitchhiker's guide to digital colour]
 *    https://chrisbrejon.com/articles/ocio-display-transforms-and-misconceptions/  [OCIO, Display Transforms, Misconceptions - Chris Brejon]
 *    https://developer.nvidia.com/sites/default/files/akamai/gameworks/hdr/UHDColorForGames.pdf [ got from: https://stackoverflow.com/q/49082820]
 * 
 *    and if you would like to give sm JUICE to your HEAD: https://www.khronos.org/registry/DataFormat/specs/1.3/dataformat.1.3.pdf   --  Start at Chapter 12 or 5.8
 *    KarenTheDorf provided this: Vulkan Discord: https://discord.com/channels/427551838099996672/427951526967902218/896415963950485524
 * 
 * if you are really Interested in this Topic, ColorSpace.... 
 *    [Not everyone has to be, like you can just go ahead and tell the vk To use what the DRIVER Supports.... 
 *      or just USE amVK, let amVK dooo all the stuffs under the HOOD.... \todo SOON UI Integrations....]
 *    you have to co-operate with Other ColorScientists.... 🙂😉
 *    
 * But If you are confused about anything, hit me up.... or if you really wonder about SOBOTKA at this point, let me know, [I shouldn't be telling you to knock him, but you actually prolly can]
 */
typedef enum VkColorSpaceKHR_amVKPort {
  amVK_sRGB = 0,    // no EOFT??? [My guess is, a SRGB EOFT.... cz i dont think the vkSpecs talks about a SOFTWARE LAYER of EOTF functions, it references to the DISPLAY's intended EOTF, i think]
                    // But VK_FORMAT_B8G8R8A8_SRGB   kinda feels like....   was made for no SRGB EOTF

  amVK_displayP3 = VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT,        //sRGB-like EOTF applied (should be) to this before present [like amVK_EXTENDED_SRGB]
  amVK_displayP3_LINEAR = VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT,    //   linear EOTF

  amVK_bt709 =        VK_COLOR_SPACE_BT709_NONLINEAR_EXT,          //SMPTE 170M EOTF
  amVK_bt709_LINEAR = VK_COLOR_SPACE_BT709_LINEAR_EXT,             //    linear EOTF

  // COOLEST Ones.... [Hope that ACES stuffs comes out in future too!]
  amVK_DCI_P3 = VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT,    //DCI-P3 EOTF. Note that values in such an image are interpreted as XYZ encoded color data by the presentation engine.
                                                        // CZ This gamut it pretty DIfferent.... Check it out 😉 [Gamut is also in the DOC Spec page]
  
  // HDR ONES
  amVK_bt2020_LINEAR = VK_COLOR_SPACE_BT2020_LINEAR_EXT,// linear EOTF.
  amVK_DOLBYVISION_DV = VK_COLOR_SPACE_DOLBYVISION_EXT, // [Dolby Vision (BT2020) ] SMPTE ST2084 EOTF.
  amVK_HDR10_ST2084 = VK_COLOR_SPACE_HDR10_ST2084_EXT,  // [ HDR10 (BT2020) ] SMPTE ST2084 Perceptual Quantizer (PQ) EOTF.
  amVK_HDR10_HLG = VK_COLOR_SPACE_HDR10_HLG_EXT,        // [ HDR10 (BT2020) ] Hybrid Log Gamma (HLG) EOTF.

  // SHIT-Theory ONES
  amVK_scRGB = VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT,     //  sRGB EOTF  [Modified for ranged beyond 0-1, see the DataFormat specs 1.3]
  amVK_scRGB_LINEAR = VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT, //Linear EOTF

  amVK_adobeRGB = VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT,               //    LINEAR EOTF
  amVK_adobeRGB_LINEAR = VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT,           // Gamma 2.2 EOTF.


  // COOL!
  amVK_COLOR_SPACE_PASS_THROUGH = VK_COLOR_SPACE_PASS_THROUGH_EXT,
  amVK_DISPLAY_NATIVE_AMD = VK_COLOR_SPACE_DISPLAY_NATIVE_AMD  // display’s native color space. This matches the color space expectations of AMD’s FreeSync2 standard, for displays supporting it. 
                                                                // Wow This COOL, coming from AMD
} amVkColorSpaceKHR;

/**
 * VkFormats that are for the DISPLAY.... or the so called PRESENTATION ENGINE
 * For now these are the knows Ones I found from Nvidia or AMD surfaceCapabilities
 * http://www.vulkan.gpuinfo.org/listsurfaceformats.php
 * 
 * \note we removed SNORM.... cz there will always be UNORM, and SNORM is just.... -_-   APPLE Doesn't report it back....
 * and i was gonna create another amVK version of this, but why complicate things that doesn't need to be
 * 
 * https://stackoverflow.com/a/59630187
 */
char *VkFormat_Display_2_String(VkFormat F);


/** Extent and ImageCount is handled in \fn createSwapchain, so we use this cut version to catch your eyes on these settings and save sm bytes too */
typedef struct amVK_SurfaceCaps_GEN2 { 
  uint32_t                         maxImageArrayLayers;
  VkSurfaceTransformFlagsKHR       supportedTransforms;
  VkSurfaceTransformFlagBitsKHR    currentTransform;
  VkCompositeAlphaFlagsKHR         supportedCompositeAlpha;
  VkImageUsageFlags                supportedUsageFlags;
} amVK_SurfaceCaps;

/** Stores extended data related to VkSurface.... cz RenderPass needed a demo surface */
struct amVK_SurfaceMK2 {
  VkSurfaceKHR _S;
  VkPhysicalDevice _PD;

  amVK_Array<VkSurfaceFormatKHR> surface_formats = {};
  amVK_Array<VkPresentModeKHR>     present_modes = {};
  /** \todo test other vkEnum / vkGet funcs too, if they act like these ones */

  amVK_SurfaceMK2(VkSurfaceKHR S, VkPhysicalDevice PD) : _S(S), _PD(PD) {}
  ~amVK_SurfaceMK2 () {delete[] surface_formats.data; delete[] present_modes.data;}

  /** \brief imageUsageFlags, maxImageArrayLayers [stereoscope3D Stuff], Transform & CompositeAlpha */
  amVK_SurfaceCaps get_SurfaceCapabilities_II(void);

  void get_PresentModes(void);
  /** calling get_SurfaceFormats 2nd time gives NULL values.... (nvidia, win10)   main reason we had to create this struct to store the DATA */
  void get_SurfaceFormats(void);
};

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
 * \fn create_framebufs     [MK2 now]
 * \var the_info, [MODS:] you should set values to it as you wish, but do it once.... before the first creation, 
 *                only .imageExtent and .oldSwapchain needs to be updated [in \fn createSwapchain] for reCreate
 * 
 * \note For now ColorAttachment is default - images created by swapchain is used for that
 * \todo explain/doc IMG, IMGview, SWAPCHAIN, FRAMEBUFFER & RenderPass
 * TODO: Test Buffering MULTI-THREADING....   + TODO: TEST how muich memory consumption per IMAGE
 * 
 * IMMEDIATE https://discord.com/channels/427551838099996672/427833316536746005/861359982469447720
 * IMAGE-COUNT How many to be SAFE: https://discord.com/channels/427551838099996672/427803486269210627/885268738247647263
 */
class amVK_WI {
 public:
  const char           *_window;            /** Name your Window, DUH! */
  amVK_Device          *_amVK_D;
  amVK_RenderPassMK2  *_amVK_RP;            /** [IN] used for swapchainCI.imageFormat/ColorSpace, cz RenderPass colorAttachment.imageFormat has to match this TOO! */
  VkSurfaceKHR         _surface = nullptr;  /** [IN] */
  amVK_SurfaceMK2  *_surfaceExt = nullptr;  /** [IN] \todo intention, we should delete after first swapchain/RenderPass Creation */

  /** USED-IN: CONSTRUCTOR & \see amVK_Device::_present_qFam   */
  VkBool32 is_presenting_sup(void)      {         if (_amVK_D->_present_qFam(_surface)        == 0xFFFFFFFF) {return false;} else return true;}
  uint32_t present_qFam(VkSurfaceKHR S) {uint32_t x = _amVK_D->_present_qFam(_surface); if (x != 0xFFFFFFFF) {return     x;} else return 0xFFFFFFFF;}

  VkSwapchainKHR    _swapchain = nullptr;   /** [OUT], not meant to be modified by you */
  VkExtent2D           _extent = {};        /** [OUT], from \fn createSwapchain, updated as you reCreate */



  /**  ╻ ╻   ╺┳╸╻ ╻┏━╸   ╻┏┓╻┏━╸┏━┓
   *   ╺╋╸    ┃ ┣━┫┣╸    ┃┃┗┫┣╸ ┃ ┃
   *   ╹ ╹    ╹ ╹ ╹┗━╸╺━╸╹╹ ╹╹  ┗━┛          **/
  VkSwapchainCreateInfoKHR the_info = {};   /** Not many vars needs to be modified for a reCreate */
  /**
   * stuffs that I'ld tell ppl to use....
   * presentMode IMMEDIATE is checked if supported, if not, FIFO used
   * imageCount is currently incremented by   surfaceCapabilities.minImageCount  cz of   https://discord.com/channels/427551838099996672/427803486269210627/885268738247647263 
   * imageFormat & imageColorSpace  \see the comment-block before get**SurfaceFormats in \fn createSwapchain()
   */
  void the_info_defaults() {
    the_info.presentMode    = VK_PRESENT_MODE_IMMEDIATE_KHR;    /** Uncapped Frames [Less Input Lag],  yet minImageCount should be  at least 2, theoritically
                                                                    FALLBACK: FIFO */
    the_info.minImageCount  = 2; /** + surfaceCaps.minImageCount    this is where you do buffering, e.g. Triple Buffering or VSync  \todo move & ext.docs on these 2
                                       [added anyway]               uhh, number of FINAL 'whatever you send to the Display' ???? \see nExt option, that's \related !
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


  /**
   * \brief vkGetSwapchainImagesKHR()      
   *    also acts as reCreateSwapchain [.oldSwapchain used if exists]
   * 
   * \todo Different ImageFormat & ColorSpace Support other than sRGB
   * \todo More options support for generic_mods as it covers up DeviceGroup, SharingMode, Stereoscopic3D options
   */
  VkSwapchainKHR   createSwapchain(bool call_the_info_defaults = false);
  void swapchain_CI_generic_mods(void);



  /**
   *   ╻ ╻   ┏━╸┏━┓┏┓╻┏━┓╺┳╸┏━┓╻ ╻┏━╸╺┳╸┏━┓┏━┓
   *   ╺╋╸   ┃  ┃ ┃┃┗┫┗━┓ ┃ ┣┳┛┃ ┃┃   ┃ ┃ ┃┣┳┛
   *   ╹ ╹   ┗━╸┗━┛╹ ╹┗━┛ ╹ ╹┗╸┗━┛┗━╸ ╹ ┗━┛╹┗╸
   * sets parameter vars to member vars and checks if valid or not
   * \param window: whatever name you choose to give to the Window....
   * \param device: DUH!!!!
   * \param surface: For now, \see amGHOST_Window::vulkan_create_surface() or create your own impl of vkCreateXXXSurfaceKHR()
   */
  amVK_WI(const char *window, amVK_SurfaceMK2 *S, amVK_RenderPassMK2 *RP, amVK_Device *D = nullptr);
  ~amVK_WI() {}
};













/**
 *             ██╗███╗   ███╗ ██████╗         ██████╗  █████╗ ████████╗ █████╗         ███╗   ███╗██╗  ██╗██████╗ 
 *   ▄ ██╗▄    ██║████╗ ████║██╔════╝         ██╔══██╗██╔══██╗╚══██╔══╝██╔══██╗        ████╗ ████║██║ ██╔╝╚════██╗
 *    ████╗    ██║██╔████╔██║██║  ███╗        ██║  ██║███████║   ██║   ███████║        ██╔████╔██║█████╔╝  █████╔╝
 *   ▀╚██╔▀    ██║██║╚██╔╝██║██║   ██║        ██║  ██║██╔══██║   ██║   ██╔══██║        ██║╚██╔╝██║██╔═██╗ ██╔═══╝ 
 *     ╚═╝     ██║██║ ╚═╝ ██║╚██████╔╝███████╗██████╔╝██║  ██║   ██║   ██║  ██║███████╗██║ ╚═╝ ██║██║  ██╗███████╗
 *             ╚═╝╚═╝     ╚═╝ ╚═════╝ ╚══════╝╚═════╝ ╚═╝  ╚═╝   ╚═╝   ╚═╝  ╚═╝╚══════╝╚═╝     ╚═╝╚═╝  ╚═╝╚══════╝
 */
/** \see create_framebufs for how this is used */
typedef struct SwapchainData_GEN3 {
  VkFramebuffer   *framebufs = nullptr;
  bool imageless_framebuffer = false;     /** if true, 1 framebuf */
  VkImageView   *attachments = {};        /** VkImageView[attach_n][n]   includes the views created from swap_imgs */
  VkImage            *images = nullptr;   /**     VkImage[n][attach_n]   includes vkGetSwapchainImagesKHR   [this one's rows are above one's columns] */

  uint8_t n = 0;                          /** the_info.minImageCount \see the_info.defaults */
  uint8_t attach_n = 0;                   /** _amVK_RP->attachment_descs.n */


  bool swap_imgs_are_color = true;
  uint8_t color_index = 0;                /** _amVK_RP->color_index */
  uint8_t swap_imgs_index = 0;            /** Yes default is zero, like _amVK_RP->color_index, change if you not using these for colorAttachment */
  VkImage     *ptr_swap_imgs(void)        {return &images[swap_imgs_index * n];}
  VkImageView *ptr_swap_attach(uint8_t i) {return (attachments + swap_imgs_index + (i * attach_n));}
  

  bool alloc_called = false;
  bool alloc(void) {
    if (alloc_called) {
      LOG("amVK_WI.IMGs.alloc_called == true;  seems like its already ALLOCATED!!!!       [we not allocating] ");
      return false;
    } 
    if (!n || !attach_n) {
      LOG("IMGs.n == 0   or   IMGs.attach_n == 0      [Nothing to malloc for, set those two (& possibly other values too)]");
      return false;
    }

    uint32_t framebuf_size = (imageless_framebuffer) ? sizeof(VkImage) : sizeof(VkImage) * n;
    void *xd = calloc(sizeof(VkImageView), /* attachments + images */ (n * attach_n * 2) + framebuf_size);
    
    attachments = reinterpret_cast<  VkImageView *> (xd);
    images =      reinterpret_cast<      VkImage *> (attachments + (n * attach_n));
    framebufs =   reinterpret_cast<VkFramebuffer *> (     images + (n * attach_n));

    alloc_called = true;
  }
} IMG_DATA_MK2;



/**
 *             ███╗   ███╗██╗  ██╗██████╗ 
 *   ▄ ██╗▄    ████╗ ████║██║ ██╔╝╚════██╗
 *    ████╗    ██╔████╔██║█████╔╝  █████╔╝
 *   ▀╚██╔▀    ██║╚██╔╝██║██╔═██╗ ██╔═══╝ 
 *     ╚═╝     ██║ ╚═╝ ██║██║  ██╗███████╗
 *             ╚═╝     ╚═╝╚═╝  ╚═╝╚══════╝
 * 
 * TODO: imageless_framebuffer support
 * MODS: IMGs.attachments, IMGs.framebufs
 */
class amVK_WI_MK2 : public amVK_WI {
 public:
  IMG_DATA_MK2              IMGs = {};        /** sm ppl would like to call this 'attachments', but duh! */
  amVK_WI_MK2(const char *window, amVK_SurfaceMK2 *S, amVK_RenderPassMK2 *RP, amVK_Device *D = nullptr) : amVK_WI(window, S, RP, D) {}
  ~amVK_WI_MK2() {}

  /** 
   *   \│/  ╔═╗┬─┐┌─┐┌┬┐┌─┐╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐   ┬   ╔═╗┌┬┐┌┬┐┌─┐┌─┐┬ ┬┌┬┐┌─┐┌┐┌┌┬┐┌─┐      
   *   ─ ─  ╠╣ ├┬┘├─┤│││├┤ ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘  ┌┼─  ╠═╣ │  │ ├─┤│  ├─┤│││├┤ │││ │ └─┐      
   *   /│\  ╚  ┴└─┴ ┴┴ ┴└─┘╚═╝└─┘└  └  └─┘┴└─  └┘   ╩ ╩ ┴  ┴ ┴ ┴└─┘┴ ┴┴ ┴└─┘┘└┘ ┴ └─┘   
   * 
   * \brief  [out] IMGs.framebufs, will be Compat with renderPass   
   */
  void malloc_n_get_IMGs(void) {
    uint32_t swap_imgs_n = 0;
    vkGetSwapchainImagesKHR(_amVK_D->_D, _swapchain, &swap_imgs_n, nullptr);
    LOG("Swapchain Images N: " << swap_imgs_n);
    
    IMGs.n = (uint8_t) swap_imgs_n;
    IMGs.attach_n = _amVK_RP->attachment_descs.n;
    IMGs.color_index = _amVK_RP->color_index;
    IMGs.swap_imgs_index =  IMGs.color_index;   /** \todo swap_imgs could be used in other ways too */
    /** \todo    IMGs.imageless_framebuffer = false */
    IMGs.alloc();

    vkGetSwapchainImagesKHR(_amVK_D->_D, _swapchain, &swap_imgs_n, IMGs.ptr_swap_imgs());
  }

  /**
   * When it came out in DX11.2 https://blogs.windows.com/windowsexperience/2014/01/30/directx-11-2-tiled-resources-enables-optimized-pc-gaming-experiences/
   * The Called it Tiles Resources: https://channel9.msdn.com/Events/Build/2013/4-063
   * This page has Quite Good Links: https://en.wikipedia.org/wiki/Texture_filtering
   * Unless you are making a GAME-ENGINE, you wouldn't really need to care about this one
   * TODO: [IDK-WHEN] Maybe ask Nvidia Devs about How they did the Optimizations
   * 
   * Well, basically put together, an Image under the Hood doesn't have to laid out like UNCOMPRESSED PNG [linear-tiling]
   * There is something called OPTIMAL_TILING bcz of which image's pixels and rgb channels can be laid out differently     [is this Bcz of GPU Multi-Threading? IDK]
   * Also there is MultiLayer images (stereoScope3d, VR ig.) and we might want/need to render to specific LAYER
   * Also there is this Theory of TEXELS.... & mip Levels
   * Thats why imageView exists, describing how to READ/WRITE, how bits & bytes are distributed
   */
  void create_imageviews(void);


  void create_framebufs(void);
};

#endif //amVK_WI_H