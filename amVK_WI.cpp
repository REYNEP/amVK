#include "amVK_WI.hh"
#include "amVK_Common.hh"
#include "amVK_ImgMemBuf.hh"


amVK_WI::amVK_WI(const char *window, amVK_SurfaceMK2 *S, amVK_RenderPassMK2 *RP, amVK_Device *D) : _window(window), _amVK_D(D), _surfaceExt(S), _surface(S->_S), _amVK_RP(RP) {
    if           (D == nullptr) { LOG_EX("param amVK_Device *D is nullptr...... " << "it has to be valid"); }
    else if (D->_PD == nullptr) { LOG_EX("param amVK_Device *D->_PD is nullptr.." << "it has to be valid"); }
         if (     S == nullptr) { LOG_EX("param S  is nullptr.......  " << "it has to be valid"); }
         if (    RP == nullptr) { LOG_EX("param RP (amVK_RenderPass) is nullptr.......  " << "it has to be valid"); }
         if (window == nullptr) { LOG_EX("Please dont pass nullptr, ERROR/WARNING Logs will look messy ");}

    /** [VUID-VkSwapchainCreateInfoKHR-surface-01270] - vkCreateSwapchain will fail     
     * Val Layer Error if    is_presenting_sup() not done per _surface    (a.k.a Basically per WINDOW) */
    amASSERT(!is_presenting_sup());
}

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

void amVK_SurfaceMK2::get_SurfaceFormats(void) {
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

        LOG_LOOP("------ LIST OF SUPPORTED PRESENTATION-IMAGE FORMATS ------", i, surface_formats.n, 
                "format: " << (uint32_t)surface_formats.data[i].format << ", colorSpace: " << (uint32_t)surface_formats.data[i].colorSpace);
    }
}


VkSwapchainKHR amVK_WI::createSwapchain(bool call_the_info_defaults) {
    VkSurfaceCapabilitiesKHR surfaceCaps;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_amVK_D->_PD, _surface, &surfaceCaps);
    /** maxImageExtent is not How much GPU can support, its more like HOW MUCH the SURFACE actually Needs  [even tho it says CAPABILITIES]*/
    _extent = surfaceCaps.minImageExtent;
        LOG("Swapchain Extent: (" << _extent.width << ", " << _extent.height << ")");


    if (_swapchain == nullptr) {    // A.k.a Creating Swapchain for first time
        // ----------- THE INFO - FirstTime ------------
        if (call_the_info_defaults) {
            the_info_defaults();
        }


        // ----------- PRESENT MODE SUPPORTED ------------
        _surfaceExt->get_PresentModes();
        amVK_Array<VkPresentModeKHR> present_modes = _surfaceExt->present_modes;
        bool found_immediate = false;
        for (int i = 0; i < present_modes.n; i++) {
            if (present_modes.data[i] == the_info.presentMode) {    //VK_PRESENT_MODE_IMMEDIATE_KHR in the_info_defaults
                found_immediate = true;
                break;
            }
        }
        if (!found_immediate) {the_info.presentMode = VK_PRESENT_MODE_FIFO_KHR; LOG("swapchain presentMode " << the_info.presentMode << " not available, using FIFO");}


        // ----------- SURFACE FORMAT SUPPORTED ------------
        bool found_it = false;  //We gonna try and make sure that RenderPass imageFormat and imageColorSpace is VALID for this surface....
        bool found_r8g8b8a8 = false;
        _surfaceExt->get_SurfaceFormats();
        amVK_Array<VkSurfaceFormatKHR> surface_formats = _surfaceExt->surface_formats;
        LOG("imageFormat: " << the_info.imageFormat << ", imageColorSpace: " << the_info.imageColorSpace << " N: " << surface_formats.n);
        //2nd time calling get_surfaceFormats || vkGetSurfaceFormats actually doesn't give anything on NVIDIA Driver (WINdows)
        for (int i = 0; i < surface_formats.n; i++) {
            if ((surface_formats.data[i].format == the_info.imageFormat)&&
                (surface_formats.data[i].colorSpace == the_info.imageColorSpace)) {
                found_it = true;
                break;
            }
            else if (surface_formats.data[i].format == VK_FORMAT_R8G8B8A8_SRGB) {
                found_r8g8b8a8 = true;
            }
        }
        if (!found_it) {
            LOG_EX("seems that .the_info.imageFormat & .the_info.imageColorSpace is set to something that ain't supported." << std::endl
                << "[Note that these 2 should also match with RenderPass's ImageFormat/ColorSpace....  so you need to configure using amVK_RenderPassMK2 stuffs]");
            if (found_r8g8b8a8) {LOG("Using \u2022" << VkFormat_Display_2_String(VK_FORMAT_R8G8B8A8_SRGB) << "\u2022 as the_info.imageFormat"); the_info.imageFormat = VK_FORMAT_R8G8B8A8_SRGB;}
            else                {LOG("Using \u2022" << VkFormat_Display_2_String(VK_FORMAT_B8G8R8A8_SRGB) << "\u2022 as the_info.imageFormat"); the_info.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;}
                                    LOG("Using \u2022" << VK_COLOR_SPACE_SRGB_NONLINEAR_KHR <<"\u2022 as the_info.imageColorSpace"); the_info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        }
        if (the_info.imageFormat != _amVK_RP->final_imageFormat) {
            LOG("amVK_WI.the_info.imageFormat != _amVK_RP->final_imageFormat" << std::endl << "will result in wrong colors" << "Window: " << _window);
        }
        if (the_info.imageColorSpace != _amVK_RP->final_imageColorSpace) {
            LOG("amVK_WI.the_info.imageColorSpace != _amVK_RP->final_imageColorSpace" << std::endl << "will result in wrong colors" << "Window: " << _window);
        }
        
        // ----------- IMAGE COUNT SUPPORTED ------------
        the_info.minImageCount = the_info.minImageCount + surfaceCaps.minImageCount;
        if (the_info.minImageCount > surfaceCaps.maxImageCount) {
            LOG("the_info.minImageCount:- " << the_info.minImageCount << " > " << surfaceCaps.maxImageCount << "  [surfaceCaps.maxImageCount]");
            LOG("you asked for " << the_info.minImageCount - surfaceCaps.minImageCount << " images   [why this happens? see amVK_WI::the_info_defaults()]");
            LOG("setting: " << "the_info.minImageCount = surfaceCaps.minImageCount");
            the_info.minImageCount = surfaceCaps.minImageCount;
        }
    }

    the_info.imageExtent            = {_extent.width, _extent.height};
    the_info.oldSwapchain           = _swapchain;

    // ----------- Actually Create the VkSwapchainKHR ------------
    finally_create_swapchain:
    {
        vkDeviceWaitIdle(_amVK_D->_D);      //I dont remember actually Why i put this here [LOG: 23 SEPT]
        VkResult res = vkCreateSwapchainKHR(_amVK_D->_D, &the_info, nullptr, &_swapchain);
        if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res)); LOG("vkCreateSwapchainKHR() failed"); amASSERT(true); return nullptr;}

        return _swapchain;
    }
}

