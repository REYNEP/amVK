#include "amVK_WI.hh"
#include "amVK.hh"  //Depends on amVK_WI.hh
#include "amVK_Logger.hh"
#include "amVK_Types.hh" //amVK_SWAPCHAIN_IMAGE_FORMAT

VkBool32 amVK_WI::is_present_sup(uint32_t qFamily_Index) {
    VkBool32 res = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(amVK_CX::heart->_vkDevice_list[_amVK_D->_D]->_PD, qFamily_Index, _surface, &res);
    return res;
}

/**
 * |-----------------------------------------|
 *    - Window System Integration [WSI] -
 * |-----------------------------------------|
 * SwapChain, FIFO (V-Sync), Presentation, Frame-Uncap (IMMEDIATE), Tripple-Buffering
 * IMMEDIATE https://discord.com/channels/427551838099996672/427833316536746005/861359982469447720
 */
VkSwapchainKHR amVK_WI::create_swapchain(bool full) {
    //Get PhysicalDevice
    VkPhysicalDevice PD = amVK_CX::heart->_vkDevice_list[_amVK_D->_D]->_PD;
    
    //VUID-VkSwapchainCreateInfoKHR-surface-01270
    VkBool32 is_sup_surface = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(PD, 0, _surface, &is_sup_surface);

    //More like what the Combination of GPU and DISPLAY and other hardware actually Supports.... e.g. Extent
    VkSurfaceCapabilitiesKHR surface_cap;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PD, _surface, &surface_cap);
    _extent = surface_cap.minImageExtent;
    LOG("Swapchain Extent: (" << _extent.width << ", " << _extent.height << ")");
    //minImageExtent == maxImageExtent; in most cases [even after resize on windows]

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
    uint32_t n = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(PD, _surface, &n, nullptr);
    std::vector<VkSurfaceFormatKHR> surface_formats(n);                 //TODO: Add support for Choosing SURFACE-FORMAT
    vkGetPhysicalDeviceSurfaceFormatsKHR(PD, _surface, &n, surface_formats.data());

    LOG(std::endl << std::endl);
    LOG("------ LIST OF SUPPORTED PRESENTATION-IMAGE FORMATS ------");
    LOG("Support for List SOON");

    /** All Option COVERED !!!! */
    VkSwapchainCreateInfoKHR the_info = {};
        the_info.sType                  = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        the_info.pNext                  = nullptr;
    /** check VK_SWAPCHAIN_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT_KHR if using DEVICEgroup */
        the_info.flags                  = 0;
        the_info.surface                = _surface;
    /** we want at least Triple-Buffering Support - TODO: Test this with MULTI-THREADING.... MORE MEMORY CONSUMPTION TODO: TEST how muich */
        the_info.minImageCount          = 3;
    /** These 2 Below are reported by hardware [DRIVER].... Are More like what your DISPLAY and DRIVER combination Supports 
     *  U won't need to take care these Formats EXPLICITLY: .... U just need to use SAME format to CREATE RenderPass ColorAttachment & FrameBuffer ImageView 
     *  The DRIVER Handles these IMPLICITLY:, you just need to let the DRIVER know what you are chosing now later on */                      
        the_info.imageFormat            = amVK_SWAPCHAIN_IMAGE_FORMAT;          // See the comment-block before get**SurfaceFormats above
        the_info.imageColorSpace        = amVK_SWAPCHAIN_IMAGE_COLORSPACE;      // (u could choose from SRGB/DCI-P3 if DISPLAY sup & DRIVER Reports)
   
    /** most of the Time _minExtent == _maxExtent */
        the_info.imageExtent            = {_extent.width, _extent.height};
    /** something that you should look at if you wanna do stereoscope3D stuffs */
        the_info.imageArrayLayers       = 1;
    /** Gotta be this CZ we wanna create IMAGEVIEWS for FRAMEBUFFER-Creation */
        the_info.imageUsage             = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    /** If graphicsQFamily != presentationQFamily, you need to share Images Between qFamiles .... VK_SHARING_MODE_CONCURRENT */
        the_info.imageSharingMode       = VK_SHARING_MODE_EXCLUSIVE;            //3060Ti
    /** if imageSharingMode is VK_SHARING_MODE_CONCURRENT .... These should be the qFamilies sharing the Images*/
        the_info.queueFamilyIndexCount  = 0;
        the_info.pQueueFamilyIndices    = nullptr;


        the_info.preTransform           = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;// e.g rotation / horizontal flip .... if supported by DRIVER
        the_info.compositeAlpha         = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;    // [Pre/Post-Multiplied alpha  .... if DRIVER sup (check VkSurfaceCapabilitiesKHR)]
        the_info.clipped                = VK_FALSE;                             // Who wants clipping? Is it really VISUAL or MEMORY only Clipping?
    
    /** aids in resource reuse and makes sure that we can still present already acquired images        - Sascha Williems */                                                            
        the_info.oldSwapchain           = _swapchain;
        the_info.presentMode            = VK_PRESENT_MODE_IMMEDIATE_KHR;        /** Uncapped Frames [Less Input Lag] */


    vkDeviceWaitIdle(_amVK_D->_D);      //I dont remember actually Why i put this here [LOG: 23 SEPT]
    VkResult res = vkCreateSwapchainKHR(_amVK_D->_D, &the_info, nullptr, &_swapchain);
    if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res)); LOG("vkCreateSwapchainKHR() failed"); return nullptr;}    //TODO: LOG THE INFO

    LOG("vkCreateSwapchainKHR returned Experimental alloc_swapchain_Data");
    this->alloc_swapchain_Data();
    vkGetSwapchainImagesKHR(_amVK_D->_D, _swapchain, &_Data.IMGs_n, _Data.IMGs);
    if (full) {
        LOG("amVK_WI::create_swapchain     param 'full' = true      ---- creating ImageViews !");
        this->create_imageviews();
    }
    LOG("Experimental alloc_swapchain_Data passed");

    return _swapchain;
}








/**
 * Rather than Malloc for all three IMG, IMGView & FrameBuffer, We do Malloc once
 */
void amVK_WI::alloc_swapchain_Data(void) {
    if (_Data.IMGs == nullptr) {   /** _Data was Value initialized */
        uint32_t n = 0;
        vkGetSwapchainImagesKHR(_amVK_D->_D, _swapchain, &n, nullptr);

        //VkImage, VkImageView & VkFramebuffer all are POINTERS
        _Data.IMGs =               (VkImage *)                        malloc(sizeof(VkImage) * 3 * n);
        _Data.IMGviews =       (VkImageView *) (_Data.IMGs + (1 * n));
        _Data.framebuffers = (VkFramebuffer *) (_Data.IMGs + (2 * n));
        _Data.IMGs_n = n;
    } else {
        LOG("amVK_WI._Data.IMGs isn't nullptr....   seems like its already ALLOCATED!!!!       [we not allocating] ");
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
	for (int i = 0; i < _Data.IMGs_n; i++) {
        attachments[0] = _Data.IMGviews[i];
		fb_info.pAttachments = attachments;
		vkCreateFramebuffer(_amVK_D->_D, &fb_info, nullptr, &_Data.framebuffers[i]);
	}

    return _Data.framebuffers;
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

    uint32_t n = _Data.IMGs_n;
    for (uint32_t i = 0; i < n; i++) {
        the_info.image = _Data.IMGs[i];
        vkCreateImageView(_amVK_D->_D, &the_info, nullptr, (_Data.IMGviews + i));
    }
}