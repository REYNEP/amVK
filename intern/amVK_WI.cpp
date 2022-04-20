#define amVK_WI_CPP
#include "amVK_WI.hh"
#include "amVK_ImgNBuf.hh"

/** 
   ╻ ╻   ┏━╸┏━┓┏┓╻┏━┓╺┳╸┏━┓╻ ╻┏━╸╺┳╸┏━┓┏━┓
   ╺╋╸   ┃  ┃ ┃┃┗┫┗━┓ ┃ ┣┳┛┃ ┃┃   ┃ ┃ ┃┣┳┛
   ╹ ╹   ┗━╸┗━┛╹ ╹┗━┛ ╹ ╹┗╸┗━┛┗━╸ ╹ ┗━┛╹┗╸
 */
amVK_WI_MK2::amVK_WI_MK2(const char *window, amVK_SurfaceMK2 *S, amVK_RenderPassMK2 *RP, amVK_DeviceMK2 *D) : m_windowName(window), amVK_D(D), amVK_S(S), surface(S->S), amVK_RP(RP) {
         if (window == nullptr) { amVK_LOG_EX("Please dont pass nullptr, ERROR/WARNING Logs will look messy, this is used to let you know which window ");
                                  window = "nullptr";}
    if           (D == nullptr) { amVK_LOG_EX("[param]        amVK_DeviceMK2 *D      is nullptr.... name: " << window); }
    else if (D->PD == nullptr) {  amVK_LOG_EX("[param]        amVK_DeviceMK2 *D->_PD is nullptr.... name: " << window); }
         if (     S == nullptr) { amVK_LOG_EX("[param]       amVK_SurfaceMK2 *S      is nullptr.... name: " << window); }
         if (    RP == nullptr) { amVK_LOG_EX("[param]    amVK_RenderPassMK2 *RP     is nullptr.... name: " << window); }


    if (amVK_S->PD != amVK_D->PD) {
        amVK_LOG_EX("[param]       \u0027amVK_SurfaceMK2 *S->VkSurface _S\u0027, wasn't created on the same PhysicalDevice as _amVK_Device->_PD....  This will probably fail....");
    }
    amASSERT(!amVK_S->is_presenting_sup());

    Default_the_info();
    fallback_the_info();
    _LOG0("amVK_WI_MK2 Constructed   &   Default_the_info() called...");
}


/** \see \fn amVK_SurfaceMK2::is_presenting_sup() */
uint32_t amVK_SurfaceMK2::present_qFam(void) {
    uint32_t PD_index = HEART->SPD.index(PD);
    amVK_Array<VkQueueFamilyProperties> qFAM_list = HEART->SPD.qFamily_lists[PD_index];

    VkBool32 sup = false;
    for (int i = 0, lim = qFAM_list.n; i < lim; i++) {
        vkGetPhysicalDeviceSurfaceSupportKHR(PD, i, S, &sup);
        if (sup == true) { present_qFamily = i;  found_present_qFamily = true; }
    }

    if (present_qFamily == 0xFFFFFFFF) {amVK_LOG_EX("Couldn't Find any qFamily on PhysicalDevice that supports Presentation to given surface....");}
    return present_qFamily;
}


VkPhysicalDevice amVK_SurfaceMK2::select_DisplayDevice(void) {
    if (!HEART->SPD.list) {HEART->load_PD_info(false, true);}

    /** we are gonna choose a GPU that supports presenting to the surface.... */
    VkPhysicalDevice *PD_list = HEART->SPD.list;
    uint32_t PD_n = HEART->SPD.n;

    /** The list is already sorted.... */
    for (int i = 0; i < PD_n; i++) {
        PD = PD_list[i];
        if (is_presenting_sup()) return PD;     /** calls \fn above, present_qFam() */
    }

    amVK_LOG_EX("Couldn't create DisplayDevice.... no VkPhysicalDevice seems to support presenting to required surface.... [return nullptr]");
    return nullptr;
}




