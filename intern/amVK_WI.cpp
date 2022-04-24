#include "amVK_WI.hh"
#include "amVK_ImgNBuf.hh"

/** 
 *  ╻ ╻   ┏━┓┏┳┓╻ ╻╻┏    ┏━┓╻ ╻┏━┓┏━╸┏━┓┏━╸┏━╸┏┳┓╻┏ ┏━┓
 *  ╺╋╸   ┣━┫┃┃┃┃┏┛┣┻┓   ┗━┓┃ ┃┣┳┛┣╸ ┣━┫┃  ┣╸ ┃┃┃┣┻┓┏━┛
 *  ╹ ╹   ╹ ╹╹ ╹┗┛ ╹ ╹╺━╸┗━┛┗━┛╹┗╸╹  ╹ ╹┗━╸┗━╸╹ ╹╹ ╹┗━╸
 */
amVK_SurfaceMK2::amVK_SurfaceMK2(VkSurfaceKHR S, amVK_DeviceMK2 *D, bool uKnowWhatURDoing) : S(S), PD(nullptr) {
    if (D == nullptr) {
        if (!uKnowWhatURDoing) { amVK_LOG_EX("you passed in a amVK_DeviceMK2 variable which is nullptr.... [ _PD = nullptr ] has been set"); }
        // PD = nullptr;
    }
    else {
        PD = D->PD;
    }
}

/**
 *  \│/  ┌─┐┌─┐┌┬┐   ╔═╗┬ ┬┬─┐┌─┐┌─┐┌─┐┌─┐╔═╗┬ ┬┌─┐┌─┐┌─┐┬─┐┌┬┐
 *  ─ ─  │ ┬├┤  │    ╚═╗│ │├┬┘├┤ ├─┤│  ├┤ ╚═╗│ │├─┘├─┘│ │├┬┘ │ 
 *  /│\  └─┘└─┘ ┴────╚═╝└─┘┴└─└  ┴ ┴└─┘└─┘╚═╝└─┘┴  ┴  └─┘┴└─ ┴ 
 */
uint32_t amVK_SurfaceMK2::present_qFam(bool re) {
    // :re
    if (!re && this->present_qFamily != UINT32_T_NULL) {
        return this->present_qFamily;
    }

    int _id = HEART->SPD.index(PD);
    int lim = HEART->SPD.qFamily_lists[_id].n;

    VkBool32 sup = false;
    for (int i = 0; i < lim; i++) {
        vkGetPhysicalDeviceSurfaceSupportKHR(PD, i, S, &sup);
        if (sup == true) { present_qFamily = i; }
    }

    if (present_qFamily == UINT32_T_NULL) {amVK_LOG_EX("Couldn't Find any qFamily on PhysicalDevice that supports Presentation to given surface....");}
    return present_qFamily;
}

VkPhysicalDevice amVK_SurfaceMK2::select_DisplayDevice(void) {
    if (!HEART->SPD.list) {HEART->load_PD_info(false, true);}

    /** we are gonna choose a GPU that supports presenting to the surface.... */
    VkPhysicalDevice *PD_list = HEART->SPD.list;
             uint32_t PD_n    = HEART->SPD.n;

    /** The list is already sorted by BenchMarks....   [at least should be]   */
    for (int i = 0; i < PD_n; i++) {
        PD = PD_list[i];
        if (is_present_sup()) return PD;  /** calls \fn above, present_qFam() */
    }

    amVK_LOG_EX("Couldn't select DisplayDevice.... no VkPhysicalDevice seems to support presenting to required surface.... [return nullptr]");
    return nullptr;
}





/** 
    \│/  ┌─┐┌─┐┌┬┐   ╔═╗┬ ┬┬─┐┌─┐┌─┐┌─┐┌─┐╔═╗┌─┐┬─┐┌┬┐┌─┐┌┬┐┌─┐  
    ─ ─  │ ┬├┤  │    ╚═╗│ │├┬┘├┤ ├─┤│  ├┤ ╠╣ │ │├┬┘│││├─┤ │ └─┐  
    /│\  └─┘└─┘ ┴────╚═╝└─┘┴└─└  ┴ ┴└─┘└─┘╚  └─┘┴└─┴ ┴┴ ┴ ┴ └─┘  
 */
