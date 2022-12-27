#pragma once

#include "amVK_INK.hh"




/** 
 * FORWARD DECLARATION.... cz of Depencency Loop [this file is included in amVK_Device.hh]
 * all other amVK Headers/Files include amVK_Device.hh if they have a `amVK_DeviceMK2` var
 */
#ifndef amVK_DEVICE_HH
  class amVK_DeviceMK2;
#endif
/**
 * I just wanted to access amVK_IN::D_list with D_list[(VkDevice)D]
 * IMPL: \see amVK_Device.cpp
 */
class vec_amVK_Device : public amVK_ArrayDYN<amVK_DeviceMK2 *> {
  public:
    vec_amVK_Device(uint32_t n) : amVK_ArrayDYN<amVK_DeviceMK2 *> (n) {}
    vec_amVK_Device(void) : amVK_ArrayDYN<amVK_DeviceMK2 *>(1) {}
    ~vec_amVK_Device() {}

    amVK_DeviceMK2 *operator[](VkDevice D);
    bool doesExist(amVK_DeviceMK2 *amVK_D);
    bool erase(uint32_t nth);
    uint32_t index(amVK_DeviceMK2 *D);
};



/** 
 * all PhysicalDevice Related information
 * LOAD: \see amVK_InstanceMK2::load_PD_info()
 * 
 * Per PhysicalDevice, its gonna be around ~1600Bytes
 */
struct loaded_PD_info_internal {
  VkPhysicalDevice                 *list = nullptr;         //'Physical Devices List'
  VkPhysicalDeviceProperties      *props = nullptr;         //'Physical Devices Properties'
  VkPhysicalDeviceFeatures     *features = nullptr;         //'Physical Devices Features'
  uint32_t                             n = 0;               //'Physical Devices Count'

  amVK_Array<VkQueueFamilyProperties> *qFamily_lists = nullptr;     // One Physical Devices can have Multiple qFamilies
  VkPhysicalDeviceMemoryProperties        *mem_props = nullptr;     // Used in ImagesMK2

  /** Plus Plus Stuffs */
  bool                           *isUsed = nullptr;
  VkPhysicalDevice                chozen = nullptr;
  uint32_t                  chozen_index = 0;
  uint32_t                   *benchMarks = nullptr;
  uint32_t           *index_sortedByMark = nullptr;

  inline uint32_t index(VkPhysicalDevice pd) {
    for (uint32_t i = 0; i < n; i++) {
      if (pd == list[i]) {return i;}
    }
    return UINT32_T_NULL;  //NOT FOUND
  }

  inline const VkQueueFamilyProperties *get_qFamilies(VkPhysicalDevice pd) {return const_cast<VkQueueFamilyProperties *> (qFamily_lists[index(pd)].data);}
};



struct instance_creation_settings_internal {
  /** instance Extension */
  amVK_Array<VkExtensionProperties>             IEP = {nullptr, 0};                 //'Instance Extension' Properties     [Available to this System]
  amVK_Array<bool>                   isEnabled_iExt = {nullptr, 0};
  VkExtensionProperties             _req_surface_ep[2];
  amVK_Array<VkExtensionProperties>  req_surface_ep = {_req_surface_ep, 2, 2};      // 'Surface Extension' Properties     [for now we know this will be 2; \see filter_SurfaceExts(), allocated there]
  amVK_ArrayDYN<char *>               enabled_iExts = amVK_ArrayDYN<char *>(16);    // USE add_ValLayer

  /** Vulkan Layers */
  amVK_Array<VkLayerProperties>             vLayerP = {nullptr, 0};
  amVK_Array<bool>                 isEnabled_vLayer = {nullptr, 0};
  amVK_ArrayDYN<char *>             enabled_vLayers = amVK_ArrayDYN<char *>(8);     // WELP, the default one.



  #ifdef amVK_RELEASE
    const bool enableDebugLayers_LunarG = false;
  #else
    const bool enableDebugLayers_LunarG = true;   /** by default adds VK_LAYER_KHRONOS_validation*/

