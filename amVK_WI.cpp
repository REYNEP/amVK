#define amVK_WI_CPP
#include "amVK_WI.hh"
#include "amVK_Common.hh"
#include "amVK_ImgMemBuf.hh"

/** 
   ╻ ╻   ┏━╸┏━┓┏┓╻┏━┓╺┳╸┏━┓╻ ╻┏━╸╺┳╸┏━┓┏━┓
   ╺╋╸   ┃  ┃ ┃┃┗┫┗━┓ ┃ ┣┳┛┃ ┃┃   ┃ ┃ ┃┣┳┛
   ╹ ╹   ┗━╸┗━┛╹ ╹┗━┛ ╹ ╹┗╸┗━┛┗━╸ ╹ ┗━┛╹┗╸
 */
amVK_WI_MK2::amVK_WI_MK2(const char *window, amVK_SurfaceMK2 *S, amVK_RenderPassMK2 *RP, amVK_Device *D) : _window(window), _amVK_D(D), _amVK_S(S), _surface(S->_S), _amVK_RP(RP) {
         if (window == nullptr) { LOG_EX("Please dont pass nullptr, ERROR/WARNING Logs will look messy, this is used to let you know which window ");
                                  window = "nullptr";}
    if           (D == nullptr) { LOG_EX("[param]           amVK_Device *D      is nullptr.... name: " << window); }
    else if (D->_PD == nullptr) { LOG_EX("[param]           amVK_Device *D->_PD is nullptr.... name: " << window); }
         if (     S == nullptr) { LOG_EX("[param]       amVK_SurfaceMK2 *S      is nullptr.... name: " << window); }
         if (    RP == nullptr) { LOG_EX("[param]    amVK_RenderPassMK2 *RP     is nullptr.... name: " << window); }

    /** [VUID-VkSwapchainCreateInfoKHR-surface-01270] - vkCreateSwapchain will fail     
     * Val Layer Error if    is_presenting_sup() not done per _surface    (a.k.a Basically per WINDOW) */
    amASSERT(!is_presenting_sup());

    Default_the_info();
    fallback_the_info();
    LOG("amVK_WI_MK2 Constructed   &   Default_the_info() called...");
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
     *  The DRIVER Handles these IMPLICITLY:, you just need to let the DRIVER know what you are chosing now later on */

    amVK_Array<VkSurfaceFormatKHR> tmp_surface_formats = {};
    vkGetPhysicalDeviceSurfaceFormatsKHR(_PD, _S, &tmp_surface_formats.n, nullptr);              tmp_surface_formats.data = new VkSurfaceFormatKHR[tmp_surface_formats.n];
    vkGetPhysicalDeviceSurfaceFormatsKHR(_PD, _S, &tmp_surface_formats.n, tmp_surface_formats.data);

    /** calling get_SurfaceFormats 2nd time gives NULL values.... (nvidia, win10) */
    if (tmp_surface_formats.data != nullptr && tmp_surface_formats.n > 0) {
        surface_formats = tmp_surface_formats;

        if (print) {
            LOG_LOOP_MK1("------ LIST OF SUPPORTED PRESENTATION-IMAGE FORMATS ------", i, surface_formats.n, 
                "format: " << (uint32_t)surface_formats.data[i].format << ", colorSpace: " << (uint32_t)surface_formats.data[i].colorSpace);
        }
    }
}

/** 
 * other amVK_SurfaceMK2 Get stuffs....
 */
