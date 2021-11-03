#ifndef amVK_LIB
#define amVK_LIB

/** 
   ╻ ╻   ┏━┓┏━┓┏━╸┏━┓┏━┓┏━┓┏━╸┏━╸┏━┓┏━┓┏━┓┏━┓┏━┓
   ╺╋╸   ┣━┛┣┳┛┣╸ ┣━┛┣┳┛┃ ┃┃  ┣╸ ┗━┓┗━┓┃ ┃┣┳┛┗━┓
   ╹ ╹   ╹  ╹┗╸┗━╸╹  ╹┗╸┗━┛┗━╸┗━╸┗━┛┗━┛┗━┛╹┗╸┗━┛
 * If you are searching on GITHUB, it Has a problem, Sometimes it won't show stuffs from BIG files when you search like vkCreateInstnace in trampoline.c
 * If U R using VSCode /w C/C++ Ext (for VSCode), BeAware it will cause 1 ERROR by not being able to find any of the Macros Below and then Reporting that #error (below #else) statement as an ERROR
 * Solve this with CMake-Tools For VSCode Ext, or add one of the Macros MANUALLY in the 'defines' list (C/C++ Ext SETTINGS, Just search 'defines' [UR 1stTime? then hit 'edit in settings.json']) 
 */
#ifdef amVK_BUILD_WIN32   /** \todo properly doc about this VK MACROS.... \see vulkan/vulkan.h amGHOST needs this cz of VkSurfaceKHR creation.... */
  #define VK_USE_PLATFORM_WIN32_KHR
#elif defined(amVK_BUILD_X11)
  #define VK_USE_PLATFORM_XCB_KHR
  #define VK_USE_PLATFORM_XLIB_KHR
#elif defined(amVK_BUILD_WAYLAND)
  #define VK_USE_PLATFORM_WAYLAND_KHR
#elif defined(amVK_BUILD_COCOA)
  #define VK_USE_PLATFORM_METAL_EXT
  #define VK_USE_PLATFORM_MACOS_MVK
#else 
  #error A amVK Build OS must be Specified [amVK_BUILD_WIN32/X11/WAYLAND/COCOA] by defining One of the Macros from inside amVK.hh .... use CMAKE for such
#endif
#include "vulkan/vulkan.h"  // Vulkan Includes "windows.h" on WIN     [? DEFINE  WIN32_LEAN_AND_MEAN  + possibly (https://github.com/achalpandeyy/VolumeRenderer/blob/30996789d819dba59db683e8d996b03eee456fdd/Source/Core/Win32.h)]

#include <cstdint>
#include <cstddef>


//Includes all the common & preprocessors for all amVK headers/files/modules
#define amVK_LOGGER_BLI_ASSERT  /** \todo there are other modules inside amVK_Logger.hh.... */
#include "amVK_Logger.hh"
#include "amVK_Utils.hh"
#include "amVK_Types.hh"



#ifndef amVK_DEVICE_HH
  class amVK_DeviceMK2;
#endif

#include <vector>
/**
 * I just wanted to access amVK_CX::_device_list with _device_list[(VkDevice)D]
 * IMPL: \see amVK_DeviceMK2.cpp
 * Declarations and Includes at Top
 */
class vec_amVK_Device : public std::vector<amVK_DeviceMK2 *> {
  public:
    vec_amVK_Device(uint32_t n) : std::vector<amVK_DeviceMK2 *> (n) {}
    vec_amVK_Device(void) : std::vector<amVK_DeviceMK2 *>() {}
    ~vec_amVK_Device() {}

    //amVK Stores a lot of different kinda data like this. Maybe enable a option to store these in HDD as cache
    amVK_DeviceMK2 *operator[](VkDevice D);
    bool doesExist(amVK_DeviceMK2 *amVK_D);  /** cz, VkDevice is inside amVK_DeviceMK2 class, that means the need to include amVK_DeviceMK2.hh in every single file*/
    uint32_t index(amVK_DeviceMK2 *D);
};



#define HEART amVK_IN::heart

/** Since amVK_CX has a lot of things, decided to separate these, cz are used in many amVK files.... 
 * only used as amVK_CX base/parent class & called as amVK_CX Constructor is called! */
class amVK_IN {
  public:
    static inline                        amVK_IN *heart = nullptr;  /** C++17 */
    static inline amVK_DeviceMK2               *activeD = nullptr;
    static inline VkInstance                   instance = nullptr;
    static inline VkApplicationInfo          vk_appInfo = {};
    static inline vec_amVK_Device                D_list = {};
    loaded_PD_info_plus_plus                         PD = {};   /** \ref amVK_Types.hh */


    virtual VkInstance CreateInstance(void) = 0;
    virtual bool DestroyInstance(void) = 0;

    virtual bool check_VkSupport(void) = 0; /** \todo */
    virtual void set_VkApplicationInfo(VkApplicationInfo *appInfo = nullptr) = 0;

    /**
     * sets into PD (member var)
     * \return true if already/successfully loaded.... false if enum_PhysicalDevs \returns false
     * \param force_load: pretty much sure you get this one ;)
     * \param auto_choose: does benchmark too, if not already done
     */
    virtual bool load_PD_info(bool force_load, bool auto_choose) = 0;

    /** used INTERNALLY in other amVK files */
    uint32_t PD_to_index(VkPhysicalDevice pd) {
      for (uint32_t i = 0; i < PD.n; i++) {
        if (pd == PD.list[i]) {return i;}
      }
      return 0xFFFFFFFF;  //NOT FOUND
    }

    inline const VkQueueFamilyProperties *get_qFamilies(VkPhysicalDevice pd) {return const_cast<VkQueueFamilyProperties *> (PD.qFamily_lists[PD_to_index(pd)].data);}


    /** Only way to set activeD, function not internally called.... */
    void activate_device(amVK_DeviceMK2 *D) {
      if (D_list.doesExist(D)) activeD = D;
    }

    amVK_IN(void) {heart = this; LOG("amVK_IN::heart set! \n");} //called by amVK_CX CONSTRUCTOR
    ~amVK_IN() {}
};


#define amVK_CHECK_DEVICE(PARAM_D, VAR_D) \
  if (PARAM_D && !HEART->D_list.doesExist(PARAM_D)) { \
    LOG_EX("\u0027param amVK_DeviceMK2 *" << #PARAM_D << "\u0027 doesn't exist in 'amVK_CX::heart->D_list'"); \
  } else { VAR_D = PARAM_D; }

#define amVK_SET_activeD(VAR_D) \
  if           (!HEART->activeD){ LOG_DBG("Either pass a valid amVK_DeviceMK2 as param" << \
                                         "\n  ...or see amVK_CX::activeD & amVK_CX::activate_device()  "); } \
  else { VAR_D = HEART->activeD; }

#endif //amVK_LIB