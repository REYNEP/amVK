#pragma once
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
#ifdef amVK_BUILD_WIN32
  #define VK_USE_PLATFORM_WIN32_KHR //Doesn't Have any Usage Now yet, We use amVK_BUILD_WIN32 instead DIRECTLY, See amVK.cpp
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
#include "vulkan/vulkan.h"  //   Vulkan Includes "windows.h" on WIN     [? DEFINE  WIN32_LEAN_AND_MEAN ]
#include <cstring>          // strcmp() [Notincluded in the header]
#include <cstdlib>          // calloc() [Notincluded in the Header]
#include <vector>

#ifndef amVK_DEVICE_H       // Not a Dependency Cycle, cz amVK_Device.hh won't need ever need to include this file [TODO: Docs on DEPENDENCY GRAPH]
  #include "amVK_Device.hh" // vec_amVK_Device [parent: std::vector<amVK_Device *>]
#endif
#include "amVK_Utils.hh"    // Has mergeSort() for now & vulkan_result_msg()
#include "amVK_Types.hh"    // Has some DEFINES like SWAPCHAIN_IMAGE_FORMAT



/** 

     █████╗ ███╗   ███╗██╗   ██╗██╗  ██╗         ██████╗██╗  ██╗
    ██╔══██╗████╗ ████║██║   ██║██║ ██╔╝        ██╔════╝╚██╗██╔╝
    ███████║██╔████╔██║██║   ██║█████╔╝         ██║      ╚███╔╝ 
    ██╔══██║██║╚██╔╝██║╚██╗ ██╔╝██╔═██╗         ██║      ██╔██╗ 
    ██║  ██║██║ ╚═╝ ██║ ╚████╔╝ ██║  ██╗███████╗╚██████╗██╔╝ ██╗
    ╚═╝  ╚═╝╚═╝     ╚═╝  ╚═══╝  ╚═╝  ╚═╝╚══════╝ ╚═════╝╚═╝  ╚═╝

 * ---------------------- HIGH LIGHTS --------------------------
 * USE: CreateInstance, CreateDevice       mainly
 *      \see  enum_InstanceExts() & add_InstanceExts()    for enabling VULKAN EXTENSIONS
 * also \see    
 * 
 * RULE-I: we Dont do much Error Handling ourself cz of FAST & FURIOUS performance.... please do that yourselves, we return BOOL    [tho You can enable ValidationLayers    ( andWe do plan on making our own ValLayers) ]                
 * RULE-II: so, if you call same (enum_PhysicalDevs() or even benchMark_PD().... stuffs like that)     its gonna do force_load/calc again....
 * 
 * HEART: [static amVK_CX *heart]   THE One & Only HEART....   [cz Multi-instance isn't officially supported yet, tho you can \see CreateInstance impl. and use under the hood functions]
 */
class amVK_CX {
 public:
  static amVK_CX *heart;
  static VkInstance instance;
  static VkApplicationInfo vk_appInfo;


  /**
   *             ┬  ┬┬┌─╔═╗┬─┐┌─┐┌─┐┌┬┐┌─┐╦┌┐┌┌─┐┌┬┐┌─┐┌┐┌┌─┐┌─┐
   *        ───  └┐┌┘├┴┐║  ├┬┘├┤ ├─┤ │ ├┤ ║│││└─┐ │ ├─┤││││  ├┤   ───
   *              └┘ ┴ ┴╚═╝┴└─└─┘┴ ┴ ┴ └─┘╩┘└┘└─┘ ┴ ┴ ┴┘└┘└─┘└─┘   vkCreateInstance
   * |-------------------------------------------------------------------|
   * CAUTION: More than 1 VkInstance is NEVER needed! [ \see first if-else block of CreateInstance impl.]
   */
  VkInstance CreateInstance(void);

  bool check_VkSupport(void); /** \todo */
  /** \param appInfo: if nullptr, set custom info  [otherwise use appInfo to fill in]    --   Call before CreateInstance */
  void set_VkApplicationInfo(VkApplicationInfo *appInfo = nullptr);

  /**
   * \see _iep
   * 
   * \return false if   vkEnumerateInstanceExtensions() called but failed
   *         You should error Handle, and not Call any amVK functions anymore....
   * NOTE: [OUT] You don't have to MALLOC, &
   *       PLEASE: don't change any value in memory of [OUT], if you must, then copy it
   * \param pointer_iec: [OUT] Pointer to Number of Exts. Default: nullptr [see func definition] 
   * \param pointer_iep: [OUT] Pointer to [VkExtensionProperties *].... Hope you understand why :)
   */
  bool enum_InstanceExts(bool do_log = true, VkExtensionProperties **pointer_iep = nullptr, int *pointer_iec = nullptr);