amVK_SurfaceCaps amVK_SurfaceMK2::get_SurfaceCapabilities_II(void) {
    VkSurfaceCapabilitiesKHR surfaceCaps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_PD, _S, &surfaceCaps);
    
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
    vkGetPhysicalDeviceSurfacePresentModesKHR(_PD, _S, &tmp_present_modes.n, nullptr);           tmp_present_modes.data = new VkPresentModeKHR[tmp_present_modes.n];
    vkGetPhysicalDeviceSurfacePresentModesKHR(_PD, _S, &tmp_present_modes.n, tmp_present_modes.data);

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
    
    LOG("  [searching...] imageFormat: " << what_to_filter.format << ", imageColorSpace: " << what_to_filter.colorSpace);
    //this->surface_formats
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
        LOG_EX("SurfaceFormat {" << VkFormat_2_String(what_to_filter.format) << ", " << VkColorSpace_2_String(what_to_filter.colorSpace) << "}.... " << "not supported." << std::endl
            << "[Note that these 2 should also match with RenderPass's ImageFormat/ColorSpace....  so you need to configure using amVK_RenderPassMK2 stuffs]");

             if (found_r8g8b8a8) {                what_to_filter.format = VK_FORMAT_R8G8B8A8_SRGB; 
                LOG("Returning \u2022" <<               VkFormat_2_String(VK_FORMAT_R8G8B8A8_SRGB)           << "\u2022 [ from amVK_SurfaceMK2::filter_SurfaceFormat() ]");}
        else if (found_b8g8r8a8) {                what_to_filter.format = VK_FORMAT_B8G8R8A8_SRGB; 
                LOG("Returning \u2022" <<               VkFormat_2_String(VK_FORMAT_B8G8R8A8_SRGB)           << "\u2022 [ from amVK_SurfaceMK2::filter_SurfaceFormat() ]");}

        what_to_filter.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; 
                LOG("Returning \u2022" <<      VkColorSpace_2_String(VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)      << "\u2022 [ from amVK_SurfaceMK2::filter_SurfaceFormat() ]");
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
        _amVK_S->get_PresentModes();
        amVK_Array<VkPresentModeKHR> present_modes = _amVK_S->present_modes;

        bool found_immediate = false;
        amVK_ARRAY_IS_ELEMENT(present_modes, found_immediate, the_info.presentMode);
        if (!found_immediate) {
            this->the_info.presentMode = VK_PRESENT_MODE_FIFO_KHR; 
            LOG("swapchain presentMode " << the_info.presentMode << " not available, using FIFO");
        }

    // ----------- IMAGE COUNT SUPPORTED ------------
        VkSurfaceCapabilitiesKHR surfaceCaps;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_amVK_D->_PD, _surface, &surfaceCaps);

        uint32_t target_img_cnt = the_info.minImageCount + surfaceCaps.minImageCount;
        if (target_img_cnt > surfaceCaps.maxImageCount) {
            if (the_info.minImageCount == surfaceCaps.maxImageCount) {  //This will be true only if the above if true
                LOG("You have requested " << the_info.minImageCount << " Images. Thats the max amount this surface's capability is.");
            }
            else if (the_info.minImageCount > surfaceCaps.maxImageCount) {
                LOG("You have requested " << the_info.minImageCount << " Images. surfaceCaps.maxImageCount = " << surfaceCaps.maxImageCount);
                LOG("We are capping it, to surfaceCaps.maxImageCount");
            }
            else if (the_info.minImageCount < surfaceCaps.maxImageCount) {
                LOG("You have requested " << the_info.minImageCount << " Images. surfaceCaps.maxImageCount = " << surfaceCaps.maxImageCount);
                LOG("you are safe!");
            }

            /** Finally set the_info.minImageCount.... \note \see Default_the_info */
            LOG("in create_Swapchain we do,    the_info.minImageCount += surfaceCaps.minImageCount;     [\see create_Swapchain]");
            LOG("So we are setting here        the_info.minImageCount  = surfaceCaps.maxImageCount - surfaceCaps.minImageCount");
            the_info.minImageCount = surfaceCaps.maxImageCount - surfaceCaps.minImageCount;
        }


     // ----------- SURFACE FORMAT SUPPORTED ------------
        VkSurfaceFormatKHR _final = _amVK_S->filter_SurfaceFormat({the_info.imageFormat, the_info.imageColorSpace});
        the_info.imageFormat = _final.format;
        the_info.imageColorSpace = _final.colorSpace;

        /** RenderPass filters too.... but why not make it future proof double check.... cz i dont feel like coming here and changing stuffs inside this function. */
        if (the_info.imageFormat != _amVK_RP->final_imageFormat) {
            LOG("amVK_WI_MK2.the_info.imageFormat != _amVK_RP->final_imageFormat" << std::endl << "will result in wrong colors" << "Window: " << _window);
        }
        if (the_info.imageColorSpace != _amVK_RP->final_imageColorSpace) {
            LOG("amVK_WI_MK2.the_info.imageColorSpace != _amVK_RP->final_imageColorSpace" << std::endl << "will result in wrong colors" << "Window: " << _window);
        }
}

