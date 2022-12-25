#pragma once
#ifndef amVK_TYPES_H
#define amVK_TYPES_H

/** This file should be C-Compatible.... IG 🙄 */
#include "vulkan/vulkan.h"
/** \see amVK_IMPLs.cpp */

/** Types or Stucts that are Important or Cool but doesn't have any amVK_<Module>.hh dedicated for itself.... are here, so that they catch your eye!
 *      some may have a great amout to deal to the Graphics World.... e.g. ImageFormat, ColorSpace
 *      some may just be as simple as the UINT32_T_NULL preprocessor below 😉 */

/** Pretty Cool idea, huh? ... or is it a bad one? */
#define UINT32_T_NULL 0xFFFFFFFF
#define UINT32_T_FALSE 0xFFFFFFFF
#define amVK_VkFlags_NULL 0x00000000

/** 
 * TODO: Rename amVK_DP -> DP
 * TODO: You can use it like    DevicePresets = amVK_DevicePreset_Graphics + amVK_DevicePreset_Compute   [We will create 1 GRAPHICS & 1 COMPUTE Queue]
 * TODO: Detailed description on what Every option Does
 * TODO: GEN3 support Multiple Flag Mixing...   like even though this is names _flags.... this for now supports 1 flag at a TIME
 * NOTE: Only amVK_DP_GRAPHICS & amVK_DP_COMPUTE is supported for now
 */
typedef enum amVK_TDevicePreset_GEN2__ {
  amVK_DP_UNDEFINED = 0,

  /** Choose at least 1 from the 6 Below */
  amVK_DP_GRAPHICS = 1,
  amVK_DP_COMPUTE = 2, 
  amVK_DP_TRANSFER = 4,
  amVK_DP_SPARSE = 8,
  amVK_DP_VIDEO_DECODE = 16,
  amVK_DP_VIDEO_ENCODE = 32,
  /** Choose at least 1 from the 6 Above */
  amVK_DP_PROTECTED_MEM = 64,               //[https://renderdoc.org/vkspec_chunked/chap12.html#memory-protected-memory]

  /** amVK's Secret Formulaes */
  amVK_DP_3DEngine = 1 + 128,               //GeometryShader & TessellationShader & sampleRateShading   [Ones that are Available.... VkPhysicalDeviceFeatures]
  amVK_DP_Encode_Decode = 16 + 32 + 256,
  amVK_DP_Image_Shaders = 512,
  amVK_DP_Compositor = 1024,
  amVK_DP_RayTracing = 2048,                //2000 series RTX Cards
} amVK_DevicePresets;
typedef uint32_t amVK_DevicePreset_Flags;

/** amVK_DevicePreset_Flags to STRING  for print/Log purposes */
const char *flag_2_strName(amVK_DevicePreset_Flags flag);


typedef enum amVK_TShaderStage__ {
  Shader_Unknown = 0,   //as in can be still determined.... or should be at least ig
  Shader_Vertex,
  Shader_Fragment,
  Shader_Compute,

  Shader_Geometry,
  Shader_TessellationControl,
  Shader_TessellationEvaluation,
  Shader_TesC = Shader_TessellationControl,
  Shader_TesE = Shader_TessellationEvaluation,

  Shader_RT,
  Shader_RayTracing = Shader_RT
}amVK_ShaderStage;



