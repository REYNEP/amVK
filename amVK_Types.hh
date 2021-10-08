#ifndef amVK_TYPES_H
#define amVK_TYPES_H
#ifndef amVK_LIB
  #include "vulkan/vulkan.h"
#endif



#define UINT32_T_NULL 0xFFFFFFFF
#define UINT32_T_FALSE 0xFFFFFFFF

//We didn't actually need this for small usage. but as I was introduced to qFamilies, Things started looking soo soo nasty and Out of Way.... just ahhhh disgusting as Hell
// [amVK_types.hh]
template<typename T> 
struct amVK_Array {
    T *data;    //Pointer to the first element [But we dont do/keep_track_of MALLOC, so DUH-Optimizations]
    uint32_t n;

    /** CONSTRUCTOR */
    amVK_Array(T *D, uint32_t N) : data(D), n(N) {if (D == nullptr || N == 0) {LOG_EX("ERROR");}}
    amVK_Array(void) {data = nullptr; n = 0;}

    uint32_t next_add_where = 0;
    inline void push_back(T item) {   //if you use like 'memcpy to .data' & also use push_back(), its not gonna work
#ifndef NDEBUG
      if (next_add_where >= n) {
        LOG_EX("amVK_Array FILLED!.... You need to MALLOC manually and set to .data     amVK_Array doesn't support that cz of Memory Optimizations");
        return;
      }
#endif

      data[next_add_where] = item;
      next_add_where++;
    }

    inline T& operator[](uint32_t index) {
      return data[index];
    }
    inline size_t size(void) {return static_cast<size_t>(n);}
};


//Report this bug to VSCODE, everything looks the same goddamn color
//Holds info about multiple PD
//Only used for amVK_CX::PD... and it was intended  (Type [should] not [be] used anywhere else)
typedef struct everything_PD__ {
  VkPhysicalDevice                 *list = nullptr;         //'Physical Devices List'       [will be sorted after sort_physical_devices() is called]
  VkPhysicalDeviceProperties      *props = nullptr;         //'Physical Devices Properties' [                        ++                            ]
  VkPhysicalDeviceFeatures     *features = nullptr;         //'Physical Devices Features'
  uint32_t                             n = 0;               //'Physical Devices Count'
  amVK_Array<VkQueueFamilyProperties> *qFamily_lists = nullptr;     //'One Physical Devices can have Multiple qFamilies

  /** Plus Plus Stuffs */
  bool                           *isUsed = nullptr;
  VkPhysicalDevice                chozen = nullptr;
  uint32_t                  chozen_index = 0;
  uint32_t                   *benchMarks = nullptr;
  uint32_t           *index_sortedByMark = nullptr;
} loaded_PD_info_plus_plus;

/** 
 * You can use it like    DevicePresets = amVK_DevicePreset_Graphics + amVK_DevicePreset_Compute   [We will create 1 GRAPHICS & 1 COMPUTE Queue]
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




/**
 *  ╻ ╻   ╻ ╻╺┳╸╻╻  
 *  ╺╋╸   ┃ ┃ ┃ ┃┃  
 *  ╹ ╹   ┗━┛ ╹ ╹┗━╸
 * I just wanted to access amVK_CX::_device_list with _device_list[(VkDevice)D]
 * NOTE: if it was std::vector<amVK_Device> then if sm1 changes a amVK_Device, that won't be change in the VECTOR ONE
 *       So we gotta store only the REference or ratherMore pointers
 * IMPL: \see amVK_Device.cpp
 */
#if defined(amVK_DEVICE_CPP) || defined(VEC_amVK_DEVICE)
  #include <vector>
  #ifndef amVK_DEVICE_H
    class amVK_Device;
  #endif

  class vec_amVK_Device : public std::vector<amVK_Device *> {
   public:
    vec_amVK_Device(uint32_t n) : std::vector<amVK_Device *> (n) {}
    vec_amVK_Device(void) : std::vector<amVK_Device *>() {}
    ~vec_amVK_Device() {}

    //amVK Stores a lot of different kinda data like this. Maybe enable a option to store these in HDD as cache
    amVK_Device *operator[](VkDevice D);
  };

  #ifdef IMPL_VEC_amVK_DEVICE
    #ifndef amVK_LOGGER
      #include "amVK_Logger.hh"
    #endif
    amVK_Device *vec_amVK_Device::operator[](VkDevice D) {
      amVK_Device **data = this->data();
      for (int i = 0, lim = this->size(); i < lim; i++) {
        if (data[i]->_D == D) {
          return data[i];
        }
      }
      LOG_EX("LogicalDevice doesn't Exist");
      amASSERT(true);
      return nullptr;
    }
  #endif //IMPL_VEC_amVK_DEVICE
#endif //amVK_DEVICE_CPP || VEC_amVK_DEVICE


/** SWAPCHAIN IMAGE FORMAT DOCS
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

#endif //#ifndef amVK_TYPES_H