void amVK_SurfaceMK2::get_SurfaceFormats(bool print) {
    /** 
     * \todo move to readme file....
     * 
     * [its more like, WHAT Your Display's FORMAT & COLORSPACE support is + How your GPU wants it]
     * e.g. Your Display might support RGB 10-bit per channel.... But your GPU might want A2B10G10R10 instead of A2R10G10B10
     *   Maybe because of PERF or other under the Hood reasons.... But it really doesn't matter the ORDER of RGB
     *   cz If we (as the DRIVER Programmer) wanted to, we Could have impl. RGB or BGR without any Perf COST
     *   It's just that without giving whole bunch of ~5-20 formats [like AMD], NVIDIA kept it simple and LABELed it what they wanted
     *     (P.S. I think they Label it in BGR order.... cz Probably thats what the MEMORY RePresentation Under The HOOD Looks like)
     *     (I don't think it actually matters, cz.... the mysteries of the DRIVER is like the Mysteries of the HARDWARE-Engineering)
     * 
     * it's as Simple as it Goes.... These are just Some LABELs based on WHAT matches the closest from all the VkFormat enums
     * Don't OVER-THINK it.... just don't....
     * 
     *  These 2 Below are reported by hardware [DRIVER].... Are More like what your DISPLAY and DRIVER combination Supports 
     *  U won't need to take care these Formats EXPLICITLY: .... U just need to use SAME format to CREATE RenderPass ColorAttachment & FrameBuffer ImageView 
     *  The DRIVER Handles these IMPLICITLY:, you just need to let the DRIVER know what you are chosing now later on 
     */

    if (is_present_sup() == false) {
        amVK_LOG_EX("PD doesn't support presenting to VkSurfaceKHR S");
        return;
    }

    uint32_t n = 0;
    VkResult zz = vkGetPhysicalDeviceSurfaceFormatsKHR(PD, S, &n, nullptr);
    if (zz != VK_SUCCESS) {
        amVK_LOG_EX("vkGetPhysicalDeviceSurfaceFormatsKHR() error" << amVK_Utils::vulkan_result_msg(zz)); 
        return;
    }
    if (n == 0) {
        /** calling get_SurfaceFormats 2nd time gave me NULL values.... */
        /** we dont do error logging, cause its not a error */
        /**     - (nvidia, win10)  [smtime around 2021 Q2]  */
        amVK_LOG_EX("vkGetPhysicalDeviceSurfaceFormatsKHR(PD, S, &n, nullptr)....   [n == 0]");
        if (print && surface_formats.n) goto _print;
        return;
    }


    surface_formats.data = new VkSurfaceFormatKHR[n];
    vkGetPhysicalDeviceSurfaceFormatsKHR(PD, S, &n, surface_formats.data);
    surface_formats.n    = n;

    _print: {
        if (print) {
            _LOG_LOOP("------ LIST OF SUPPORTED PRESENTATION-IMAGE FORMATS ------", i, surface_formats.n, 
                "format: " << VkFormat_2_String(surface_formats.data[i].format) << ", colorSpace: " << VkColorSpace_2_String(surface_formats.data[i].colorSpace));
        }
    }
}


void amVK_SurfaceMK2::get_PresentModes(void) {
    if (is_present_sup() == false) {
        amVK_LOG_EX("PD doesn't support presenting to VkSurfaceKHR S");
        return;
    }

    uint32_t n = 0;
    VkResult zz = vkGetPhysicalDeviceSurfacePresentModesKHR(PD, S, &n, nullptr);       
    if (zz != VK_SUCCESS) {
        amVK_LOG_EX("vkGetPhysicalDeviceSurfacePresentModesKHR() error" << amVK_Utils::vulkan_result_msg(zz)); 
        return;
    }
    if (n == 0) {
        return;
    }

    present_modes.data = new VkPresentModeKHR[n];
    vkGetPhysicalDeviceSurfacePresentModesKHR(PD, S, &n, present_modes.data);
    present_modes.n    = n;
}





