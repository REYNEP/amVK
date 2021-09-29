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
#include <sstream>          // std::stringstream
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
   * \param presets: \see amVK_TDevicePreset....  [its more like, when  \param CI is nullptr, we gotta use sm deafault options right....]
   *                 [you can mix multiple Presets]
   * \param CI:  since this CI is like 90% stuffs that you would prolly like to MODIFY.... if making smth COOL & Fast.... we dont wanna hide this
   *             make sure its like FILLED and VALIDATED.... cz if not nullptr, we wont modify at all
   * 
   * \param init_vma:  [ext1] if true device->init_VMA() is called....   [Deprecated, \see amVK_Device::init_VMA & \see amVK_Device::CONSTRUCTOR]
   */
  amVK_Device *CreateDevice(amVK_DevicePreset_Flags presets, VkDeviceCreateInfo *CI = nullptr);
  
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















/** 
   ╻ ╻   ┏━┓┏┳┓╻ ╻╻┏    ╺┳┓┏━╸╻ ╻╻┏━╸┏━╸┏┳┓┏━┓╺┳┓┏━┓
   ╺╋╸   ┣━┫┃┃┃┃┏┛┣┻┓    ┃┃┣╸ ┃┏┛┃┃  ┣╸ ┃┃┃┃ ┃ ┃┃┗━┓
   ╹ ╹   ╹ ╹╹ ╹┗┛ ╹ ╹╺━╸╺┻┛┗━╸┗┛ ╹┗━╸┗━╸╹ ╹┗━┛╺┻┛┗━┛

   Handles everything of VkDeviceCreateInfo.... QueueCreateInfos, DeviceExtensions & DeviceFeatures
 */

typedef struct amVK_BExtensions__ {
  bool VK_NONE_UNDEFINED = false;
  
  bool VK_KHR_SWAPCHAIN = false;
  /** amVK_DevicePreset_Encode_Decode */
  bool VK_KHR_VIDEO_DECODE_QUEUE = false;
  bool VK_KHR_VIDEO_ENCODE_QUEUE = false;
  bool VK_KHR_VIDEO_QUEUE = false;
  /** amVK_DevicePreset_Image_Shaders | amVK_DevicePreset_Compositor */
  bool VK_KHR_IMAGE_FORMAT_LISTS = false;
  /** bool VK_KHR_PERFORMANCE_QUERY = false; */
  /** amVK_DevicePreset_RayTracing */
  bool VK_KHR_RAY_QUERY = false;
  bool VK_KHR_RAY_TRACING_PIPELINE = false;
} amVK_DeviceExtensionsBools;

#ifdef amVK_LIB
static const char *amVK_DeviceExtensions[8] = {
  "VK_NONE_UNDEFINED",
  /** amVK_DevicePreset_Graphics */
  "VK_KHR_swapchain",
  /** amVK_DevicePreset_Encode_Decode */
  "VK_KHR_video_decode_queue",
  "VK_KHR_video_encode_queue",
  "VK_KHR_video_queue",
  /** amVK_DevicePreset_Image_Shaders | amVK_DevicePreset_Compositor */
  "VK_KHR_image_format_lists",
  /** VK_KHR_PERFORMANCE_QUERY", */
  /** amVK_DevicePreset_RayTracing */
  "VK_KHR_ray_query",
  "VK_KHR_ray_tracing_pipeline"
};
#endif



/** Currently This Provied so many LOGGINGs.... But No error Handling.... so if Any error Occurs like the DEVICE doesn't Support the needed Extension, most probably your program will crash */
class amVK_DeviceMods {
 public:
  amVK_DevicePreset_Flags _flags = amVK_DP_UNDEFINED;
  VkPhysicalDevice _PD = nullptr;

  /** CONSTRUCTOR */
  amVK_DeviceMods(amVK_DevicePreset_Flags DevicePreset, VkPhysicalDevice PD, bool DoEverything = true) : _flags(DevicePreset), _PD(PD) {if (DoEverything) do_everything();}
  /** INITIALIZE */
  void do_everything(void) {configure_n_malloc(); set_qCIs(); set_exts(); set_ftrs();}

  /** PRIVATE - Handle with Care !!!! */
  VkQueueFlags req_Queues = 0;
  amVK_Array<VkDeviceQueueCreateInfo> qCIs = {};
  float _qPRIORITIES = 0.0f;

