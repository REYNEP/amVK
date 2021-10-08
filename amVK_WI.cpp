#include "amVK_WI.hh"
#include "amVK.hh"  //Depends on amVK_WI.hh
#include "amVK_Logger.hh"
#include "amVK_Types.hh" //amVK_SWAPCHAIN_IMAGE_FORMAT


amVK_WI::amVK_WI(T_WINDOW_amVK *window, amVK_Device *D, VkSurfaceKHR S) : _window(window), _amVK_D(D), _surface(S) {
    if           (D == nullptr) { LOG_EX("param amVK_Device *D is nullptr...... " << "it has to be valid"); }
    else if (D->_PD == nullptr) { LOG_EX("param amVK_Device *D->_PD is nullptr.." << "it has to be valid"); }
    else if (     S == nullptr) { LOG_EX("param S (_surface) is nullptr.......  " << "it has to be valid"); }

    /** [VUID-VkSwapchainCreateInfoKHR-surface-01270] - vkCreateSwapchain will fail     
     * Val Layer Error if    is_presenting_sup() not done per _surface    (a.k.a Basically per WINDOW) */
    amASSERT(!is_presenting_sup());
}

/**
 * |-----------------------------------------|
 *    - Window System Integration [WSI] -
 * |-----------------------------------------|
 * SwapChain, FIFO (V-Sync), Presentation, Frame-Uncap (IMMEDIATE), Tripple-Buffering
 */
VkSwapchainKHR amVK_WI::create_swapchain(VkSwapchainCreateInfoKHR *CI, bool CI_generic_level0) {
        VkSurfaceCapabilitiesKHR surface_cap;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_amVK_D->_PD, _surface, &surface_cap);
    /** maxImageExtent is not How much GPU can support, its more like HOW MUCH the SURFACE actually Needs  [even tho it says CAPABILITIES]*/
    _extent = surface_cap.minImageExtent;
        LOG("Swapchain Extent: (" << _extent.width << ", " << _extent.height << ")");


    /** 
     * [its more like, WHAT Your Display's FORMAT & COLORSPACE support is + How your GPU wants it]
     * e.g. Your Display might support RGB 10-bit per channel.... But your GPU might want A2B10G10R10 instead of A2R10G10B10
     *   Maybe because of PERF or other under the Hood reasons.... But it really doesn't matter the ORDER of RGB
     *   cz If we (as the DRIVER Programmer) wanted to, we Could have impl. RGB or BGR without any Perf COST
     *   It's just that without giving whole bunch of ~5-20 formats [like AMD], NVIDIA kept it simple and LABELed it what they wanted
     *     (P.S. I think they Label it in BGR order.... cz Probably thats what the MEMORY RePresentation Under The HOOD Looks like)
     *     (I don't think it actually matters, cz.... the mysteries of the DRIVER is like the Mysteries of the HARDWARE-Engineering)
     * 
     * it's as Simple as it Goes.... These are just Some LABELs based on WHAT matches the closest from all the VkFormat enums
     * Don't OVER-THINK it.... just don't..... you hear me Johnny? 
     * 
     * P.S. Watch Cobra Kai.... You'll Like it ;)
     */
    uint32_t n = 0;                                                                         amVK_Array<VkSurfaceFormatKHR> surface_formats{};
    vkGetPhysicalDeviceSurfaceFormatsKHR(_amVK_D->_PD, _surface, &n, nullptr);              surface_formats.data = new VkSurfaceFormatKHR[n];
    vkGetPhysicalDeviceSurfaceFormatsKHR(_amVK_D->_PD, _surface, &n, surface_formats.data);
    LOG_LOOP("------ LIST OF SUPPORTED PRESENTATION-IMAGE FORMATS ------", i, n, "format: " << (uint32_t)surface_formats[i].format << ", colorSpace: " << (uint32_t)surface_formats[i].colorSpace);




    // ----------- THE INFO ------------
    VkSwapchainCreateInfoKHR the_info;
    all_options_covered: {
        if (CI == nullptr) {
        /** this is where you do buffering, e.g. 3 for Triple-Buffering */
            the_info.minImageCount          = 3;
            the_info.presentMode            = VK_PRESENT_MODE_IMMEDIATE_KHR;        /** Uncapped Frames [Less Input Lag]    Still, minImageCount should be 2 */
        /** These 2 Below are reported by hardware [DRIVER].... Are More like what your DISPLAY and DRIVER combination Supports 
         *  U won't need to take care these Formats EXPLICITLY: .... U just need to use SAME format to CREATE RenderPass ColorAttachment & FrameBuffer ImageView 
         *  The DRIVER Handles these IMPLICITLY:, you just need to let the DRIVER know what you are chosing now later on */                      
            the_info.imageFormat            = amVK_SWAPCHAIN_IMAGE_FORMAT;          // See the comment-block before get**SurfaceFormats above
            the_info.imageColorSpace        = amVK_SWAPCHAIN_IMAGE_COLORSPACE;      // (u could choose from SRGB/DCI-P3 if DISPLAY sup & DRIVER Reports)

        /** \todo Relation between RenderPass, Framebuffer, MultiPass/Deffered Rendering, Depth */
            the_info.imageUsage             = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        /** DEFAULT MODS  xD */
            swapchain_CI_mods(&the_info, true);
            CI = &the_info;
        } else {swapchain_CI_mods(CI, CI_generic_level0);}
            CI->surface                = _surface;
            CI->imageExtent            = {_extent.width, _extent.height};                                       
            CI->oldSwapchain           = _swapchain;
        /** DEFAULT MODS END */
    }



    // ----------- Actually Create the VkSwapchainKHR ------------
    finally_create_swapchain:
    {
        vkDeviceWaitIdle(_amVK_D->_D);      //I dont remember actually Why i put this here [LOG: 23 SEPT]
        VkResult res = vkCreateSwapchainKHR(_amVK_D->_D, CI, nullptr, &_swapchain);
        if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res)); LOG("vkCreateSwapchainKHR() failed"); amASSERT(true); return nullptr;}

        this->alloc_swapchain_Data();
        vkGetSwapchainImagesKHR(_amVK_D->_D, _swapchain, &_IMGs.n, _IMGs.list);
        this->create_imageviews();

        return _swapchain;
    }
}