/** 
   ╻ ╻   ╺┳┓┏━╸╻ ╻╻┏━╸┏━╸   ┏━╸╻ ╻╺┳╸┏━╸┏┓╻┏━┓╻┏━┓┏┓╻┏━┓
   ╺╋╸    ┃┃┣╸ ┃┏┛┃┃  ┣╸    ┣╸ ┏╋┛ ┃ ┣╸ ┃┗┫┗━┓┃┃ ┃┃┗┫┗━┓
   ╹ ╹   ╺┻┛┗━╸┗┛ ╹┗━╸┗━╸   ┗━╸╹ ╹ ╹ ┗━╸╹ ╹┗━┛╹┗━┛╹ ╹┗━┛
   a STRING-ARRAY in [amVK_Device.cpp] Corresponds to this, so only add to the bottom, and dont delete anything
*/
typedef struct amVK_BExtensions__ {
  bool VK_KHR_IMAGE_FORMAT_LIST = false;
  bool VK_KHR_IMAGELESS_FRAMEBUFFER = false;

  bool VK_KHR_MAINTENANCE2 = false;

  bool VK_KHR_PERFORMANCE_QUERY = false;

  bool VK_KHR_RAY_QUERY = false;
  bool VK_KHR_RAY_TRACING_PIPELINE = false;

  bool VK_KHR_SWAPCHAIN = false;

  bool VK_KHR_VIDEO_DECODE_QUEUE = false;
  bool VK_KHR_VIDEO_ENCODE_QUEUE = false;
  bool VK_KHR_VIDEO_QUEUE = false;

  bool VK_EXT_MEMORY_BUDGET = false;
} amVK_DeviceExtsBools;









