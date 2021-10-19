#ifndef amVK_COMMON_H
#define amVK_COMMON_H

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
#define VEC_amVK_DEVICE
#include "amVK_Utils.hh"
#include "amVK_Types.hh"

#define HEART amVK_IN::heart
#define HEART_CX static_cast<amVK_CX *>(HEART)
#define LOG_activeD_nullptr() LOG_EX("Either pass a valid amVK_Device (created with amVK_CX::CreateDevice/MK2) as param,    or see amVK_CX::activeD & amVK_CX::activate_device()")

/** Used in ALL [mostly] CLASS Constructors.... \see LOG in amVK_Logger.h, LOG_EX is prints an extra FIRST-LINE giving __func__, __line__*/
#define amVK_CHECK_DEVICE(PARAM_D, VAR_D) \
  if (PARAM_D && !HEART->D_list.doesExist(PARAM_D)) { \
    LOG_EX("\u0027param amVK_Device *" << #PARAM_D << "\u0027 doesn't exist in 'amVK_CX::heart->D_list'"); \
  } else { VAR_D = PARAM_D; }

#define amVK_SET_activeD(VAR_D) \
  if           (!HEART->activeD){ LOG_activeD_nullptr(); } \
  else { VAR_D = HEART->activeD; }

#include "vulkan/vulkan.h"
#ifndef amVK_HH
    class amVK_CX;
#endif
#ifndef amVK_DEVICE_H
    class amVK_Device;
#endif



/** Since amVK_CX has a lot of things, decided to separate these, cz are used in many amVK files.... */
class amVK_IN {
  public:
    static amVK_IN *heart;      
    static amVK_Device *activeD;
    static VkInstance instance;
    static VkApplicationInfo vk_appInfo;

    amVK_IN(void) {heart = this; LOG("amVK_IN::heart set! \n\n");} //called by amVK_CX CONSTRUCTOR
    ~amVK_IN() {}

    /** HEART->D_list.doesExist() used to check if Device exists or not */
    amVK_Utils::vec_amVK_Device         D_list{};
};

#endif //amVK_COMMON_H