    /** lets you use 'debugPrintfEXT' function inside shaders.... check out: https://anki3d.org/debugprintf-vulkan/ */
    /** NOTE: You'd also need to call \fn amVK_InstanceMK2::set_debug_printf_callback() *
     *       & ENABLE VK_KHR_shader_non_semantic_info    [Device Extension] */
    VkValidationFeatureEnableEXT GLSL_debug_printf_EXT = VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT;   /** TODO: Better way to enable...*/

    VkDebugReportCallbackEXT s_DebugKonsument1 = nullptr;
  #endif
};

















/**
 * ═════════════════════════════════════════════════════════════════════
 *                         - amVK_InstanceMK2 -
 * ═══════════════════════════ HIGH LIGHTS ═════════════════════════════
 * 
 * - VkInstance Creation/Destruction and all Instance related stuffs
 * - VkPhysicalDevice related stuffs
 * - an App, one VkInstance
 * 
 * - FLOW:
 *    - amVK_InstanceMK2 amVK_I = amVK_InstanceMK2();
 *    - amVK_I.add_InstanceExt();    // or stuffs like that
 *    - amVK_I.Create_VkInstance();
 *    - amVK_I.load_PD_info();       // you dont gotta care, called internally if you create `amVK_DeviceMK2`
 * 
 *  List of InstanceExtensions: https://vulkan.gpuinfo.org/listinstanceextensions.php
 * 
 *   TODO: Add support for UI kinda choosing/showing
 */
class amVK_InstanceMK2 {
 public:
  static inline                 amVK_InstanceMK2 *heart = nullptr;  /** C++17 - static inline */
  static inline amVK_DeviceMK2               *s_ActiveD = nullptr;
  static inline VkInstance                   s_Instance = nullptr;
  static inline VkApplicationInfo           s_VkAppInfo = {
    VK_STRUCTURE_TYPE_APPLICATION_INFO, nullptr,
    "xD_MadeWith_amVK", VK_MAKE_VERSION(0, 0, 1),     /** AppName, Version */
                "amVK", VK_MAKE_VERSION(0, 0, 4),     /** EngineName, Ver. */ /** Also see   VK_MAKE_API_VERSION    */
                        VK_MAKE_VERSION(1, 3, 0)      /** Vulkan(SDK) Ver. */ /** If 1.3 U can't call 1.4 stuff.... but can run app. in v1.0, if device/sys. only sup. upto that (dont call EXTs / v1.1+ stuff) */
  };
  static inline VkInstanceCreateInfo           the_info = {
    VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, nullptr, 0,
    &s_VkAppInfo,
    0, nullptr,                                       /**   EnabledLayers  */
    0, nullptr                                        /**    EnabledExts.  */
  };
  vec_amVK_Device                          s_DeviceList = {};       /**  not a static one :D  */
  loaded_PD_info_internal                           SPD = {};       /** call \fn load_PD_info() before, always used as 'HEART->PD' */
  instance_creation_settings_internal               ICS = {};



 public:
  /**
   * vkCreateInstance
   * call \fn add_InstanceExt() before this
   */
  VkInstance Create_VkInstance(void);
       bool Destroy_VkInstance(void);

  /** 
   * WORKS: if amVK_RELEASE isn't defined
   * \param DebugKonsument: a CallBack function (yours) 
   */
  VkDebugReportCallbackEXT Set_DebugKonsument(PFN_vkDebugReportCallbackEXT DebugKonsument = nullptr);
  /**
   *  \see ICS.GLSL_debug_printf_EXT
   *  You should set VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT
   */
  void Set_ValFeatures(VkValidationFeaturesEXT valFeatures);

  amVK_InstanceMK2(bool debug_EXTs = true) {heart = this; constructor_default_settings(debug_EXTs);}
  ~amVK_InstanceMK2() {}