/** 
  \│/  ╦┌┬┐┌─┐┌─┐┌─┐╔═╗┌─┐┬─┐┌┬┐┌─┐┌┬┐   ┬   ╔═╗┌─┐┬  ┌─┐┬─┐╔═╗┌─┐┌─┐┌─┐┌─┐
  ─ ─  ║│││├─┤│ ┬├┤ ╠╣ │ │├┬┘│││├─┤ │   ┌┼─  ║  │ ││  │ │├┬┘╚═╗├─┘├─┤│  ├┤ 
  /│\  ╩┴ ┴┴ ┴└─┘└─┘╚  └─┘┴└─┴ ┴┴ ┴ ┴   └┘   ╚═╝└─┘┴─┘└─┘┴└─╚═╝┴  ┴ ┴└─┘└─┘

 * SWAPCHAIN IMAGE FORMAT DOCS, also Surface Format [vkSurfaceFormatKHR] docs 
  hey, so, if anyone really knows smth about the whole VK_FORMAT_B8G8R8A8_SRGB and why not R8G8B8A8   [is reported by NVIDIA]
  I reall wish you share the intel/story with me ;) .... would really love to hear


  Well, If you are like struggling with this option. Think of it in this way, When you call vkGetPhysicalDeviceSurfaceFormatsKHR
  You are actually talking with NVIDIA GPU, about what it supports for display output....
  It's not something, like some image format like in Blender.... like that Filmic/sRGB/DCI-P3 output Settings.... nor it's like a Converter/Middle-Format
  It is the Final representation of the MEMORY that Display (or rathermore likely OS DisplayManager/Compositor) reads from
  BGR/RGB it doesn't actually matter.... as long as the BITS per channel Matches     (well, there ain'y any YUV monitor right? All monitor wants Plain RGB bitmaps)
  When creating RenderPass, colorAttachment format is where you say.... 'Hey, so this is the Format Requested by the GPU-DRIVER'
  At the end of the Day, VK_FORMAT_B8G8R8A8_SRGB is like a LABEL.... which matched the Best for what goes on UNDER-THE-HOOD inside the DRIVER
  Now you can obviously ask WHY? But the answer is not gonna be anything that will still hold After Years....
  It's Just that NVIDIA Decided to go with that Format.... maybe because its FASTER or maybe they just liked the IDEA of it....
  I mean, It's like How they wanted it to be DESIGNED.... Like I do Design my Own code.... e.g Using Classes to divide parts of API

  

  Why not RGB? [Seems like has got to do something with BYTE-ORDEr of Memory Little/Big ending https://gitlab.freedesktop.org/mesa/mesa/-/blob/main/src/vulkan/wsi/wsi_common_wayland.c] 
  This is like .... should be what your Monitor Supports.... But also not 
  CZ, even if the Monitor has RGB (instead of BGR reverse order Pixels) BGR might do some optimizations.... like in NVIDIA's Case
  Nvidia has Got VK_FORMAT_B8G8R8A8_SRGB and A2B10G10R10_UNORM_PACK32 [Both BGR]
  But AMD has got What NVIDA has Plus the Same thing in RGB order
  But Both Nvidia & AMD doesn't support more than 8bpc on linux as of now
  Amd does have VK_FORMAT_R8G8B8A8_SRGB tho, and NVIDIA doesn't
  [https://vulkan.gpuinfo.org/listreports.php?surfaceformat=A2B10G10R10_UNORM_PACK32] [So No, No fking 10-Bit Surface support on LINUX]

  Well if you like think about LITTLE-ENDIAN [eg. 0xAARRGGBB will be like 0xBB 0xGG 0xRR 0xAA in memory 0xBB is at address 97 if 0xAA is at 100]
  and then if you treat those bytes as UINT32 then, lets say 'x' is a pointer to 97 memory address, X's Value would be 0xBBGGRRAA
  and if you are like you wanna Send monitor uint32 instead of individual Bytes, then you are sending Monitor BGRA
  (and if you are like setting values in memory as 1BYTE, eg. 0xAA 0xRR 0xGG 0xBB 0xAA 0xRR.... ) then when read as uint32 thats BGRA
  Then you might need to like Tell the Monitor whats actually going on.... And Nvidia just Lets the USER [like me] know that We are actually using BGRA
  I really wonder what would happen if a Monitor like GIGABYTE m27 (which uses BGR PixelOrder) on a NVIDIA

  But people say like, VK_FORMAT_B8G8R8A8_SRGB doesn't actually Relate to BYTE-ORDERING or more vaguely LITTLE-Endian
  Go search like LINUX BGRA you would find some Threads/Ques

  So arguabbly / theoritically what speed-ups couuld actually happen.... could it be something like
  the monitor doesn't actually want an CHAR array pointing to ARGB in that order?
    or maybe the DISPLAY MANAGER/Compositor doesn't [eg. win32 DRM, mesa DRM, X-server]
  Like a PNG Uncompressed image would contain RGB in that order in memory....
    but the monitor could be like 'Hey I can't just read that... you gotta send be in some other order'
    then for every image we would have to Like convert at a Final stage or rather The GPU would have to READ in different ORDER before sending to DISPLAY
    that means EXTRA CALCULATIONs

1 fact is, we know the GPU at least sends BYTES as in Memory data BINARY Zeroes and Ones.... cz DisplayPort/HDMI specs are based on How much BYtes over (hz) frequency
I really wish to know what actually happens under the Hood when Some RGB (png-like in RGB order) allocated FrameBuffer is sent to the display
Or ratherMore what are the Advantages of maybe using BGR reverse order... (if there are any)


Instead of doing complicated stuffs, having so many fucking Functions around. Its just feels Better/Faster/Easier/Understandable-at-a-glance
      -- to use VK_FORMAT_B8G8R8A8_SRGB, cz mostly any hardware workflow has support for that


 Thank you.... really.... If you have come this Far, you are pretty cool
 I Actually feel Like I got way too much Deep into it.... 


   SCIENCE
Test BGR/RGB Monitor/Display Pixel-Order with stuffs like the PONG that cs50 Made [with anti Aliasing off....] + a Macro Camera
Also See the SLO-MO Guys video about Monitor, You will see that Monitor is Refreshed LEFT-RIGHT [can be flipped in some techs]
      -- But because of HorizonTal Sync features of monitors, a ROW is like read and SEnd to monitor at a Time to Display
      -- https://www.youtube.com/watch?v=3BJU2drrtCM
*/

//See The docs in amVK_Types.hh
#define amVK_SWAPCHAIN_IMAGE_FORMAT VK_FORMAT_B8G8R8A8_SRGB

//See The docs in amVK_Types.hh
#define amVK_SWAPCHAIN_IMAGE_COLORSPACE VK_COLOR_SPACE_SRGB_NONLINEAR_KHR