VkSwapchainKHR amVK_WI_MK2::create_Swapchain(bool check_the_info) {
    if (check_the_info) {
        fallback_the_info();
    }

    VkSurfaceCapabilitiesKHR surfaceCaps;
    // ----------- get_SurfaceExtent() ------------
    get_SurfaceExtent: {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_amVK_D->_PD, _surface, &surfaceCaps);
        /** maxImageExtent is not How much GPU can support, its more like HOW MUCH the SURFACE actually Needs  [even tho it says CAPABILITIES] */
        _extent = surfaceCaps.minImageExtent;
        LOG("");
        LOG("Swapchain Extent: (" << _extent.width << ", " << _extent.height << ")");
    }

    uint8_t imageCount_fix = surfaceCaps.minImageCount; /** be safe & sound */
    the_info.minImageCount += imageCount_fix;   /** Decremented at func end */

    the_info.imageExtent            = {_extent.width, _extent.height};
    the_info.oldSwapchain           = _swapchain;

    finally: {
        vkDeviceWaitIdle(_amVK_D->_D);          /** a lot of people says its better to just wait for the device to be idle, but why */
        VkResult res = vkCreateSwapchainKHR(_amVK_D->_D, &the_info, nullptr, &_swapchain);
        if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res)); LOG("vkCreateSwapchainKHR() failed"); amASSERT(true); return nullptr;}
        
        if (the_info.oldSwapchain) {LOG("Swapchain ReCreated.");}
        else {LOG("Swapchain Created! Time TraveL....");}

        /** \todo what if someone suddenly turns off imageless_framebuffer or increases the_info.minImageCount */
        post_create_swapchain();
    }

    the_info.minImageCount -= imageCount_fix;   /** lets not make people confused after create_swapchain(); returns.... so we reset */
    return _swapchain;
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
        vkGetSwapchainImagesKHR(_amVK_D->_D, _swapchain, &swap_imgs_n, nullptr);
        LOG("Swapchain Images N: " << swap_imgs_n);
        
        IMGs.framebuf_n = (uint8_t) swap_imgs_n;
        IMGs.attach_n = _amVK_RP->attachment_descs.n;
        IMGs.color_index = _amVK_RP->color_index;
        IMGs.swap_attach_index =  IMGs.color_index;   /** \todo swap_imgs could be used in other ways too */
        /** \todo    IMGs.imageless_framebuffer = false */
        IMGs.alloc();
    }

    uint32_t smth = IMGs.framebuf_n;
    vkGetSwapchainImagesKHR(_amVK_D->_D, _swapchain, &smth, IMGs._ptr_img(0, IMGs.swap_attach_index));

    VkImageViewCreateInfo CI = {};
        CI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        CI.viewType = VK_IMAGE_VIEW_TYPE_2D;
    /** The Only Thing that you need to Care about when creating FRAMEBUFFER imageViews
     *    - Should match to RenderPass ColorAttachment.format ---- VUID-VkFramebufferCreateInfo-pAttachments-00880
     *    - Should match to imageFormat passed to SwapchainCreateInfo ---- VUID-VkImageViewCreateInfo-image-01762 */
        CI.format = the_info.imageFormat;

    /** describes a remapping from components of the image to components of the vector returned by shader image instructions. */
        CI.components = {};                       //All zeroes is fine for FRAMEBUFFER
        CI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    
    /** We don't need mipmap for frameBuffers IG   [1, cz VUID-VkImageCreateInfo-mipLevels-00947 mipLevels must be greater than 0] */
        CI.subresourceRange.baseMipLevel = 0;     //0 means the OG/base Image
        CI.subresourceRange.levelCount = 1;
        CI.subresourceRange.baseArrayLayer = 0;   //For StereoScope3D [also see VK_IMAGE_VIEW_TYPE_2D]
        CI.subresourceRange.layerCount = 1;

    for (uint32_t i = 0; i < IMGs.framebuf_n; i++) {
        CI.image = IMGs._ptr_img(0, IMGs.swap_attach_index)[i];

        VkResult res = vkCreateImageView(_amVK_D->_D, &CI, nullptr, IMGs._ptr_attach(i, IMGs.swap_attach_index));
        if (res != VK_SUCCESS) {amVK_Utils::vulkan_result_msg(res);}
        LOG("[Swapchain]_ IMGs.attachments[" << (((uint64_t)IMGs._ptr_attach(i, IMGs.swap_attach_index) - (uint64_t)IMGs._ptr_attach(0, IMGs.swap_attach_index)) / (uint64_t)8) << "]");
    }
    LOG("");
}