/** More like EXTENDED FEATURES.... */
void amVK_WI::swapchain_CI_generic_mods(void) {
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
 * 
 * TODO: imageless_framebuffer support
 */
/** 
 * create the framebuffers for the swapchain images. 
 * This will connect the render-pass to the images for rendering
 * 
 * \param renderPass: Rendering is done in renderPasses.... So RenderPass uses FrameBuffer's Images
 */
void amVK_WI_MK2::create_framebufs(void) {
    if (!IMGs.alloc_called) {
        malloc_n_get_IMGs();
    } 
    else {uint32_t x = static_cast<uint32_t>(IMGs.n); vkGetSwapchainImagesKHR(_amVK_D->_D, _swapchain, &x, IMGs.ptr_swap_imgs()); create_imageviews();}

    if (*(IMGs.ptr_swap_attach(0)) == nullptr) {   //IMGs data was calloc-ed
        LOG("Creating Swapchain ImageViews....");
        create_imageviews();
    }

    // ----------- CREATE ATTACHMENT/IMGVIEWS FOR RENDERPASS ------------
    /** \see amVK_RenderPassMK2::color_index.... For now, we create Color Attachment anyway 
    for (int i = 0; i < color_index; i++) {
        createAttachment();
    }
    \todo let choose arrayLayers & midLevels & imageType * pnext
    */
    
    for (int i = _amVK_RP->color_index + 1; i < IMGs.attach_n; i++) {
        for (int x = 0; x < IMGs.n; x++) {
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
            IMGs.images[(i * IMGs.n) + x] = xd._img;
            LOG("Created attachment: IMGs.attachments[" << ((x * IMGs.attach_n) + i) << "],  IMGs.images[" << ((i * IMGs.n) + x) << "]");
        }
    }


    // ----------- \todo imageless_framebuffer ------------

    VkFramebufferCreateInfo fb_info = {};
        fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fb_info.pNext = nullptr;

        fb_info.renderPass = _amVK_RP->_RP;
        fb_info.attachmentCount = 2;
        fb_info.width = _extent.width;
        fb_info.height = _extent.height;
        fb_info.layers = 1;


    for (int i = 0, lim = IMGs.n; i < lim; i++) {
        fb_info.pAttachments = &IMGs.attachments[i * IMGs.attach_n];
        vkCreateFramebuffer(_amVK_D->_D, &fb_info, nullptr, &IMGs.framebufs[i]);
    }
}

/** \todo let modify options */
void amVK_WI_MK2::create_imageviews(void) {
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

    if (!IMGs.alloc_called) {LOG_EX("IMGs.alloc() wasn't called"); return;}
    for (uint32_t i = 0; i < IMGs.n; i++) {
        CI.image = ( IMGs.ptr_swap_imgs() )[i];

        VkResult res = vkCreateImageView(_amVK_D->_D, &CI, nullptr, IMGs.ptr_swap_attach(i));
        if (res != VK_SUCCESS) {amVK_Utils::vulkan_result_msg(res);}
        LOG("IMGs.attachments[" << (((uint64_t)IMGs.ptr_swap_attach(i) - (uint64_t)IMGs.ptr_swap_attach(0)) / (uint64_t)8) << "]");
    }
}





















/**
 * Those formats listed as sRGB-encoded have in-memory representations of R, G and B components which are nonlinearly-encoded as R', G', and B'; any alpha component is unchanged. 
 * As part of filtering, the nonlinear R', G', and B' values are converted to linear R, G, and B components; 
 * The conversion between linear and nonlinear encoding is performed as described in the “KHR_DF_TRANSFER_SRGB” section of the Khronos Data Format Specification.
 * 
 * Appendix C: Compressed Image Formats 1.2.198
 *      Chunked: https://vulkan.lunarg.com/doc/view/1.2.189.0/windows/chunked_spec/chap48.html
 */
char *VkFormat_Display_2_String(VkFormat F) {
  switch (F) {
    //100% on MAC & Android [100% from the ones that Support VULKAN]
    case VK_FORMAT_R8G8B8A8_SRGB:  return "VK_FORMAT_R8G8B8A8_SRGB";
    case VK_FORMAT_R8G8B8A8_UNORM: return "VK_FORMAT_R8G8B8A8_UNORM";
    //100% on Windows
    case VK_FORMAT_B8G8R8A8_SRGB:  return "VK_FORMAT_B8G8R8A8_SRGB";
    case VK_FORMAT_B8G8R8A8_UNORM: return "VK_FORMAT_R8G8B8A8_UNORM";

    case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
      return "VK_FORMAT_A8B8G8R8_SRGB_PACK32";
    case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
      return "VK_FORMAT_A8B8G8R8_UNORM_PACK32";

    /** 10Bit & 12Bit if any GPU & OS Combination supports it */
    case VK_FORMAT_A2R10G10B10_UNORM_PACK32: return "VK_FORMAT_A2R10G10B10_UNORM_PACK32";
    case VK_FORMAT_A2B10G10R10_UNORM_PACK32: return "VK_FORMAT_A2B10G10R10_UNORM_PACK32";
    case VK_FORMAT_R16G16B16A16_SFLOAT:      return "VK_FORMAT_R16G16B16A16_SFLOAT";
    case VK_FORMAT_R16G16B16A16_UNORM:       return "VK_FORMAT_R16G16B16A16_UNORM";

    // WEIRD-SHITS!
    case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
      return "VK_FORMAT_B10G11R11_UFLOAT_PACK32";

    // SHITS!
    case VK_FORMAT_B4G4R4A4_UNORM_PACK16:    return "VK_FORMAT_B4G4R4A4_UNORM_PACK16";
    case VK_FORMAT_R4G4B4A4_UNORM_PACK16:    return "VK_FORMAT_R4G4B4A4_UNORM_PACK16";
    case VK_FORMAT_R5G5B5A1_UNORM_PACK16:    return "VK_FORMAT_R5G5B5A1_UNORM_PACK16";
    case VK_FORMAT_B5G5R5A1_UNORM_PACK16:    return "VK_FORMAT_B5G5R5A1_UNORM_PACK16";

    case VK_FORMAT_R5G6B5_UNORM_PACK16:      return "VK_FORMAT_R5G6B5_UNORM_PACK16";   //Mobile has this one
    case VK_FORMAT_B5G6R5_UNORM_PACK16:      return "VK_FORMAT_B5G6R5_UNORM_PACK16";   //Mobile doesn't have this one

    default: 
      return "well, i guess the format you are thinking about must be really stellar....";
  }
}