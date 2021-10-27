#include "amVK_Types.hh"

/**
 * ONE: VkFormats that are for the DISPLAY.... or the so called PRESENTATION ENGINE
 * For now these are the known Ones I found from Nvidia or AMD surfaceCapabilities
 * http://www.vulkan.gpuinfo.org/listsurfaceformats.php
 */
char *VkFormat_2_String(VkFormat F) {
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
    case VK_FORMAT_B5G6R5_UNORM_PACK16:      return "VK_FORMAT_B5G6R5_UNORM_PACK16";   //Mobile doesn't have this 1

    default: 
      return "well, i guess the format you are thinking about must be really stellar....";
  }
}


/** \see amVK_ColorSpace in amVK_Types.hh */
char *VkColorSpace_2_String(VkColorSpaceKHR colorspace_name) {
  amVK_ColorSpace _colorspace = (amVK_ColorSpace) colorspace_name;
  switch (_colorspace) 
  {
    case amVK_sRGB:                       return "amVK_sRGB                         [      VK_COLOR_SPACE_SRGB_NONLINEAR_KHR   ]";

    case amVK_displayP3:                  return "amVK_displayP3                    [  VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT ]";
    case amVK_displayP3_LINEAR:           return "amVK_displayP3_LINEAR             [   VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT   ]";

    case amVK_bt709:                      return "amVK_bt709                        [     VK_COLOR_SPACE_BT709_NONLINEAR_EXT   ]";
    case amVK_bt709_LINEAR:               return "amVK_bt709_LINEAR                 [      VK_COLOR_SPACE_BT709_LINEAR_EXT     ]";

    // COOLEST Ones.... [Hope that ACES stuffs comes out in future too!]
    case amVK_DCI_P3:                     return "amVK_DCI_P3                       [    VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT   ]";

    // HDR ONES
    case amVK_bt2020_LINEAR:              return "amVK_bt2020_LINEAR                [      VK_COLOR_SPACE_BT2020_LINEAR_EXT    ]";
    case amVK_DOLBYVISION_DV:             return "amVK_DOLBYVISION_DV               [       VK_COLOR_SPACE_DOLBYVISION_EXT     ]";
    case amVK_HDR10_ST2084:               return "amVK_HDR10_ST2084                 [      VK_COLOR_SPACE_HDR10_ST2084_EXT     ]";
    case amVK_HDR10_HLG:                  return "amVK_HDR10_HLG                    [        VK_COLOR_SPACE_HDR10_HLG_EXT      ]";

    // SHIT-Theory ONES
    case amVK_scRGB:                      return "amVK_scRGB                        [VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT]";
    case amVK_scRGB_LINEAR:               return "amVK_scRGB_LINEAR                 [  VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT ]";
    case amVK_adobeRGB:                   return "amVK_adobeRGB                     [   VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT  ]";
    case amVK_adobeRGB_LINEAR:            return "amVK_adobeRGB_LINEAR              [     VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT   ]";

    // COOL!
    case amVK_COLOR_SPACE_PASS_THROUGH:   return "amVK_COLOR_SPACE_PASS_THROUGH     [       VK_COLOR_SPACE_PASS_THROUGH_EXT    ]";
    case amVK_DISPLAY_NATIVE_AMD:         return "amVK_DISPLAY_NATIVE_AMD           [      VK_COLOR_SPACE_DISPLAY_NATIVE_AMD   ]";
  }
}

char * amVKColorSpace_2_String(amVK_ColorSpace amVK_Colorspace_name) {
  return VkColorSpace_2_String( static_cast<VkColorSpaceKHR> (amVK_Colorspace_name) );
}