/** 
 * create the framebuffers for the swapchain images. 
 * This will connect the render-pass to the images for rendering
 * 
 * \param renderPass: Rendering is done in renderPasses.... So RenderPass uses FrameBuffer's Images
 */
void amVK_WI_MK2::create_Attachments_n_FrameBuf(void) {
    // ----------- CREATE ATTACHMENT/IMGVIEWS FOR RENDERPASS ------------
    for (int i = _amVK_RP->color_index + 1; i < IMGs.attach_n; i++) {
        for (int x = 0; x < IMGs.framebuf_n; x++) {
            VkImageCreateInfo dImage_CI = {};
                dImage_CI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                dImage_CI.pNext = nullptr;

                dImage_CI.imageType = VK_IMAGE_TYPE_2D;

            /** hardcoding the depth format to 32 bit float   You might want to choose other formats for other uses, or if you use Stencil buffer.*/
                dImage_CI.format = _amVK_RP->attachment_descs[i].format;
                dImage_CI.extent = {_extent.width, _extent.height, 1};

                dImage_CI.mipLevels = 1;
                dImage_CI.arrayLayers = 1;
                dImage_CI.samples = _amVK_RP->samples;
            /** Tiling is very important. Tiling describes how the data for the texture is arranged in the GPU. 
             *  For improved performance, GPUs do not store images as 2d arrays of pixels, 
             *  but instead use complex custom formats, unique to the GPU brand and even models. 
             *  VK_IMAGE_TILING_OPTIMAL tells vulkan to let the driver decide how the GPU arranges the memory of the image. 
             *  If you use VK_IMAGE_TILING_OPTIMAL, it won’t be possible to read the data from CPU or to write it without changing its tiling first 
             *  (it’s possible to change the tiling of a texture at any point, but this can be a costly operation). 
             *  The other tiling we can care about is VK_IMAGE_TILING_LINEAR, which will store the image as a 2d array of pixels. 
             *  While LINEAR tiling will be a lot slower, it will allow the cpu to safely write and read from that memory.   - vblanco20-1 */
                dImage_CI.tiling = VK_IMAGE_TILING_OPTIMAL;

                dImage_CI.usage = image_layout_2_usageflags(_amVK_RP->attachment_descs[i].finalLayout); //VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT

            imgBuf xd = new_image(_amVK_D, &dImage_CI);

            
            // TODO MERGE with IMAGEUTIL  [see wip stuffs in WI.cpp]
            //build a image-view for the depth image to use for rendering
            VkImageViewCreateInfo dview_info = {};
                dview_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                dview_info.pNext = nullptr;

            /** In here, we will have it matched to image_create_info, and hardcode it to 2D images as it’s the most common case. */
                dview_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
                dview_info.image = xd._img;
            /** format has to match the format in the image this view was created from.   
             *  It’s possible for the formats to not match, and that will let you “reinterpret” the formats, 
             *    but that can be tricky to use, and very niche, so for now make sure that the formats will match.  */
                dview_info.format = _amVK_RP->attachment_descs[i].format;;

            /** subresourceRange holds the information about where the image points to. 
             *  This is used for layered images, where you might have multiple layers in one image, and want to create an imageview that points to a specific layer. 
             *  It’s also possible to control the mipmap levels with it. 
             *  For our current usage, we are going to default it to no mipmaps (mipmap base 0, and mipmaps level 1), and only 1 texture layer. */
                dview_info.subresourceRange.baseMipLevel = 0;
                dview_info.subresourceRange.levelCount = 1;
                dview_info.subresourceRange.baseArrayLayer = 0;
                dview_info.subresourceRange.layerCount = 1;
            /** aspectMask is similar to the usageFlags from the image. It’s about what this image is used for. */
                dview_info.subresourceRange.aspectMask = image_layout_2_aspectMask(_amVK_RP->attachment_descs[i].finalLayout); //VK_IMAGE_ASPECT_DEPTH_BIT

            vkCreateImageView(_amVK_D->_D, &dview_info, nullptr, &xd._imgView);

            IMGs.attachments[(x * IMGs.attach_n) + i] = xd._imgView;
            IMGs.images[(i * IMGs.framebuf_n) + x] = xd._img;
            LOG("Created attachment: IMGs.attachments[" << ((x * IMGs.attach_n) + i) << "],  IMGs.images[" << ((i * IMGs.framebuf_n) + x) << "]");
        }
    }


    // ----------- \todo imageless_framebuffer & Separate create_Framebuffer_s------------

    VkFramebufferCreateInfo fb_info = {};
        fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fb_info.pNext = nullptr;

        fb_info.renderPass = _amVK_RP->_RP;
        fb_info.attachmentCount = 2;
        fb_info.width = _extent.width;
        fb_info.height = _extent.height;
        fb_info.layers = 1;


    for (int i = 0, lim = IMGs.framebuf_n; i < lim; i++) {
        fb_info.pAttachments = &IMGs.attachments[i * IMGs.attach_n];
        vkCreateFramebuffer(_amVK_D->_D, &fb_info, nullptr, &IMGs.framebufs[i]);
    }
}


