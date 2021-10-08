#ifndef amVK_WI_H
#define amVK_WI_H
#include "vulkan/vulkan.h"
#include "amVK_Device.hh"

#ifndef T_WINDOW_amVK
  #define T_WINDOW_amVK amGHOST_Window  //Use whatever Hell type you wish to use.... [That 1-thing that Distinguishes 1 Object from other amVK_WI objects]
#endif
class T_WINDOW_amVK;


typedef struct swapchain_Data_Gen2__ {
  VkFramebuffer  *framebufs = nullptr;
  VkRenderPass    renderpass = nullptr;
  VkImage        *list = nullptr;
  VkImageView    *views = nullptr;
  uint32_t        n = 0;
} IMG_DATA;

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
 * 
 * \fn create_swapchain
 * \fn create_framebuffers
 * 
 * \see T_WINDOW_amVK
 * \note WI, like... WSI: Window 'System' Integration ;)  [smth we gotta have Per Window. So we name it Window Interface/Integration]
 * \todo explain/doc IMG, IMGview, SWAPCHAIN, FRAMEBUFFER & RenderPass
 * \todo MAKE THEM KNOW ABOUT T_WINDOW_amVK amGHOST_Window above....
 * 
 * TODO: Test Buffering MULTI-THREADING....   + TODO: TEST how muich memory consumption per IMAGE
 * MODS: \fn swapchain_CI_mods()
 * 
 *  IMMEDIATE https://discord.com/channels/427551838099996672/427833316536746005/861359982469447720
 */
class amVK_WI {
 public:
  T_WINDOW_amVK *_window;
  amVK_Device *_amVK_D;
  
  VkExtent2D    _extent = {};
  VkSurfaceKHR _surface = nullptr;

  VkSwapchainKHR   _swapchain = nullptr;
  IMG_DATA _IMGs = {};

  /**
   * \param window: \see T_WINDOW_amVK
   * \param device: DUH!!!!
   * \param surface: For now, \see amGHOST_Window::vulkan_create_surface() or create your own impl of vkCreateXXXSurfaceKHR()
   */
  amVK_WI(T_WINDOW_amVK *window, amVK_Device *D, VkSurfaceKHR S);
  ~amVK_WI() {}

  /** \see CONSTRUCTOR & \see amVK_Device::_present_qFam   */
  VkBool32 is_presenting_sup(void)      {         if (_amVK_D->_present_qFam(_surface)        == 0xFFFFFFFF) {return false;} else return true;}
  uint32_t present_qFam(VkSurfaceKHR S) {uint32_t x = _amVK_D->_present_qFam(_surface); if (x != 0xFFFFFFFF) {return     x;} else return 0xFFFFFFFF;}

  /**
   * \brief vkGetSwapchainImagesKHR()   [.oldSwapchain used if exists]
   * \param CI: if not nullptr, will be passed to vkCreateSwapchainKHR()
   * \todo Different ImageFormat & ColorSpace Support other than sRGB
   */
  VkSwapchainKHR create_swapchain(VkSwapchainCreateInfoKHR *CI = nullptr, bool CI_generic_level0 = true);
  static void   swapchain_CI_mods(VkSwapchainCreateInfoKHR *CI,           bool CI_generic_level0);

  /** \brief Framebufs will be Compat with renderPass
   * [out] _IMGs.framebufs */
  VkFramebuffer *create_framebuffers(VkRenderPass renderPass, VkImageView depthAttachment = nullptr);

  
  /** 
   * \brief Rather than Malloc for all three IMG, IMGView & FrameBuffer, We do Malloc once.... reason why this exists 
   * \return false: if (_IMGs.list != nullptr)
   */
  bool alloc_swapchain_Data(void);

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
};

#endif //amVK_WI_H