/** 
 *    \│/  ┌─┐┬┬ ┌┬┐┌─┐┬─┐    ╔═╗┬ ┬┬─┐┌─┐┌─┐┌─┐┌─┐╔═╗┌─┐┬─┐┌┬┐┌─┐┌┬┐
 *    ─ ─  ├┤ ││  │ ├┤ ├┬┘    ╚═╗│ │├┬┘├┤ ├─┤│  ├┤ ╠╣ │ │├┬┘│││├─┤ │ 
 *    /│\  └  ┴┴─┘┴ └─┘┴└─────╚═╝└─┘┴└─└  ┴ ┴└─┘└─┘╚  └─┘┴└─┴ ┴┴ ┴ ┴ 
 */
void amVK_SurfaceMK2::filter_SurfaceFormat(void) {
    bool found_it = false;
    bool found_r8g8b8a8 = false;
    bool found_b8g8r8a8 = false;

    this->get_SurfaceFormats(true);
    
    _LOG("  [searching...] imageFormat: " << VkFormat_2_String(final_imageFormat) << ", imageColorSpace: " << VkColorSpace_2_String(final_imageColorSpace));

    for (int i = 0; i < surface_formats.n; i++) {
        if ((surface_formats.data[i].format     == final_imageFormat) &&
            (surface_formats.data[i].colorSpace == final_imageColorSpace)) {
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
        amVK_LOG_EX("SurfaceFormat {" << VkFormat_2_String(final_imageFormat) << ", " << VkColorSpace_2_String(final_imageColorSpace) << "}.... " << "not supported." << amVK::endl);

             if (found_r8g8b8a8) {                final_imageFormat = VK_FORMAT_R8G8B8A8_SRGB; 
                amVK_LOG("USING \u2022" <<      VkFormat_2_String(VK_FORMAT_R8G8B8A8_SRGB)           << "\u2022 [ from amVK_SurfaceMK2::filter_SurfaceFormat() ]");}
        else if (found_b8g8r8a8) {                final_imageFormat = VK_FORMAT_B8G8R8A8_SRGB; 
                amVK_LOG("USING \u2022" <<      VkFormat_2_String(VK_FORMAT_B8G8R8A8_SRGB)           << "\u2022 [ from amVK_SurfaceMK2::filter_SurfaceFormat() ]");}

        final_imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; 
                amVK_LOG("USING \u2022" <<      VkColorSpace_2_String(VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)      << "\u2022 [ from amVK_SurfaceMK2::filter_SurfaceFormat() ]");
    }
    _filtered = true;
}








bool IMG_DATA_MK2::i_alloc(void) {  //malloc
    if (alloc_called) {
        amVK_LOG_EX("amVK_WI.IMGs.alloc_called == true;  seems like its already ALLOCATED!!!!       [we not allocating] ");
        return false;
    } 
    if (!framebuf_n || !attach_n) {
        amVK_LOG_EX("IMGs.attach_i == 0   or   IMGs.attach_n == 0      [Nothing to malloc for, set those two (& possibly other values too)]");
        return false;
    }

    uint32_t framebuf_size = (imageless_framebuffer) ? sizeof(VkFramebuffer) : sizeof(VkFramebuffer) * framebuf_n;
    uint32_t    clear_vals = attach_n * sizeof(VkClearValue);
    void *xd = calloc((framebuf_n * attach_n * 3) + framebuf_size + clear_vals, sizeof(VkImage));
    
    attachments = reinterpret_cast<  VkImageView *> (xd);
    images =      reinterpret_cast<      VkImage *> (attachments + (framebuf_n * attach_n));
    mem =         reinterpret_cast<VkDeviceMemory*> (     images + (framebuf_n * attach_n));
    framebufs =   reinterpret_cast<VkFramebuffer *> (        mem + (framebuf_n * attach_n));
    clearValues = reinterpret_cast<VkClearValue  *> (  framebufs + framebuf_size);

    alloc_called = true;
}








































/**
 *  ╻ ╻   ┏━┓┏┳┓╻ ╻╻┏    ╻ ╻╻   ┏┳┓╻┏ ┏━┓
 *  ╺╋╸   ┣━┫┃┃┃┃┏┛┣┻┓   ┃╻┃┃   ┃┃┃┣┻┓┏━┛
 *  ╹ ╹   ╹ ╹╹ ╹┗┛ ╹ ╹╺━╸┗┻┛╹╺━╸╹ ╹╹ ╹┗━╸
 */
amVK_WI_MK2::amVK_WI_MK2(const char *window, amVK_SurfaceMK2 *S, amVK_RenderPassMK2 *RP, amVK_DeviceMK2 *D) : m_windowName(window), amVK_D(D), amVK_S(S), amVK_RP(RP) {
         if (window == nullptr) { amVK_LOG_EX("Please dont pass nullptr, ERROR/WARNING Logs will look messy, this is used to let you know which window ");
                                  window = "nullptr";}
    if           (D == nullptr) { amVK_LOG_EX("[param]        amVK_DeviceMK2 *D      is nullptr.... name: " << window); }
    else if ( D->PD == nullptr) { amVK_LOG_EX("[param]        amVK_DeviceMK2 *D->_PD is nullptr.... name: " << window); }
         if (     S == nullptr) { amVK_LOG_EX("[param]       amVK_SurfaceMK2 *S      is nullptr.... name: " << window); }
         if (    RP == nullptr) { amVK_LOG_EX("[param]    amVK_RenderPassMK2 *RP     is nullptr.... name: " << window); }


    if (amVK_S->PD != amVK_D->PD) {
        amVK_LOG_EX("[param]       \u0027amVK_SurfaceMK2 *S->VkSurface _S\u0027, wasn't created on the same PhysicalDevice as _amVK_Device->_PD....  This will probably fail....");
    }
    amASSERT(!amVK_S->is_present_sup());

    _default_the_info();
    _LOG0("amVK_WI_MK2 Constructed   &   _default_the_info() called...");
}

void amVK_WI_MK2::_fallback_the_info(void) {
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
        VkSurfaceCapabilitiesKHR *surfaceCaps = amVK_S->get_SurfaceCaps();

        uint32_t target_img_cnt = the_info.minImageCount + surfaceCaps->minImageCount;
        if (target_img_cnt > surfaceCaps->maxImageCount) {
            if (the_info.minImageCount == surfaceCaps->maxImageCount) {  //This will be true only if the above if true
                amVK_LOG("You have requested " << the_info.minImageCount << " Images. Thats the max amount this surface's capability is.");
            }
            else if (the_info.minImageCount > surfaceCaps->maxImageCount) {
                amVK_LOG("You have requested " << the_info.minImageCount << " Images. surfaceCaps->maxImageCount = " << surfaceCaps->maxImageCount);
                amVK_LOG("We are capping it, to surfaceCaps->maxImageCount");
            }
            else if (the_info.minImageCount < surfaceCaps->maxImageCount) {
                amVK_LOG("You have requested " << the_info.minImageCount << " Images. surfaceCaps->maxImageCount = " << surfaceCaps->maxImageCount);
                amVK_LOG("you are safe!");
            }

            /** Finally set the_info.minImageCount.... \note \see Default_the_info */
            amVK_LOG("in create_Swapchain we do,    the_info.minImageCount += surfaceCaps->minImageCount;     [\see create_Swapchain]");
            amVK_LOG("So we are setting here        the_info.minImageCount  = surfaceCaps->maxImageCount - surfaceCaps->minImageCount");
            the_info.minImageCount = surfaceCaps->maxImageCount - surfaceCaps->minImageCount;
        }


     // ----------- SURFACE FORMAT SUPPORTED ------------
        amVK_S->filter_SurfaceFormat();
        the_info.imageFormat = amVK_S->final_imageFormat;
        the_info.imageColorSpace = amVK_S->final_imageColorSpace;
}


/**
    \│/  ┌─┐┬─┐┌─┐┌─┐┌┬┐┌─┐    ╔═╗┬ ┬┌─┐┌─┐┌─┐┬ ┬┌─┐┬┌┐┌
    ─ ─  │  ├┬┘├┤ ├─┤ │ ├┤     ╚═╗│││├─┤├─┘│  ├─┤├─┤││││
    /│\  └─┘┴└─└─┘┴ ┴ ┴ └─┘────╚═╝└┴┘┴ ┴┴  └─┘┴ ┴┴ ┴┴┘└┘
 */
VkSwapchainKHR amVK_WI_MK2::create_Swapchain(bool check_the_info) {
    if (this->swapchain || check_the_info) {
        _fallback_the_info();
    }

    /** maxImageExtent is not How much GPU can support, its more like HOW MUCH the SURFACE actually Needs  [even tho it says CAPABILITIES] */
    extent = amVK_S->get_SurfaceCaps()->minImageExtent;
    amVK_LOG("");
    amVK_LOG("Swapchain Extent: (" << extent.width << ", " << extent.height << ")");

    uint8_t imageCount_fix  = amVK_S->surface_caps.minImageCount; /** be safe & sound */
    the_info.minImageCount += imageCount_fix;
    n_IMGs = the_info.minImageCount;

    the_info.imageExtent    = {extent.width, extent.height};
    the_info.oldSwapchain   = swapchain;

    finally: {
        vkDeviceWaitIdle(amVK_D->D);          /** a lot of people says its better to just wait for the device to be idle, but why */
        VkResult res = vkCreateSwapchainKHR(amVK_D->D, &the_info, nullptr, &swapchain);
        if (res != VK_SUCCESS) {amVK_LOG_EX(amVK_Utils::vulkan_result_msg(res)); amVK_LOG("vkCreateSwapchainKHR() failed"); amASSERT(true); return nullptr;}
        
        if (the_info.oldSwapchain) {
            amVK_LOG("Swapchain ReCreated.");
        }
        else {
            amVK_LOG("Swapchain Created! Time TraveL....");
        }

        post_create_swapchain();
    }

    return swapchain;
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
        IMGs.attach_n = amVK_RP->m_attachment_descs.n;
        IMGs.color_index = amVK_RP->color_index;
        IMGs.swap_attach_index =  IMGs.color_index;   /** \todo swap_imgs could be used in other ways too */
        /** \todo    IMGs.imageless_framebuffer = false */
        IMGs.i_alloc();
        
        set_RenderPassClearVals();
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






/** 
 *   \│/  ╔═╗┬─┐┌─┐┌┬┐┌─┐╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐   ┬   ╔═╗┌┬┐┌┬┐┌─┐┌─┐┬ ┬┌┬┐┌─┐┌┐┌┌┬┐┌─┐
 *   ─ ─  ╠╣ ├┬┘├─┤│││├┤ ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘  ┌┼─  ╠═╣ │  │ ├─┤│  ├─┤│││├┤ │││ │ └─┐
 *   /│\  ╚  ┴└─┴ ┴┴ ┴└─┘╚═╝└─┘└  └  └─┘┴└─  └┘   ╩ ╩ ┴  ┴ ┴ ┴└─┘┴ ┴┴ ┴└─┘┘└┘ ┴ └─┘
 */
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
                imgCI.format = amVK_RP->m_attachment_descs[i].format;
                imgCI.usage  = image_layout_2_usageflags(amVK_RP->m_attachment_descs[i].finalLayout);
                viewCI.format = amVK_RP->m_attachment_descs[i].format;
                viewCI.subresourceRange.aspectMask = image_layout_2_aspectMask(amVK_RP->m_attachment_descs[i].finalLayout);

                ImageMK2 xd;
                xd.amvkCreateImage(&imgCI, &viewCI, amVK_D);   //Creates Image + View

                *(IMGs.i_ptr_attach(fi, i)) = xd.VIEW;
                *(IMGs.i_ptr_img   (fi, i)) = xd.IMG;
                *(IMGs.i_ptr_mem   (fi, i)) = xd.MEMORY;
                _LOG("Created attachment: IMGs.attachments[" << ((fi * IMGs.attach_n) + i) << "],  IMGs.images[" << ((i * IMGs.framebuf_n) + fi) << "]");
            }
            _LOG("");
        }
    }
}