  /** \todo I Dont think there will be any cases where vulkan might not be supported 😜*/
  static bool is_VkSupported(void) { 
    return true; 
  };                  

  /** \param appVersion: VK_MAKE_VERSION(1, 3, 0) */
  inline void set_AppInfo(const char *appName, uint32_t appVersion) {
    s_VkAppInfo.pApplicationName   = appName;
    s_VkAppInfo.applicationVersion = appVersion;
  }

 protected:
  amVK_InstanceMK2(const amVK_InstanceMK2&) = delete;             // Brendan's Solution
  amVK_InstanceMK2& operator=(const amVK_InstanceMK2&) = delete;  // Brendan's Solution









 public:
  /**
   * NOTE: impl. is BruteForce   based on   iExtName_to_index()
   * \return true if Added/AddedBefore. false if not Supported on current PC/Device/Computer/System whatever 
   *         [false and AddedBefore has LOG()]
   * 
   * \note calls: \fn enum_InstanceExts() if IEP.data == nullptr;
   */
  bool       add_InstanceExt(const char *extName);
  bool isEnabled_InstanceExt(const char *extName) {return ICS.isEnabled_iExt[iExtName_to_index(extName)];}

  bool       add_ValLayer(const char *vLayerName);
  bool isEnabled_ValLayer(const char *vLayerName) {return ICS.isEnabled_vLayer[vLayerName_to_index(vLayerName)];}

  amVK_ArrayDYN<char *> getEnabled_InstanceExts(void) {return ICS.enabled_iExts;}
  amVK_ArrayDYN<char *> getEnabled_ValLayers(void) {return ICS.enabled_vLayers;}

  /** currently uses BruteForce */
  uint32_t iExtName_to_index(const char *iExtName);
  uint32_t vLayerName_to_index(const char *vLayerName);



  /**
   *   \│/  ╦  ╔═╗╦  ╦╔═╗╦    ┏━┓
   *   ─ ─  ║  ║╣ ╚╗╔╝║╣ ║    ┏━┛
   *   /│\  ╩═╝╚═╝ ╚╝ ╚═╝╩═╝  ┗━╸
   * 
   * stuffs that I didn't wanna expose
   * stuffs that U shouldn't be using.
   * stuffs that R called internally..
   * stuffs that U don't have2 call...
   * stuffs that R  in CONSTRUCTOR....
   * 
   * \return false if   vkEnumerateInstanceExtensions() called but failed
   *         You should error Handle, and not Call any amVK functions anymore....

   * \param pointer_iec: [OUT] Pointer to Number of Exts. Default: nullptr [see func definition] 
   * \param pointer_iep: [OUT] Pointer to [VkExtensionProperties *].... Hope you understand why :)
   * 
   *     NOTE: [OUT] You don't have to MALLOC, &
   *   PLEASE: don't change any value in memory of [OUT], if you must, then copy it
   */
  bool enum_InstanceExts(bool do_log = true, VkExtensionProperties **pointer_iep = nullptr, int *pointer_iec = nullptr);

  /** 
   * Filter out the must include OS specific SurfaceExts [Pretty easy code, you can take a look yourself ;)]
   * \return false if must needed [2] surface_exts are not available to system....   [_req_surface_exts wont have value]
   */
  bool filter_SurfaceExts(void);

  //---------------- Validation Layers ----------------
  /** Well the name says it all. just check out vlayers_p var */
  void enum_ValLayers(void);










  /** 
   * separated, cz it wasn't looking good
   */
  inline void constructor_default_settings(bool debug_EXTs) {
    enum_InstanceExts();
    filter_SurfaceExts();
    add_InstanceExt( ICS.req_surface_ep[0].extensionName );
    add_InstanceExt( ICS.req_surface_ep[1].extensionName );
    if (debug_EXTs) {
      add_InstanceExt("VK_EXT_debug_report");
      add_InstanceExt("VK_EXT_debug_utils");
    }
  }