  amVK_DeviceExtensionsBools req_exts = {};
  amVK_Array<char *> exts = {};   /** Pointers to amVK_DeviceExtensions is push_back -ed */

  VkPhysicalDeviceFeatures req_ftrs = {}; //Features
  amVK_Array<char *> vLyrs = {}; //Validation layers


  /** 
   * CONFIGURE
   */
  void configure_n_malloc(void) {
    // ----------- PreMod Settings [a.k.a Configurations] ------------
    configure_preMod_settings_based_on_presets:
    {
      if (_flags & amVK_DP_GRAPHICS) {
        req_Queues += VK_QUEUE_GRAPHICS_BIT;
        qCIs.n++;

        req_exts.VK_KHR_SWAPCHAIN = true;
        exts.n++;
      }
      if (_flags & amVK_DP_COMPUTE) {
        req_Queues += VK_QUEUE_COMPUTE_BIT;
        qCIs.n++;
      }
    }


    // ----------- Memory Allocation [MALLOC] ------------
    memory_allocation_malloc:
    {
      /* Mixed with Configuration above
      if (req_Queues & VK_QUEUE_GRAPHICS_BIT) { qCIs.n++; }
      if (req_Queues & VK_QUEUE_COMPUTE_BIT)  { qCIs.n++; } 
      */

      void *test = malloc(qCIs.n * sizeof(VkDeviceQueueCreateInfo)
                         +exts.n * sizeof(char *));
      qCIs.data = static_cast<VkDeviceQueueCreateInfo *> (test);
      exts.data = reinterpret_cast<char **> (qCIs.data + qCIs.n);
    }
  }


  /** 
    \│/  ┌─┐ ╔═╗╦┌─┐
    ─ ─  │─┼┐║  ║└─┐
    /│\  └─┘└╚═╝╩└─┘
   * 1 Queue per TYPE/PRESET [Graphics/Compute/Transfer/Sparse/ENC_DEC]   only ENC_DEC has 2 queue 
   */
  void set_qCIs(void) {
    uint32_t PD_index = amVK_CX::heart->PD_to_index(_PD);
    amVK_Array<VkQueueFamilyProperties> qFAM_list = amVK_CX::heart -> PD.qFamily_lists[PD_index];

    std::stringstream log_device_name;
    log_device_name << "Physical Device [" << _PD << "] :- \u0027" << amVK_CX::heart->PD.props[PD_index].deviceName << "\u0027 ";

    // ----------- Indexes graphicsQueueFamily, computeQueueFamily ------------
    uint32_t graphics_qFAM = 0, compute_qFAM = 0;
    struct dedicated_qFAM_T {
      uint32_t graphics = 0xFFFFFFFF;
      uint32_t  compute = 0xFFFFFFFF;
    } dedicated_qFAM;

    // ----------- FIND Device qFamily INFO & SUP ------------
    find_req_qFAMs:
    {
      for (int i = 0, lim = qFAM_list.size(); i < lim; i++) {
        // We try to find if any qFAM support ONLY DEDICATED qTYPE   [sc: https://www.reddit.com/r/vulkan/comments/bw47tg/comment/epvnikg/]
        VkQueueFlags qFLAGS = qFAM_list[i].queueFlags;
        
        if      (qFLAGS == VK_QUEUE_GRAPHICS_BIT) { dedicated_qFAM.graphics = i;} 
        else if (qFLAGS == VK_QUEUE_COMPUTE_BIT)  { dedicated_qFAM.compute  = i;}

        if (!graphics_qFAM) {    if (qFLAGS & VK_QUEUE_GRAPHICS_BIT) {graphics_qFAM = i;}   }
        if (!compute_qFAM)  {    if (qFLAGS & VK_QUEUE_COMPUTE_BIT)  { compute_qFAM = i;}   }
      }
      if (dedicated_qFAM.graphics != 0xFFFFFFFF) { graphics_qFAM = dedicated_qFAM.graphics; }
      if (dedicated_qFAM.compute != 0xFFFFFFFF)  {  compute_qFAM = dedicated_qFAM.compute;  }
    }


    // ----------- Main MODs ------------
    modifications:
    {
      if (req_Queues & VK_QUEUE_GRAPHICS_BIT) { //amVK_DevicePreset_Graphics
        if (!graphics_qFAM) {LOG_EX(log_device_name.str() << "doesn't support GRAPHICS Queues \n" << "[ext-info: Couldn't Find any such qFamily]");}
        qCIs.push_back({
          VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
          nullptr,
          0,        /** flags */
          graphics_qFAM,
          1,        /** queueCount */
          &_qPRIORITIES
        });
      }
      if (req_Queues & VK_QUEUE_COMPUTE_BIT) {  //amVK_DevicePreset_Compute
        if (!compute_qFAM) {LOG_EX(log_device_name.str() << "doesn't support COMPUTE Queues \n"  << "[ext-info: Couldn't Find any such qFamily]");}
        qCIs.push_back({
          VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
          nullptr,
          0,        /** flags */
          graphics_qFAM,
          1,        /** queueCount */
          &_qPRIORITIES
        });
      }
    }
  }