/** 
  \│/  ┌─┐┌┬┐╦  ╦╦╔═    ╔═╗┌─┐┬  ┌─┐┬─┐╔═╗┌─┐┌─┐┌─┐┌─┐
  ─ ─  ├─┤│││╚╗╔╝╠╩╗    ║  │ ││  │ │├┬┘╚═╗├─┘├─┤│  ├┤ 
  /│\  ┴ ┴┴ ┴ ╚╝ ╩ ╩ ───╚═╝└─┘┴─┘└─┘┴└─╚═╝┴  ┴ ┴└─┘└─┘
 * 
 * \note
 * [You dont need to Need it rn if you are really new to VULKAN and didn't come here to make a COMPOSITOR or color sensitive stuffs
 * just Collapse this DOC and the STRUCT....]
 * 
 * \page
 * This time the doc is OK: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkColorSpaceKHR.html
 * & https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#data-format
 * But again for smone who doesn't know shit about COLORSPACE already, its a SHIT
 * 
 * In older versions of this extension VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT was misnamed VK_COLOR_SPACE_DCI_P3_LINEAR_EXT.
 * 
 * VK_EXT_swapchain_colorspace - '_EXT'    
 * VK_AMD_display_native_hdr   - '_AMD'
 * 
 * I think, 
 * LINEAR EOTF can be different for colorspace with different WhitePoint / ColorGamut
 *    But still that would be a LINEAR SLOPE....
 * 
 * The first one is VK_COLOR_SPACE_SRGB_NONLINEAR_KHR  -  amVK_SRGB
 * If a GPU has Presentation Support, it will have Support for this TOO   - by me
 * 
 * in the VKSpecs page, DCI-P3 is marked as Name
 *    Red Primary {1.000, 0.000}, Green Primary {0.000, 1.000},  Blue Primary {0.000, 0.000}, White-point {0.3333, 0.3333}
 *
 * 
 * 
 * 
 * Now if you see the wiki, thats not DCI-P3 Primaries.... so whats happening here??? 
 *  The fact is all colorspaces are encoded in RGB.... while DCI-P3 is encoded in XYZ values....
 *    what that means is, in RGB encoded spaces, RGB(0, 0, 1.0f) is gonna be like    at {0.150, 0.060} in XYZ....
 *      so that {0.150, 0.060} is like a MAX blue it can be.  [in XYZ]
 *    just like that, RGB encoded ones simply have a BOUND, binded to them, which we see as COLOR-PRIMARIES....
 *      but for encoding we would have to write {0, 0, 1.0}.... right?
 *      if it was in XYZ, we would instead have to write {0.150, 0.060}, 
 * 
 *    but hmmm... 🤔, what are those values relative to.... those {0.150, 0.060}???
 *      yeees, right, those values, {0.150, 0.060} are relative to that gamut-graphs that you have seen... that CIE XYZ ColorSpace, 
 *        and the entire colors in that Graph fits in 🛆{(0,0), (1,0), (0,1)}
 *        so, simply, XYZ encoded values are BOUND by or rathermore Relative to that Graph-Coordinates.... 😅
 * 
 *  Here the main fact is WHAT's RELATIVE to WHAT?
 *    and all RGB ColorSpaces are relative to a smaller XYZ-triangle, & is encoded relative to that smaller Triangle inside that Graph
 *    but not DCI-P3, as its encoded in XYZ, its encoded values are relative to the GRAPH-Coordinates....
 *    Now that is why VulkanSpecs indicated by those Primary-Values....
 * 
 * 
 * 
 * 
 * RESC: to get you started
 * start here: https://www.youtube.com/watch?v=m9AT7H4GGrA & https://www.youtube.com/watch?v=4nYGJI0r2-0
 *    Check Sobotka's Files out for FILMIC Blender.... the OCIO ones.... OCIO is cool, it was developed by Sony Pictures ImageWorks,
 *    the First Blender Filmic: https://github.com/sobotka/filmic-blender
 *    This summerizes, sm difference stuffs: http://www.wesleyknapp.com/blog/hdr
 * 
 * Search for how the Camera does it, a bit about its history. get familiar with EOTF, OETF [Optical-Electronic Transfer Function]..... For better Dynamic range after Encoding at lower bits, the idea of OETF was created
 *      Its like, Our eyes are SENSITIVE to Darker Shades.... our eyes can tell slightest difference in color, but for SunLit Bright ares in Midday, its bit less precise.... the more the light, the less our eyes can differentiate
 *      . now to give more BITS to the darker low Luminosity ares, OETF is used, 
 * 
 * Then check these ones out:
 *    https://hg2dc.com/      [The hitchhiker's guide to digital colour]
 *    https://chrisbrejon.com/articles/ocio-display-transforms-and-misconceptions/  [OCIO, Display Transforms, Misconceptions - Chris Brejon]
 *    https://developer.nvidia.com/sites/default/files/akamai/gameworks/hdr/UHDColorForGames.pdf [ got from: https://stackoverflow.com/q/49082820]
 * 
 *    and if you would like to give sm JUICE to your HEAD: https://www.khronos.org/registry/DataFormat/specs/1.3/dataformat.1.3.pdf   --  Start at Chapter 12 or 5.8
 *    KarenTheDorf provided this: Vulkan Discord: https://discord.com/channels/427551838099996672/427951526967902218/896415963950485524
 * 
 * if you are really Interested in this Topic, ColorSpace.... 
 *    [Not everyone has to be, like you can just go ahead and tell the vk To use what the DRIVER Supports.... 
 *      or just USE amVK, let amVK dooo all the stuffs under the HOOD.... \todo SOON UI Integrations....]
 *    you have to co-operate with Other ColorScientists.... 🙂😉
 *    
 * But If you are confused about anything, hit me up.... or if you really wonder about SOBOTKA at this point, let me know, [I shouldn't be telling you to knock him, but you actually prolly can]
 */