/** 
    \│/  ┌─┐┌─┐┌┬┐   ╔═╗┬ ┬┬─┐┌─┐┌─┐┌─┐┌─┐╔═╗┌─┐┬─┐┌┬┐┌─┐┌┬┐┌─┐  
    ─ ─  │ ┬├┤  │    ╚═╗│ │├┬┘├┤ ├─┤│  ├┤ ╠╣ │ │├┬┘│││├─┤ │ └─┐  
    /│\  └─┘└─┘ ┴────╚═╝└─┘┴└─└  ┴ ┴└─┘└─┘╚  └─┘┴└─┴ ┴┴ ┴ ┴ └─┘  
 */
void amVK_SurfaceMK2::get_SurfaceFormats(bool print) {
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
     * 
     * 
     *  These 2 Below are reported by hardware [DRIVER].... Are More like what your DISPLAY and DRIVER combination Supports 
     *  U won't need to take care these Formats EXPLICITLY: .... U just need to use SAME format to CREATE RenderPass ColorAttachment & FrameBuffer ImageView 
     *  The DRIVER Handles these IMPLICITLY:, you just need to let the DRIVER know what you are chosing now later on 
     */

    amVK_Array<VkSurfaceFormatKHR> tmp_surface_formats = {};
    vkGetPhysicalDeviceSurfaceFormatsKHR(PD, S, &tmp_surface_formats.n, nullptr);              tmp_surface_formats.data = new VkSurfaceFormatKHR[tmp_surface_formats.n];
    vkGetPhysicalDeviceSurfaceFormatsKHR(PD, S, &tmp_surface_formats.n, tmp_surface_formats.data);

    /** calling get_SurfaceFormats 2nd time gives NULL values.... (nvidia, win10) */
    if (tmp_surface_formats.data != nullptr && tmp_surface_formats.n > 0) {
        surface_formats = tmp_surface_formats;

        if (print) {
            _LOG_LOOP("------ LIST OF SUPPORTED PRESENTATION-IMAGE FORMATS ------", i, surface_formats.n, 
                "format: " << (uint32_t)surface_formats.data[i].format << ", colorSpace: " << (uint32_t)surface_formats.data[i].colorSpace);
        }
    }
}

/** 
 * other amVK_SurfaceMK2 Get stuffs....
 */
amVK_SurfaceCaps amVK_SurfaceMK2::get_SurfaceCapabilities_II(void) {
    VkSurfaceCapabilitiesKHR surfaceCaps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PD, S, &surfaceCaps);
    
    amVK_SurfaceCaps caps;
    caps.maxImageArrayLayers = surfaceCaps.maxImageArrayLayers;
    caps.supportedTransforms = surfaceCaps.supportedTransforms;
    caps.currentTransform    = surfaceCaps.currentTransform;
    caps.supportedCompositeAlpha = surfaceCaps.supportedCompositeAlpha;
    caps.supportedUsageFlags = surfaceCaps.supportedUsageFlags;
    return caps;
}
void amVK_SurfaceMK2::get_PresentModes(void) {
    amVK_Array<VkPresentModeKHR> tmp_present_modes = {};
    vkGetPhysicalDeviceSurfacePresentModesKHR(PD, S, &tmp_present_modes.n, nullptr);           tmp_present_modes.data = new VkPresentModeKHR[tmp_present_modes.n];
    vkGetPhysicalDeviceSurfacePresentModesKHR(PD, S, &tmp_present_modes.n, tmp_present_modes.data);

    if (tmp_present_modes.data != nullptr && tmp_present_modes.n > 0) {
        present_modes = tmp_present_modes;
    }
}









/** 
     \│/  ┌─┐┬┬ ┌┬┐┌─┐┬─┐    ╔═╗┬ ┬┬─┐┌─┐┌─┐┌─┐┌─┐╔═╗┌─┐┬─┐┌┬┐┌─┐┌┬┐
     ─ ─  ├┤ ││  │ ├┤ ├┬┘    ╚═╗│ │├┬┘├┤ ├─┤│  ├┤ ╠╣ │ │├┬┘│││├─┤ │ 
     /│\  └  ┴┴─┘┴ └─┘┴└─────╚═╝└─┘┴└─└  ┴ ┴└─┘└─┘╚  └─┘┴└─┴ ┴┴ ┴ ┴ 
 * \brief
 * cz we needed to do this in RenderPassMK2, fallbacks on B8G8R8A8/R8G8B8A8_SRGB 
 */
