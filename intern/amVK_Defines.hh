#pragma once
#ifndef amVK_DEFINES_HH
#define amVK_DEFINES_HH
/** Both pragma & include guards.... cz I might just decide to pub this as a GIST smday.... */

/** 
   ╻ ╻   ┏━┓┏━┓┏━╸┏━┓┏━┓┏━┓┏━╸┏━╸┏━┓┏━┓┏━┓┏━┓┏━┓
   ╺╋╸   ┣━┛┣┳┛┣╸ ┣━┛┣┳┛┃ ┃┃  ┣╸ ┗━┓┗━┓┃ ┃┣┳┛┗━┓
   ╹ ╹   ╹  ╹┗╸┗━╸╹  ╹┗╸┗━┛┗━╸┗━╸┗━┛┗━┛┗━┛╹┗╸┗━┛
 */

/** 
 * These are really fucking heavy.... if you just define VK_USE_PLATFORM_WIN32_KHR, it becomes a ~0.31Million lines of preprocessor file....
 *  and with the WIN32 specific definitions from 'achalpandey' it gets ot only ~0.16Million lines of stuffs
 *  but the 'vulkan/vulkan.h' is like only 20K lines using MSVC-BuildTools 2019
 *  
 * So we introduce this amVK_USE_PLATFORM_XXX_KHR
 * 
 * Any of these is usually not needed to be included anywhere, really....
 *  amGHOST, only defines it manually in its 'amGHOST_WindowWIN32.cpp'.... & is only needed for 'vkCreateWin32SurfaceKHR'
 *  - Speaking as of [2022, MAR 3]
 */
#ifdef amVK_USE_PLATFORM_XXX_KHR
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