typedef enum VkColorSpaceKHR_amVKPort {
  amVK_sRGB = 0,    /** no EOFT??? [My guess is, a SRGB EOFT.... cz i dont think the vkSpecs talks about a SOFTWARE LAYER of EOTF functions, it references to the DISPLAY's intended EOTF, i think]
                        But VK_FORMAT_B8G8R8A8_SRGB   kinda feels like....   was made for no SRGB EOTF */

  amVK_displayP3 = VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT,         //sRGB-like EOTF applied (should be) to this before present [like amVK_EXTENDED_SRGB]
  amVK_displayP3_LINEAR = VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT,     //   linear EOTF

  amVK_bt709 =        VK_COLOR_SPACE_BT709_NONLINEAR_EXT,           //SMPTE 170M EOTF
  amVK_bt709_LINEAR = VK_COLOR_SPACE_BT709_LINEAR_EXT,              //    linear EOTF

  // COOLEST Ones.... [Hope that ACES stuffs comes out in future too!]
  amVK_DCI_P3 = VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT,                /** DCI-P3 EOTF. Note that values in such an image are interpreted as XYZ encoded color data by the presentation engine.
                                                                        CZ This gamut it pretty DIfferent.... Check it out 😉 [Gamut is also in the DOC Spec page] */
  
  // HDR ONES
  amVK_bt2020_LINEAR = VK_COLOR_SPACE_BT2020_LINEAR_EXT,            // linear EOTF.
  amVK_DOLBYVISION_DV = VK_COLOR_SPACE_DOLBYVISION_EXT,             // [Dolby Vision (BT2020) ] SMPTE ST2084 EOTF.
  amVK_HDR10_ST2084 = VK_COLOR_SPACE_HDR10_ST2084_EXT,              // [ HDR10 (BT2020) ] SMPTE ST2084 Perceptual Quantizer (PQ) EOTF.
  amVK_HDR10_HLG = VK_COLOR_SPACE_HDR10_HLG_EXT,                    // [ HDR10 (BT2020) ] Hybrid Log Gamma (HLG) EOTF.

  // SHIT-Theory ONES
  amVK_scRGB = VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT,          //  sRGB EOTF  [Modified for ranged beyond 0-1, see the DataFormat specs 1.3]
  amVK_scRGB_LINEAR = VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT,      //Linear EOTF

  amVK_adobeRGB = VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT,            //    LINEAR EOTF
  amVK_adobeRGB_LINEAR = VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT,        // Gamma 2.2 EOTF.


  // COOL!
  amVK_COLOR_SPACE_PASS_THROUGH = VK_COLOR_SPACE_PASS_THROUGH_EXT,
  amVK_DISPLAY_NATIVE_AMD = VK_COLOR_SPACE_DISPLAY_NATIVE_AMD   /** display’s native color space. This matches the color space expectations of AMD’s FreeSync2 standard, for displays supporting it. 
                                                                    Wow This COOL, coming from AMD */
} amVK_ColorSpace;