void amVK_WI_MK2::set_RenderPassClearVals(void) {
    IMGs.clearValues[amVK_RP->color_index] = { 0.0f, 0.0f, 0.0f, 1.0f }; /** Color */

    if (amVK_RP->m_attachment_descs.n >= 2) {
      IMGs.clearValues[amVK_RP->depth_index] = { 1.f }; /** Depth */
    }
    /** More Attachment Support soon */

    rpInfo.clearValueCount = amVK_RP->m_attachment_descs.n;
    rpInfo.pClearValues = IMGs.clearValues;
  }


void amVK_WI_MK2::create_Framebuffers(void) {
    // ----------- \todo imageless_framebuffer & Separate create_Framebuffer_s------------

    VkFramebufferCreateInfo fb_info = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO, nullptr, 0,
        amVK_RP->RP, amVK_RP->m_attachment_descs.n, nullptr, /** [.pAttachments] set in the loop below */
        extent.width, extent.height, 1 /** [.layers] */
    };

    for (int fi = 0, lim = IMGs.framebuf_n; fi < lim; fi++) {
        fb_info.pAttachments = IMGs.i_ptr_attach(fi, 0);
        vkCreateFramebuffer(amVK_D->D, &fb_info, nullptr, &IMGs.framebufs[fi]);
    }
}