  /**
   * \return true if already/successfully loaded.... false if enum_PhysicalDevs \returns false
   * \param force_load: pretty much sure you get this one ;)
   * \param auto_choose: does benchmark too, if not already done
   *                     our benchmark just sucks....  [ \todo]
   */
  bool load_PD_info(bool force_load, bool auto_choose);
  amVK_Array<VkPhysicalDevice> get_PD_array() 
  {
    if (SPD.list == nullptr) { 
      load_PD_info(false, true); 
    }
    return amVK_Array<VkPhysicalDevice>(SPD.list, SPD.n, SPD.n);
  }

  /** rather use the amVK_DeviceMK2::actiavet_device */
  void Activate_Device(amVK_DeviceMK2 *D) {
    if (s_DeviceList.doesExist(D)) 
      s_ActiveD = D;
  }


  /**
   * sets into PD (member var)
   * \return false if vkEnumeratePhysicalDevices \returns 0 physical device....
   * \def force_load: by default, force_load even if already loaded....
   */
  bool enum_PhysicalDevs(void);
  bool enum_PD_qFamilies(void);   //only call after enum_PhysicalDevs()

  /** 
   * Mainly Based on VkPhysicalDeviceFeatures currently....    
   * shaderStorageImageExtendedFormats is Prioratized the most 
   */
  void benchMark_PD(void);

  /** 
   * \return false: if all gpu isUsed,  
   * (PD.chozen is set to most powerfull/Strongest one.... in that case) 
   */
  bool auto_choosePD(void);

  /**
   *               ██████╗ ██╗  ██╗██╗   ██╗███████╗██╗ ██████╗ █████╗ ██╗         ██████╗ ███████╗██╗   ██╗██╗ ██████╗███████╗
   *     ▄ ██╗▄    ██╔══██╗██║  ██║╚██╗ ██╔╝██╔════╝██║██╔════╝██╔══██╗██║         ██╔══██╗██╔════╝██║   ██║██║██╔════╝██╔════╝
   *      ████╗    ██████╔╝███████║ ╚████╔╝ ███████╗██║██║     ███████║██║         ██║  ██║█████╗  ██║   ██║██║██║     █████╗  
   *     ▀╚██╔▀    ██╔═══╝ ██╔══██║  ╚██╔╝  ╚════██║██║██║     ██╔══██║██║         ██║  ██║██╔══╝  ╚██╗ ██╔╝██║██║     ██╔══╝  
   *       ╚═╝     ██║     ██║  ██║   ██║   ███████║██║╚██████╗██║  ██║███████╗    ██████╔╝███████╗ ╚████╔╝ ██║╚██████╗███████╗
   *               ╚═╝     ╚═╝  ╚═╝   ╚═╝   ╚══════╝╚═╝ ╚═════╝╚═╝  ╚═╝╚══════╝    ╚═════╝ ╚══════╝  ╚═══╝  ╚═╝ ╚═════╝╚══════╝
   * ══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════
   *                                                      - VkPhysicalDevice -
   * ═════════════════════════════════════════════════════════ HIGH LIGHTS ═════════════════════════════════════════════════════════
   */
};








#define HEART amVK_InstanceMK2::heart

/** these doesn't need amVK_Device.hh to be included */
#define amVK_CHECK_DEVICE(PARAM_D, VAR_D) \
  if (PARAM_D && !HEART->s_DeviceList.doesExist(PARAM_D)) { \
    amVK_LOG_EX("\u0027param amVK_DeviceMK2 *" << #PARAM_D << "\u0027 doesn't exist in 'amVK_InstanceMK2::heart->D_list'"); \
  } else { VAR_D = PARAM_D; }

#define amVK_SET_activeD(VAR_D) \
  if           (!HEART->s_ActiveD){ amVK_LOG_EX("Either pass a valid amVK_DeviceMK2 as param" << \
                                         "\n  ...or see amVK_InstanceMK2::s_ActiveD & amVK_InstanceMK2::activate_device()  "); } \
  else { VAR_D = HEART->s_ActiveD; }