VkSurfaceFormatKHR amVK_SurfaceMK2::filter_SurfaceFormat(VkSurfaceFormatKHR what_to_filter) {
    bool found_it = false;  //We gonna try and make sure that RenderPass imageFormat and imageColorSpace is VALID for this surface....
    bool found_r8g8b8a8 = false;    /** \todo find non-sRGB ones too! */
    bool found_b8g8r8a8 = false;    //one of these two will be available anyway on all consumer monitors

    this->get_SurfaceFormats();
    
    _LOG("  [searching...] imageFormat: " << what_to_filter.format << ", imageColorSpace: " << what_to_filter.colorSpace);

    for (int i = 0; i < surface_formats.n; i++) {
        if ((surface_formats.data[i].format     == what_to_filter.format) &&
            (surface_formats.data[i].colorSpace == what_to_filter.colorSpace)) {
            found_it = true;
            break;
        }
        else if (surface_formats.data[i].format == VK_FORMAT_R8G8B8A8_SRGB) {
            found_r8g8b8a8 = true;
        }
        else if (surface_formats.data[i].format == VK_FORMAT_B8G8R8A8_SRGB) {
            found_b8g8r8a8 = true;
        }
    }
    if (!found_it) {
        amVK_LOG_EX("SurfaceFormat {" << VkFormat_2_String(what_to_filter.format) << ", " << VkColorSpace_2_String(what_to_filter.colorSpace) << "}.... " << "not supported." << amVK::endl
            << "[Note that these 2 should also match with RenderPass's ImageFormat/ColorSpace....  so you need to configure using amVK_RenderPassMK2 stuffs]");

             if (found_r8g8b8a8) {                what_to_filter.format = VK_FORMAT_R8G8B8A8_SRGB; 
                amVK_LOG("Returning \u2022" <<               VkFormat_2_String(VK_FORMAT_R8G8B8A8_SRGB)           << "\u2022 [ from amVK_SurfaceMK2::filter_SurfaceFormat() ]");}
        else if (found_b8g8r8a8) {                what_to_filter.format = VK_FORMAT_B8G8R8A8_SRGB; 
                amVK_LOG("Returning \u2022" <<               VkFormat_2_String(VK_FORMAT_B8G8R8A8_SRGB)           << "\u2022 [ from amVK_SurfaceMK2::filter_SurfaceFormat() ]");}

        what_to_filter.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; 
                amVK_LOG("Returning \u2022" <<      VkColorSpace_2_String(VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)      << "\u2022 [ from amVK_SurfaceMK2::filter_SurfaceFormat() ]");
    }

    return what_to_filter;  /** it has been filtered & modified */
}







/**
 *  ╻ ╻   ┏━┓╻ ╻┏━┓┏━┓┏━╸╻ ╻┏━┓╻┏┓╻
 *  ╺╋╸   ┗━┓┃╻┃┣━┫┣━┛┃  ┣━┫┣━┫┃┃┗┫
 *  ╹ ╹   ┗━┛┗┻┛╹ ╹╹  ┗━╸╹ ╹╹ ╹╹╹ ╹
 */