  /**
   * NOTE: impl. is BruteForce   based on   iExtName_to_index()
   * \param extName: [that is gonna be enabled]    full list: [https://www.khronos.org/registry/vulkan/]
   * \return true if Added/AddedBefore. false if not Supported on current PC/Device/Computer/System whatever 
   *         [false and AddedBefore has LOG()]
   */
  bool add_InstanceExts(char *extName);

  /** 
   * Filter out the must Include platform (OS) specific SurfaceExts [Pretty easy code, you can take a look yourself ;)]
   * \return false if must needed [2] surface_exts are not available to system....   [_req_surface_exts wont have value]
   */
  bool filter_SurfaceExts(void);

  /** Well the name says it all. just check out vlayers_p var */
  void enum_ValLayers(void);
  /** Same as add_InstanceExts(), uses _enabled_valLayers var */
  bool add_ValLayer(char *vLayerName);
  





  /**
   *               ┬  ┬┬┌─╔═╗┬─┐┌─┐┌─┐┌┬┐┌─┐╔╦╗┌─┐┬  ┬┬┌─┐┌─┐
   *          ───  └┐┌┘├┴┐║  ├┬┘├┤ ├─┤ │ ├┤  ║║├┤ └┐┌┘││  ├┤   ───        
   *                └┘ ┴ ┴╚═╝┴└─└─┘┴ ┴ ┴ └─┘═╩╝└─┘ └┘ ┴└─┘└─┘       vkCreateDevice                                                              
   * |-------------------------------------------------------------------|
   * 
   * \return amVK_Device: ( \see amVK_Device.hh )   use amVK_Device._D   for vkDevice
   * 
   * \param preset: \see amVK_DevicePreset....   [its more like, when  \param CI is nullptr, we gotta use sm deafault options right....]
   * \param CI:  since this CI is like 90% stuffs that you would prolly like to MODIFY.... if making smth COOL & Fast.... we dont wanna hide this
   * 
   * \param init_vma:  [ext1] if true device->init_VMA() is called....   [Deprecated, \see amVK_Device::init_VMA & \see amVK_Device::CONSTRUCTOR]
   */
  amVK_Device *CreateDevice(amVK_TDevicePresetFlags preset, VkDeviceCreateInfo *CI = nullptr);
  
  loaded_PD_info_plus_plus    PD{};   /** \ref amVK_Types.hh  - Type not used anywhere else */
  vec_amVK_device _vkDevice_list{};
  /** [above ones] Was STATIC, needed to DEFINE it in amVK.cc....  thus amVK::CreateDevice() didnt work if used in GLOBAL Space of other modules outside main function */
  bool all_PD_inUse = false;
  /** \see PD.chozen */

  /**
   * sets into PD (member var)
   * \return true if already/successfully loaded.... false if enum_PhysicalDevs \returns false
   * \param force_load: pretty much sure you get this one ;)
   */
  bool load_PD_info(bool force_load);
  /**
   * sets into PD (member var)
   * \return false if vkEnumeratePhysicalDevices \returns 0 physical device....
   * \def force_load: by default, force_load even if already loaded....
   */
  bool enum_PhysicalDevs(void);   //Add support for UI kinda choosing
  bool enum_PD_qFamilies(void);   //only call after enum_PhysicalDevs()

  /** Mainly Based on VkPhysicalDeviceFeatures currently....    shaderStorageImageExtendedFormats is Prioratized the most */
  void benchMark_PD(void);

  /** \return false: if all gpu isUsed,  (PD_chozen is set to most powerfull/Strongest one.... in that case) */
  bool auto_choosePD(void);









  /** 
      ╻ ╻   ┏━╸┏━┓┏━┓┏┓ ╻╺┳┓╺┳┓┏━╸┏┓╻   ╻ ╻┏━┓┏━┓╻┏━┓┏┓ ╻  ┏━╸┏━┓
      ╺╋╸   ┣╸ ┃ ┃┣┳┛┣┻┓┃ ┃┃ ┃┃┣╸ ┃┗┫   ┃┏┛┣━┫┣┳┛┃┣━┫┣┻┓┃  ┣╸ ┗━┓
      ╹ ╹   ╹  ┗━┛╹┗╸┗━┛╹╺┻┛╺┻┛┗━╸╹ ╹   ┗┛ ╹ ╹╹┗╸╹╹ ╹┗━┛┗━╸┗━╸┗━┛
           & Quick-Hack Funcs
    * (Variables Declared by the Order of Appereance)
    * 
    * OLD-DOC: If someone has the Courage to Open and Read this file, then they surely will understand this soon or already does
    * IE: Denotes 'Instance Extensions'
    * PD:    +    'Physical Device'
    * P:     +    'Properties' eg. IEP: 'Instance Extension Properties', PDP: 'Physical Device Properties'
    * L:     +    'List' eg. PDL: 'Physical Device List
    * C:     +    'Count' eg. IEC: 'Instnace Extension Count' PDC: 'Physical Device Count'
    * I:     +    'Indexes' that we Need/Used/SpecifiedByUser, eg. last_used_qFamilyi 'Last Used qFamily Indices' [QFamiles are a THING of PDs]
    * N:     +    'Names' eg. IEN: 'Instance Extensions'
  */