bool amVK_WI_MK2::CleanUp_Swapchain(void) {
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
      IMGs.i_free();

      _LOG0("Framebuffers, Attachments, Images, SwapchainImages has been Destroyed!");
      return true;
    }
    else {return false;}
}

void amVK_WI_MK2::Destroy_Swapchain(void) {
    if (swapchain) {
        CleanUp_Swapchain();
    }
    vkDestroySwapchainKHR(amVK_D->D, swapchain, nullptr);
    swapchain = nullptr;
    _LOG0("Swapchain has been Destroyed! BYE BYE!");
}













































uint32_t amVK_WI_MK2::AcquireNextImage(VkSemaphore to_signal, uint64_t timeout) {
    VkResult res = vkAcquireNextImageKHR(amVK_D->D, swapchain, timeout, to_signal, nullptr, &nExt_img);
    if (res != VK_SUCCESS) {
        amVK_LOG_EX("Couldn't AcquireNextImageKHR....");
        amVK_LOG_EX("");
        return UINT32_T_NULL;
    }
    return nExt_img;
}

/**
 * Why is this not in amVK_RenderPass? because.... vkCmdBeginRenderPass doesn't exactly mean what it seems at first
 *    the BeginInfo here ties to this window's FrameBuffer....  and thats it, the reason
 */
void amVK_WI_MK2::Begin_RenderPass(VkCommandBuffer cmdBuf, VkSubpassContents idk) {
    rpInfo.framebuffer = IMGs.framebufs[nExt_img];
    rpInfo.renderArea.extent = extent;
      
    vkCmdBeginRenderPass(cmdBuf, &rpInfo, idk);
}

bool amVK_WI_MK2::Present(VkSemaphore to_wait) {
    VkResult res;
    present_info.pWaitSemaphores = &to_wait;
    present_info.pResults = &res;
    vkQueuePresentKHR(amVK_D->get_graphics_queue(), &present_info);

    if (res != VK_SUCCESS) {
        amVK_LOG_EX("vkQueuePresentKHR() failed.... Serious bug....");
        amVK_LOG_EX("");
        return false;
    }
    return true;
}