void amVK_WI_MK2::fallback_the_info(void) {
    // ----------- PRESENT MODE SUPPORTED ------------
        amVK_S->get_PresentModes();
        amVK_Array<VkPresentModeKHR> present_modes = amVK_S->present_modes;

        bool found_immediate = false;
        amVK_ARRAY_IS_ELEMENT(present_modes, found_immediate, the_info.presentMode);
        if (!found_immediate) {
            this->the_info.presentMode = VK_PRESENT_MODE_FIFO_KHR; 
            amVK_LOG("swapchain presentMode " << the_info.presentMode << " not available, using FIFO");
        }

    // ----------- IMAGE COUNT SUPPORTED ------------
        VkSurfaceCapabilitiesKHR surfaceCaps;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(amVK_D->PD, surface, &surfaceCaps);

        uint32_t target_img_cnt = the_info.minImageCount + surfaceCaps.minImageCount;
        if (target_img_cnt > surfaceCaps.maxImageCount) {
            if (the_info.minImageCount == surfaceCaps.maxImageCount) {  //This will be true only if the above if true
                amVK_LOG("You have requested " << the_info.minImageCount << " Images. Thats the max amount this surface's capability is.");
            }
            else if (the_info.minImageCount > surfaceCaps.maxImageCount) {
                amVK_LOG("You have requested " << the_info.minImageCount << " Images. surfaceCaps.maxImageCount = " << surfaceCaps.maxImageCount);
                amVK_LOG("We are capping it, to surfaceCaps.maxImageCount");
            }
            else if (the_info.minImageCount < surfaceCaps.maxImageCount) {
                amVK_LOG("You have requested " << the_info.minImageCount << " Images. surfaceCaps.maxImageCount = " << surfaceCaps.maxImageCount);
                amVK_LOG("you are safe!");
            }

            /** Finally set the_info.minImageCount.... \note \see Default_the_info */
            amVK_LOG("in create_Swapchain we do,    the_info.minImageCount += surfaceCaps.minImageCount;     [\see create_Swapchain]");
            amVK_LOG("So we are setting here        the_info.minImageCount  = surfaceCaps.maxImageCount - surfaceCaps.minImageCount");
            the_info.minImageCount = surfaceCaps.maxImageCount - surfaceCaps.minImageCount;
        }


     // ----------- SURFACE FORMAT SUPPORTED ------------
        VkSurfaceFormatKHR _final = amVK_S->filter_SurfaceFormat({the_info.imageFormat, the_info.imageColorSpace});
        the_info.imageFormat = _final.format;
        the_info.imageColorSpace = _final.colorSpace;

        /** RenderPass filters too.... but why not make it future proof double check.... cz i dont feel like coming here and changing stuffs inside this function. */
        if (the_info.imageFormat != amVK_RP->final_imageFormat) {
            amVK_LOG("amVK_WI_MK2.the_info.imageFormat != _amVK_RP->final_imageFormat" << amVK::endl << "will result in wrong colors" << "Window: " << m_windowName);
        }
        if (the_info.imageColorSpace != amVK_RP->final_imageColorSpace) {
            amVK_LOG("amVK_WI_MK2.the_info.imageColorSpace != _amVK_RP->final_imageColorSpace" << amVK::endl << "will result in wrong colors" << "Window: " << m_windowName);
        }
}


/**
    \│/  ┌─┐┬─┐┌─┐┌─┐┌┬┐┌─┐    ╔═╗┬ ┬┌─┐┌─┐┌─┐┬ ┬┌─┐┬┌┐┌
    ─ ─  │  ├┬┘├┤ ├─┤ │ ├┤     ╚═╗│││├─┤├─┘│  ├─┤├─┤││││
    /│\  └─┘┴└─└─┘┴ ┴ ┴ └─┘────╚═╝└┴┘┴ ┴┴  └─┘┴ ┴┴ ┴┴┘└┘
 */