  /**
   *             ╔═╗┬─┐┌─┐┌─┐┌┬┐┌─┐╦┌┐┌┌─┐┌┬┐┌─┐┌┐┌┌─┐┌─┐  ╦  ╦╔═╗╦═╗╔═╗
   *        ───  ║  ├┬┘├┤ ├─┤ │ ├┤ ║│││└─┐ │ ├─┤││││  ├┤   ╚╗╔╝╠═╣╠╦╝╚═╗  ───
   *             ╚═╝┴└─└─┘┴ ┴ ┴ └─┘╩┘└┘└─┘ ┴ ┴ ┴┘└┘└─┘└─┘   ╚╝ ╩ ╩╩╚═╚═╝    CreateInstance VARS
   */
  std::vector<VkExtensionProperties> _iep;             //'Instance Extension' Properties     [Available to this System]
  std::vector<VkExtensionProperties> _req_surface_ep;  // 'Surface Extension' Properties     [for now we know this will be 2 ; \see filter_SurfaceExts()]
  std::vector<char *> _iExts;                          //That the instance is gonna use   a.k.a ENABLED ones
  bool      *_isEnabled_iExt;

  uint32_t iExtName_to_index(char *iExtName) {
    for (uint32_t i = 0; i < _iep.size(); i++) {
      if (strcmp(iExtName, _iep[i].extensionName) == 0) {
        return i;
      }
    }
    return 0xFFFFFFFF;
  }
  uint32_t vLayerName_to_index(char *vLayerName) {
    for (uint32_t i = 0; i < _vLayers_p.size(); i++) {
      if (strcmp(vLayerName, _vLayers_p[i].layerName) == 0) {
        return i;
      }
    }
    return 0xFFFFFFFF;
  }


  //---------------- Validation Layers ----------------
#ifdef amVK_RELEASE
  const bool enableDebugLayers_LunarG = false;
#else
  const bool enableDebugLayers_LunarG = true;
#endif
  bool      *_isEnabled_vLayer;
  std::vector<char *> _vLayers = {"VK_LAYER_KHRONOS_validation"};
  std::vector<VkLayerProperties> _vLayers_p;




  /**
   *        ╔═╗┬─┐┌─┐┌─┐┌┬┐┌─┐╔╦╗┌─┐┬  ┬┬┌─┐┌─┐  ╦  ╦╔═╗╦═╗╔═╗     
   *   ───  ║  ├┬┘├┤ ├─┤ │ ├┤  ║║├┤ └┐┌┘││  ├┤   ╚╗╔╝╠═╣╠╦╝╚═╗  ───
   *        ╚═╝┴└─└─┘┴ ┴ ┴ └─┘═╩╝└─┘ └┘ ┴└─┘└─┘   ╚╝ ╩ ╩╩╚═╚═╝   CreateDevice VARS 
   */
  
  /** VkSurfaceKHR surface_2_support  [Deprecated VARIABLE] */

  //---------------- DeviceExts ----------------
  char  *req_dExt = "VK_KHR_swapchain";
  const char *const *dExts_alpha = &req_dExt;    //Enabled DeviceExts
  uint32_t dExtc_alpha = 1;
  amVK_Array<const char *const *> dExts_Beta;


  uint32_t PD_to_index(VkPhysicalDevice pd) {
    for (uint32_t i = 0; i < PD.n; i++) {
      if (pd == PD.list[i]) {
        return i;
      }
    }
    /** Code should never reach here unless \param pd is not available to system */
    return 0xFFFFFFFF;
  }

  inline const    VkQueueFamilyProperties *       get_PD_qFamilies(VkPhysicalDevice pd) {   return const_cast<VkQueueFamilyProperties *> (PD.qFamily_lists[PD_to_index(pd)].data);   }
};

#endif //#ifndef amVK_LIB