const char *VkColorSpace_2_String(VkColorSpaceKHR colorspace_name);
const char *amVKColorSpace_2_String(amVK_ColorSpace amVK_Colorspace_name);
/**{
  return VkColorSpace_2_String( static_cast<VkColorSpaceKHR> (amVK_Colorspace_name) );
}*/
const char *VkFormat_2_String(VkFormat F);

typedef enum amVK_SurfaceFormat_BitsPerChannel {
  ERROR_BIT = 0,

  // WEIRD SHITS!
  BIT_4 = 4,    //VK_FORMAT_R4G4B4A4_UNORM_PACK16
  BIT_5 = 5,    //VK_FORMAT_R5G5B5A1_UNORM_PACK16, VK_FORMAT_R5G6B5_UNORM_PACK16
  BIT_11 = 11,  //VK_FORMAT_B10G11R11_UFLOAT_PACK32

  BIT_8 = 8,
  BIT_10 = 10,  //Not many Nvidia Supports this
  BIT_12 = 12,  //A lot of AMD Cards support this
  BIT_16 = 16   //I dont know of any such GPU
} amVK_SurfaceFormat_Bit;
typedef amVK_SurfaceFormat_Bit amVK_SurfaceFormat_BPC;

typedef enum amVK_SurfaceFormat_Channels_n_Order {
  RGB = 1,
  RGBA = 2,
  BGR = 3,
  BGRA = 4
} amVK_SurfaceFormat_Channel;

/** R8G8B8A8 = BIT_DEPTH_32 */
typedef enum amVK_SurfaceFormat_BitDepth_Total_AllChannels {
  BIT_DEPTH_16 = 16,
  BIT_DEPTH_24 = 24,
  BIT_DEPTH_32 = 32,
  BIT_DEPTH_48 = 48
} amVK_SurfaceFormat_BitDepth;

/** 
 * Those formats listed as sRGB-encoded have in-memory representations of R, G and B components which are nonlinearly-encoded as R', G', and B'; any alpha component is unchanged. 
 * As part of filtering, the nonlinear R', G', and B' values are converted to linear R, G, and B components; 
 * The conversion between linear and nonlinear encoding is performed as described in the “KHR_DF_TRANSFER_SRGB” section of the Khronos Data Format Specification.
 * 
 * Appendix C: Compressed Image Formats 1.2.198
 *      Chunked: https://vulkan.lunarg.com/doc/view/1.2.189.0/windows/chunked_spec/chap48.html
 * 
 * there will always be UNORM, and SNORM is just.... -_-   APPLE Doesn't report it back....
 * 
 * https://stackoverflow.com/a/59630187 
 */
typedef enum amVK_SurfaceFormat_Encoding {
  ENC_SRGB = 1,
  ENC_UNORM = 2,
  ENC_UINT = 3,
  ENC_SINT = 4,
  ENC_UFLOAT = 5,
  ENC_SFLOAT = 6,
  ENC_USCALED = 7,  // 1 would be enc as 1.0f, 255 would be 255.0f, 0 would be 0.0f
  ENC_SSCALED = 8   // singned version of above
} amVK_SurfaceFormat_Enc;

#endif //#ifndef amVK_TYPES_H