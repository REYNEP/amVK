#ifndef amVK_WI_H
/** 
 * Just like WSI, we got WI 'Window Integration/Interface' (whatever).... TIME TRAVEL!!!!  
*/
#define amVK_WI_H
#include <vector>
#include "vulkan/vulkan.h"
#ifndef amVK_DEVICE_H
  #include "amVK_Device.hh"
#endif

#ifndef T_WINDOW_amVK
  #define T_WINDOW_amVK amGHOST_Window  //Use whatever Hell type you wish to use
#endif
class T_WINDOW_amVK;

//IMG, IMGviews, Framebuffers
typedef struct swapchain_Data {
  VkFramebuffer  *framebuffers;
  VkRenderPass   *renderpass;
  VkImage        *IMGs;
  VkImageView    *IMGviews;
  uint32_t        IMGs_n;
} swapchain_Data;

/**
 * |-----------------------------------------|
 *    - Window Integration/Interface [WI] -
 * |-----------------------------------------|
 * SwapChain, FIFO (V-Sync), Presentation, Frame-Uncap (IMMEDIATE), Tripple-Buffering
 * Check T_WINDOW_amVK
 * NOTE: Name taken from WSI: Window 'System' Integration ;)
 * DIFF: This is not WSI; Window 'System' Integration.... This is more like something we gotta have Per Window.
 *       So we name it Window Interface/Integration
 */
class amVK_WI {
  public:
    T_WINDOW_amVK *_window;       //That 1-thing that Distinguishes 1 Object from other amVK_WI objects
    amVK_Device *_amVK_D;
    
    VkExtent2D    _extent = {};   //min and max is like almost always same on NVIDIA
    VkSurfaceKHR _surface = nullptr;

    VkSwapchainKHR   _swapchain = nullptr;
    swapchain_Data _Data = {};    //IMG, IMGviews, Framebuffers data about Swapchain

    /**
     * CONSTRUCTOR
     * \param window: use macro T_WINDOW_amVK to change this Parameter's Type (along with _window memberVar)
     * \param device: The device that is Gonna manage Stuffs like SWAPCHAIN, Images, FrameBuffers
     * \param surface: So apparantly Every Surface should have its own Swapchain IG....
     */
    amVK_WI(T_WINDOW_amVK *window, amVK_Device *D, VkSurfaceKHR S) : _window(window), _amVK_D(D), _surface(S) {}
    ~amVK_WI() {}

    
    /**
     * You can do this yourself before creating Swapchain
     * \param qFamily_Index: the qFamily Index of Device to check/query presentation for
     * \return true: if qFamily_Index of this WI's device has support for Presentation
     */
    VkBool32 is_present_sup(uint32_t qFamily_Index);

    //I considered having that CreateSurface function here. But it feels right and Better to just TUNNELE with amGHOST, as GLFW is also like that
    //vkGetDeviceQueue(LOGICAL_DEVICE, qFAMILY_INDEX, QUEUE_INDEX, VkQueue_THE_QUEUE);
    //https://github.com/KhronosGroup/Vulkan-Samples/blob/master/samples/performance/swapchain_images/swapchain_images_tutorial.md
    /**
     * Calls vkGetSwapchainImagesKHR()
     * .oldSwapchain used if exists
     * 
     *     Setting oldSwapChain to the saved handle of the previous swapchain aids in resource reuse and makes sure that we can still present already acquired images   
     *     - [- Sascha Williems] 
     * \param full: if yes.... crete_imageviews & create_framebuffers is called
     */
    VkSwapchainKHR create_swapchain(bool full = false);

    
    //Rather than Malloc for all three IMG, IMGView & FrameBuffer, We do Malloc once
    void alloc_swapchain_Data(void);
    //For now Function above, Returns HEAP-allocated VkImage Pointers
    VkFramebuffer *amVK_WI::create_framebuffers(VkRenderPass renderPass, VkImageView depthAttachment);

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

#endif //#ifndef amVK_WI_H