bool amVK_WI_MK2::destroy(void) {
    if (_swapchain) {
      vkDeviceWaitIdle(_amVK_D->_D);
      // ----------- FrameBuffers ------------
      for (int i = 0; i < IMGs.framebuf_n; i++) {
        vkDestroyFramebuffer(_amVK_D->_D, IMGs.framebufs[i], nullptr);
      }
      // ----------- DepthAttachments ------------
      for (int i = _amVK_RP->color_index + 1; i < IMGs.attach_n; i++) {
        for (int x = 0; x < IMGs.framebuf_n; x++) {
          vkDestroyImageView(_amVK_D->_D, *( IMGs._ptr_attach(x, i) ), nullptr);
          vkDestroyImage(    _amVK_D->_D, *( IMGs._ptr_img   (x, i) ), nullptr);
        }   //GIT_COMMIT_FIX
      } //GIT_COMMIT_FIX
      // ----------- Swapchain Images' imageviews ------------
      for (int i = 0; i < IMGs.framebuf_n; i++) {
        vkDestroyImageView(_amVK_D->_D, *(IMGs._ptr_attach(i, IMGs.swap_attach_index)), nullptr);
      } //GIT_COMMIT_FIX
      // ----------- Finally Destroy Swapchain & Free -----------
      vkDestroySwapchainKHR(_amVK_D->_D, _swapchain, nullptr);
      IMGs._free();
    }   //GIT_COMMIT_FIX
    return true;
}   //GIT_COMMIT_FIT