VkSwapchainKHR amVK_WI_MK2::create_Swapchain(bool check_the_info) {
    if (check_the_info) {
        fallback_the_info();
    }

    VkSurfaceCapabilitiesKHR surfaceCaps;
    // ----------- get_SurfaceExtent() ------------
    get_SurfaceExtent: {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(amVK_D->PD, surface, &surfaceCaps);
        /** maxImageExtent is not How much GPU can support, its more like HOW MUCH the SURFACE actually Needs  [even tho it says CAPABILITIES] */
        extent = surfaceCaps.minImageExtent;
        amVK_LOG("");
        amVK_LOG("Swapchain Extent: (" << extent.width << ", " << extent.height << ")");
    }

    uint8_t imageCount_fix = surfaceCaps.minImageCount; /** be safe & sound */
    the_info.minImageCount += imageCount_fix;   /** Decremented at func end */

    the_info.imageExtent            = {extent.width, extent.height};
    the_info.oldSwapchain           = swapchain;

    finally: {
        vkDeviceWaitIdle(amVK_D->D);          /** a lot of people says its better to just wait for the device to be idle, but why */
        VkResult res = vkCreateSwapchainKHR(amVK_D->D, &the_info, nullptr, &swapchain);
        if (res != VK_SUCCESS) {amVK_LOG_EX(amVK_Utils::vulkan_result_msg(res)); amVK_LOG("vkCreateSwapchainKHR() failed"); amASSERT(true); return nullptr;}
        
        if (the_info.oldSwapchain) {amVK_LOG("Swapchain ReCreated.");}
        else {amVK_LOG("Swapchain Created! Time TraveL....");}

        /** \todo what if someone suddenly turns off imageless_framebuffer or increases the_info.minImageCount */
        post_create_swapchain();
    }

    the_info.minImageCount -= imageCount_fix;   /** lets not make people confused after create_swapchain(); returns.... so we reset */
    return swapchain;
}

/** More like EXTENDED FEATURE.... kinda stuffs */
void amVK_WI_MK2::swapchain_CI_generic_mods(void) {
    the_info.sType                  = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    the_info.pNext                  = nullptr;
/** check VK_SWAPCHAIN_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT_KHR if using DEVICEgroup */
    the_info.flags                  = 0;

/** If graphicsQFamily != presentationQFamily, you need to share Images Between qFamiles .... VK_SHARING_MODE_CONCURRENT */
    the_info.imageSharingMode       = VK_SHARING_MODE_EXCLUSIVE;            //3060Ti
/** if imageSharingMode is VK_SHARING_MODE_CONCURRENT .... These should be the qFamilies sharing the Images*/
    the_info.queueFamilyIndexCount  = 0;
    the_info.pQueueFamilyIndices    = nullptr;

/** something that you should look at if you wanna do stereoscope3D stuffs */
    the_info.imageArrayLayers       = 1;

    the_info.preTransform           = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;// e.g rotation / horizontal flip .... if supported by DRIVER
    the_info.compositeAlpha         = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;    // [Pre/Post-Multiplied alpha  .... if DRIVER sup (check VkSurfaceCapabilitiesKHR)]
    the_info.clipped                = VK_FALSE;   
}










/**
 *             ███╗   ███╗██╗  ██╗██████╗ 
 *   ▄ ██╗▄    ████╗ ████║██║ ██╔╝╚════██╗
 *    ████╗    ██╔████╔██║█████╔╝  █████╔╝
 *   ▀╚██╔▀    ██║╚██╔╝██║██╔═██╗ ██╔═══╝ 
 *     ╚═╝     ██║ ╚═╝ ██║██║  ██╗███████╗
 *             ╚═╝     ╚═╝╚═╝  ╚═╝╚══════╝
 */
