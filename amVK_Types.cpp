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
    case VK_FORMAT_B5G6R5_UNORM_PACK16:      return "VK_FORMAT_B5G6R5_UNORM_PACK16";   //Mobile doesn't have this one

    default: 
      return "well, i guess the format you are thinking about must be really stellar....";
  }
}