  /**
      \│/  ┌─┐─┐ ┬┌┬┐┌─┐
      ─ ─  ├┤ ┌┴┬┘ │ └─┐
      /│\  └─┘┴ └─ ┴ └─┘
   */
  void set_exts(void) {
    amVK_Array<VkExtensionProperties> sup_exts = {};
    vkEnumerateDeviceExtensionProperties(_PD, nullptr, &sup_exts.n, nullptr);
    sup_exts.data = new VkExtensionProperties[sup_exts.n];
    vkEnumerateDeviceExtensionProperties(_PD, nullptr, &sup_exts.n, sup_exts.data);

    // ----------- FIND Device Exts SUP ------------
    amVK_DeviceExtensionsBools isSup = {};
    find_sup_exts: 
    {
      uint32_t found_n = 0;

      for (int i = 0, lim = sup_exts.n; i < lim; i++) {
        if (found_n == exts.n) break;

        /** most of the time the first 6 CHARS will match, so.... */
        if (strcmp(&sup_exts.data[i].extensionName[6], "_swapchain")) {
          isSup.VK_KHR_SWAPCHAIN = true;
          found_n++;
        }
        /** else if ()
         * MORE SOON, For now only amVK_DevicePreset_Graphics */
      }
    }
    delete[] sup_exts.data;

    find_req_exts:
    {
      const bool *req_exts_p = reinterpret_cast<bool *> (&req_exts);
      const bool *sup_exts_p = reinterpret_cast<bool *> (&isSup);
      bool result_success = true;

      for (int i = 0; i < sizeof(req_exts); i++) {
        if (req_exts_p[i] && !sup_exts_p[i]) {
          LOG_EX("Device Extension: \u0027" << amVK_DeviceExtensions[i] << "\u0027 isn't supported.... But is needed"
                << std::endl << "[cz of passed amVK_TDevicePresetFlags param to amVK_CX::CreateDevice()]");
          result_success = false;
        }
      }
    }

    // ----------- Main MODs ------------
    modifications:
    {
      if (req_exts.VK_KHR_SWAPCHAIN) {
        exts.push_back(const_cast<char *>(amVK_DeviceExtensions[1]));
      }
    }
  }


  /** 
      \│/  ┌─┐┌─┐┌─┐┌┬┐┬ ┬┬─┐┌─┐┌─┐
      ─ ─  ├┤ ├┤ ├─┤ │ │ │├┬┘├┤ └─┐
      /│\  └  └─┘┴ ┴ ┴ └─┘┴└─└─┘└─┘
   * Set Device Features 
   */
  void set_ftrs(void) {
    uint32_t PD_index = amVK_CX::heart->PD_to_index(_PD);
    VkPhysicalDeviceFeatures sup_ftrs = amVK_CX::heart->PD.features[PD_index];

    // ----------- Main MODs ------------
    modifications:
    {
      if (_flags & amVK_DP_3DEngine) {
        if (sup_ftrs.geometryShader) req_ftrs.geometryShader = true;
        if (sup_ftrs.tessellationShader) req_ftrs.tessellationShader = true;
      }

      /** Sparse \todo */
      /** shaderStorageImageExtendedFormats \todo */
    }
  }
};

#endif //#ifndef amVK_LIB