void amVK_WI_MK2::post_create_swapchain(void) {
    if (!IMGs.alloc_called) {
        uint32_t swap_imgs_n = 0;
        vkGetSwapchainImagesKHR(amVK_D->D, swapchain, &swap_imgs_n, nullptr);
        amVK_LOG("Swapchain Images N: " << swap_imgs_n);
        
        IMGs.framebuf_n = (uint8_t) swap_imgs_n;
        IMGs.attach_n = amVK_RP->attachment_descs.n;
        IMGs.color_index = amVK_RP->color_index;
        IMGs.swap_attach_index =  IMGs.color_index;   /** \todo swap_imgs could be used in other ways too */
        /** \todo    IMGs.imageless_framebuffer = false */
        IMGs.i_alloc();
    }

    uint32_t smth = IMGs.framebuf_n;
    vkGetSwapchainImagesKHR(amVK_D->D, swapchain, &smth, IMGs.i_ptr_img(0, IMGs.swap_attach_index));

    VkImageViewCreateInfo CI = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, nullptr, 0,
        nullptr,    /** .image - set in the loop below */
        VK_IMAGE_VIEW_TYPE_2D,

        /** [.format]
         * The Only Thing that you need to Care about when creating FRAMEBUFFER imageViews
         *    - Should match to RenderPass ColorAttachment.format ---- VUID-VkFramebufferCreateInfo-pAttachments-00880
         *    - Should match to imageFormat passed to SwapchainCreateInfo ---- VUID-VkImageViewCreateInfo-image-01762 
         * 
         * It’s possible for the formats to not match, and that will let you “reinterpret” the formats, 
         *    but that can be tricky to use, and very niche, so for now make sure that the formats will match. - vblanco */
        the_info.imageFormat,

        /** [.components] If you wanna swap RGBA channels/components, All zeroes are okay, means stay how it was */
        {},

        /** [.subresourceRange] 
         * subresourceRange holds the information about where the image points to. 
         *  This is used for layered images, where you might have multiple layers in one image, and want to create an imageview that points to a specific layer. 
         *  It’s also possible to control the mipmap levels with it. 
         *  For our current usage, we are going to default it to no mipmaps (mipmap base 0, and mipmaps level 1), and only 1 texture layer. - vblanco */
        {   
            VK_IMAGE_ASPECT_COLOR_BIT,     /** [.aspectMask] */
            /*  We don't need mipmap for frameBuffers IG   [1, cz VUID-VkImageCreateInfo-mipLevels-00947 mipLevels must be greater than 0] */
            0, 1, /** [.baseMipLevel], [.levelCount]   -   0 means the OG/base Image i guess.... */

            0, 1  /** [.baseArrayLayer], [.layerCount] - For StereoScope3D [also see VK_IMAGE_VIEW_TYPE_2D] */
        }
    };

    for (uint32_t i = 0; i < IMGs.framebuf_n; i++) {
        CI.image = IMGs.i_ptr_img(0, IMGs.swap_attach_index)[i];

        VkResult res = vkCreateImageView(amVK_D->D, &CI, nullptr, IMGs.i_ptr_attach(i, IMGs.swap_attach_index));
        if (res != VK_SUCCESS) {amVK_Utils::vulkan_result_msg(res);}
        _LOG("[Swapchain]_ IMGs.attachments[" << (((uint64_t)IMGs.i_ptr_attach(i, IMGs.swap_attach_index) - (uint64_t)IMGs.i_ptr_attach(0, IMGs.swap_attach_index)) / (uint64_t)8) << "]");
    }
    _LOG("");
}



VkImageCreateInfo imgCI = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO, nullptr, 0,
    VK_IMAGE_TYPE_2D, VK_FORMAT_UNDEFINED, {},

    1, 1, /** [.mipLevels], [.arrayLayers] */
    VK_SAMPLE_COUNT_1_BIT, /** [.samples] - where you gotta specify MSAA */
    
    VK_IMAGE_TILING_OPTIMAL, /** [.tiling], \todo */
    0,      /** [.usage] */
    
    VK_SHARING_MODE_EXCLUSIVE, 0, nullptr, /** [.VkSharingMode] */
    VK_IMAGE_LAYOUT_UNDEFINED   /** [.initialLayout] */
};

VkImageViewCreateInfo viewCI = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, nullptr, 0,
    nullptr,   VK_IMAGE_VIEW_TYPE_2D,
    VK_FORMAT_UNDEFINED,

    {}, /** [.components] */
    {   /** [.subresourceRange] */
        0,     /** [.aspectMask] */
        0, 1, /** [.baseMipLevel], [.levelCount] */
        0, 1  /** [.baseArrayLayer], [.layerCount] */
    }
};

