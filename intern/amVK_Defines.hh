#ifndef amVK_DEFINES_HH
#define amVK_DEFINES_HH

/** 
   ╻ ╻   ┏━┓┏━┓┏━╸┏━┓┏━┓┏━┓┏━╸┏━╸┏━┓┏━┓┏━┓┏━┓┏━┓
   ╺╋╸   ┣━┛┣┳┛┣╸ ┣━┛┣┳┛┃ ┃┃  ┣╸ ┗━┓┗━┓┃ ┃┣┳┛┗━┓
   ╹ ╹   ╹  ╹┗╸┗━╸╹  ╹┗╸┗━┛┗━╸┗━╸┗━┛┗━┛┗━┛╹┗╸┗━┛
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


/** From: https://github.com/achalpandeyy/VolumeRenderer/blob/30996789d819dba59db683e8d996b03eee456fdd/Source/Core/Win32.h */
#ifdef amVK_BUILD_WIN32
    #define WIN32_LEAN_AND_MEAN
    #define NOGDICAPMASKS
    #define NOSYSMETRICS
    #define NOMENUS
    #define NOICONS
    #define NOSYSCOMMANDS
    #define NORASTEROPS
    #define OEMRESOURCE
    #define NOATOM
    #define NOCLIPBOARD
    #define NOCOLOR
    #define NOCTLMGR
    #define NODRAWTEXT
    #define NOKERNEL
    #define NONLS
    #define NOMEMMGR
    #define NOMETAFILE
    #define NOMINMAX
    #define NOOPENFILE
    #define NOSCROLL
    #define NOSERVICE
    #define NOSOUND
    #define NOTEXTMETRIC
    #define NOWH
    #define NOCOMM
    #define NOKANJI
    #define NOHELP
    #define NOPROFILER
    #define NODEFERWINDOWPOS
    #define NOMCX
    #define NORPC
    #define NOPROXYSTUB
    #define NOIMAGE
    #define NOTAPE

    #define STRICT
#endif

#endif //amVK_DEFINES_HH