/** More like EXTENDED FEATURES.... */
void amVK_WI::swapchain_CI_mods(VkSwapchainCreateInfoKHR *CI, bool CI_generic_level0) {
    if (CI_generic_level0) {
        CI->sType                  = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        CI->pNext                  = nullptr;
    /** check VK_SWAPCHAIN_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT_KHR if using DEVICEgroup */
        CI->flags                  = 0;

    /** If graphicsQFamily != presentationQFamily, you need to share Images Between qFamiles .... VK_SHARING_MODE_CONCURRENT */
        CI->imageSharingMode       = VK_SHARING_MODE_EXCLUSIVE;            //3060Ti
    /** if imageSharingMode is VK_SHARING_MODE_CONCURRENT .... These should be the qFamilies sharing the Images*/
        CI->queueFamilyIndexCount  = 0;
        CI->pQueueFamilyIndices    = nullptr;

    /** something that you should look at if you wanna do stereoscope3D stuffs */
        CI->imageArrayLayers       = 1;

        CI->preTransform           = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;// e.g rotation / horizontal flip .... if supported by DRIVER
        CI->compositeAlpha         = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;    // [Pre/Post-Multiplied alpha  .... if DRIVER sup (check VkSurfaceCapabilitiesKHR)]
        CI->clipped                = VK_FALSE;   
    }
}







/**
 * Rather than Malloc for all three IMG, IMGView & FrameBuffer, We do Malloc once
 */
bool amVK_WI::alloc_swapchain_Data(void) {
    if (_IMGs.list == nullptr) {   /** _IMGs was Value initialized */
        uint32_t n = 0;
        vkGetSwapchainImagesKHR(_amVK_D->_D, _swapchain, &n, nullptr);

        //VkImage, VkImageView & VkFramebuffer all are POINTERS
        _IMGs.list =           static_cast<      VkImage *>                  (malloc(sizeof(VkImage) * 3 * n));
        _IMGs.views =     reinterpret_cast<  VkImageView *> (_IMGs.list + n);
        _IMGs.framebufs = reinterpret_cast<VkFramebuffer *> (_IMGs.views + n);
        _IMGs.n = n;

        return true;
    } else {
        LOG("amVK_WI._IMGs.list isn't nullptr....   seems like its already ALLOCATED!!!!       [we not allocating] ");
        return false;
    }
}

/** 
 * create the framebuffers for the swapchain images. 
 * This will connect the render-pass to the images for rendering
 * 
 * \param renderPass: Rendering is done in renderPasses.... So RenderPass uses FrameBuffer's Images
 */
VkFramebuffer *amVK_WI::create_framebuffers(VkRenderPass renderPass, VkImageView depthAttachment) {
    VkFramebufferCreateInfo fb_info = {};
        fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fb_info.pNext = nullptr;

        fb_info.renderPass = renderPass;
        fb_info.attachmentCount = 2;
        fb_info.width = _extent.width;
        fb_info.height = _extent.height;
        fb_info.layers = 1;


    VkImageView attachments[2];
    attachments[1] = depthAttachment;
    //create framebuffers for each of the Swapchain Images 
    for (int i = 0, lim = _IMGs.n; i < lim; i++) {
        attachments[0] = _IMGs.views[i];
        fb_info.pAttachments = attachments;
        vkCreateFramebuffer(_amVK_D->_D, &fb_info, nullptr, &_IMGs.framebufs[i]);
    }

    return _IMGs.framebufs;
}

void amVK_WI::create_imageviews(void) {
    VkImageViewCreateInfo the_info = {};
        the_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        the_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    /** The Only Thing that you need to Care about when creating FRAMEBUFFER imageViews
     *    - Should match to RenderPass ColorAttachment.format ---- VUID-VkFramebufferCreateInfo-pAttachments-00880
     *    - Should match to imageFormat passed to SwapchainCreateInfo ---- VUID-VkImageViewCreateInfo-image-01762 */
        the_info.format = amVK_SWAPCHAIN_IMAGE_FORMAT;

    /** describes a remapping from components of the image to components of the vector returned by shader image instructions. */
        the_info.components = {};                       //All zeroes is fine for FRAMEBUFFER
        the_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    
    /** We don't need mipmap for frameBuffers IG   [1, cz VUID-VkImageCreateInfo-mipLevels-00947 mipLevels must be greater than 0] */
        the_info.subresourceRange.baseMipLevel = 0;     //0 means the OG/base Image
        the_info.subresourceRange.levelCount = 1;
        the_info.subresourceRange.baseArrayLayer = 0;   //For StereoScope3D [also see VK_IMAGE_VIEW_TYPE_2D]
        the_info.subresourceRange.layerCount = 1;

    uint32_t n = _IMGs.n;
    for (uint32_t i = 0; i < n; i++) {
        the_info.image = _IMGs.list[i];
        vkCreateImageView(_amVK_D->_D, &the_info, nullptr, (_IMGs.views + i));
    }
}