void amVK_WI_MK2::create_Attachments(void) {
    // ----------- CREATE ATTACHMENT/IMGVIEWS FOR RENDERPASS ------------
    for (int i = 0; i < IMGs.attach_n; i++) {
        if (i == IMGs.swap_attach_index) {
            continue;
        }

        //We dont modify ImageMK2::CI & view_CI values, so we have our custom 'imgCI' & 'viewCI'
        imgCI.extent = {extent.width, extent.height, 1};
        imgCI.samples = amVK_RP->samples;

        finally_create:
        {
            for (int fi = 0; fi < IMGs.framebuf_n; fi++) {
                imgCI.format = amVK_RP->attachment_descs[i].format;
                imgCI.usage  = image_layout_2_usageflags(amVK_RP->attachment_descs[i].finalLayout);
                viewCI.format = amVK_RP->attachment_descs[i].format;
                viewCI.subresourceRange.aspectMask = image_layout_2_aspectMask(amVK_RP->attachment_descs[i].finalLayout);

                ImageMK2 xd;
                xd.amvkCreateImage(&imgCI, &viewCI, amVK_D);   //Creates Image + View

                *(IMGs.i_ptr_attach(fi, i)) = xd.VIEW;
                *(IMGs.i_ptr_img   (fi, i)) = xd.IMG;
                *(IMGs.i_ptr_mem   (fi, i)) = xd.MEMORY;
                _LOG("Created attachment: IMGs.attachments[" << ((fi * IMGs.attach_n) + i) << "],  IMGs.images[" << ((i * IMGs.framebuf_n) + fi) << "]");
            }
            _LOG("");

            ClearValues: {
                set_RenderPassClearVals();
            }
        }
    }
}


void amVK_WI_MK2::create_Framebuffers(void) {
    // ----------- \todo imageless_framebuffer & Separate create_Framebuffer_s------------

    VkFramebufferCreateInfo fb_info = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO, nullptr, 0,
        amVK_RP->RP, amVK_RP->attachment_descs.n, nullptr, /** [.pAttachments] set in the loop below */
        extent.width, extent.height, 1 /** [.layers] */
    };

    for (int fi = 0, lim = IMGs.framebuf_n; fi < lim; fi++) {
        fb_info.pAttachments = IMGs.i_ptr_attach(fi, 0);
        vkCreateFramebuffer(amVK_D->D, &fb_info, nullptr, &IMGs.framebufs[fi]);
    }
}


bool amVK_WI_MK2::destroy(bool only_cleanup_for_reCreate) {
    if (swapchain) {
      vkDeviceWaitIdle(amVK_D->D);
      // ----------- FrameBuffers ------------
      for (int i = 0; i < IMGs.framebuf_n; i++) {
        vkDestroyFramebuffer(amVK_D->D, IMGs.framebufs[i], nullptr);
      }
      // ----------- DepthAttachments for now only ------------
      for (int i = 0; i < IMGs.attach_n; i++) {
        if (i == IMGs.swap_attach_index) {
            continue;   //Gotta skip Swapchain Images ones....
        }
        for (int fi = 0; fi < IMGs.framebuf_n; fi++) {
          ImageMK2::amvkDestroyImage(
              *( IMGs.i_ptr_img   (fi, i) ),
              *( IMGs.i_ptr_attach(fi, i) ),
              *( IMGs.i_ptr_mem   (fi, i) ),
              amVK_D
          );
        }
      }
      // ----------- Swapchain Images' imageviews ------------
      for (int i = 0; i < IMGs.framebuf_n; i++) {
        vkDestroyImageView(amVK_D->D, *(IMGs.i_ptr_attach(i, IMGs.swap_attach_index)), nullptr);
      }
      // ----------- Finally Destroy Swapchain & Free -----------
      if (!only_cleanup_for_reCreate) {
        vkDestroySwapchainKHR(amVK_D->D, swapchain, nullptr);
        _LOG0("Swapchain has been Destroyed! BYE BYE!");
      }
      IMGs.i_free();

      _LOG0("Framebuffers, Attachments, Images, SwapchainImages has been Destroyed!");
      return true;
    }
    else {return false;}
}
