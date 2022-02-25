#pragma once

#ifndef amVK_CX_HH
#define amVK_CX_HH

#include <vector>

#ifdef amVK_CX_CPP          // amVK_CX.cpp file
  #define amVK_LOGGER_IMPLIMENTATION  // amASSERT() impl.
#endif
#include "amVK_IN.hh"       // amVK_IN, #define HEART, HEART_CX,   amVK_Logger.hh, amVK_Types.hh, amVK_Utils.hh


/** also defined in amVK_IN */
#ifndef HEART
  #define HEART amVK_IN::heart
#endif


/** 
 *
 *       █████╗ ███╗   ███╗██╗   ██╗██╗  ██╗         ██████╗██╗  ██╗
 *      ██╔══██╗████╗ ████║██║   ██║██║ ██╔╝        ██╔════╝╚██╗██╔╝
 *      ███████║██╔████╔██║██║   ██║█████╔╝         ██║      ╚███╔╝ 
 *      ██╔══██║██║╚██╔╝██║╚██╗ ██╔╝██╔═██╗         ██║      ██╔██╗ 
 *      ██║  ██║██║ ╚═╝ ██║ ╚████╔╝ ██║  ██╗███████╗╚██████╗██╔╝ ██╗
 *      ╚═╝  ╚═╝╚═╝     ╚═╝  ╚═══╝  ╚═╝  ╚═╝╚══════╝ ╚═════╝╚═╝  ╚═╝
 * ═════════════════════════════════════════════════════════════════════
 *                - amVK_CX [Initialization (amVK_Init)] -
 * ═══════════════════════════ HIGH LIGHTS ═════════════════════════════
 * \brief
 *   Funcs intended for usage by you are in amVK_IN.... [ but we do have that inside 'intern' folder 😅]
 *   CAUTION: More than 1 VkInstance is NEVER needed!
 * \see
 *   README.md :: ex1 block for initialization example
 *   ./.readme/amVK.md
 * \note
 *   I came across a Cool theme for vscode: EvaTheme
 *   also ANSI Decorative Fonts: [i am pretty sure the NAME is misused]   \see ASCIIDecorator by helixquar and 'Convert To ASCII Art' by BitBelt... I also se my CUSTOM ONE ;) 
 */
class amVK_CX : public amVK_IN {
 private:
  /** instance Extension */
    amVK_Array<VkExtensionProperties>             m_IEP{};   //'Instance Extension' Properties     [Available to this System]
    amVK_Array<bool>                   m_isEnabled_iExt{};
    amVK_Array<VkExtensionProperties>  m_req_surface_ep{};   // 'Surface Extension' Properties     [for now we know this will be 2; \see filter_SurfaceExts(), allocated there]
    std::vector<char *>                 m_enabled_iExts{};   // USE add_ValLayer

  /** Vulkan Layers */
    amVK_Array<VkLayerProperties>  m_vLayerP{};
    amVK_Array<bool>      m_isEnabled_vLayer{};
    std::vector<char *> m_enabled_vLayers = {};   //WELP, the default one.
    #ifdef amVK_RELEASE
      const bool m_enableDebugLayers_LunarG = false;
    #else
      const bool m_enableDebugLayers_LunarG = true;
    #endif

 public:
  amVK_CX(bool debug_EXTs = true) : amVK_IN() {
    enum_InstanceExts();           // Loads into m_IEP [Force_Load]
    filter_SurfaceExts();          // m_req_surface_ep
    add_InstanceExt( m_req_surface_ep[0].extensionName );
    add_InstanceExt( m_req_surface_ep[1].extensionName );
    if (debug_EXTs) {
      add_InstanceExt("VK_EXT_debug_report");
      add_InstanceExt("VK_EXT_debug_utils");
    }
  }
  ~amVK_CX() {}

  bool check_VkSupport(void) { return true; }; /** \todo */
  void set_VkApplicationInfo(VkApplicationInfo *appInfo = nullptr);

  /**
   *             ┬  ┬┬┌─╔═╗┬─┐┌─┐┌─┐┌┬┐┌─┐╦┌┐┌┌─┐┌┬┐┌─┐┌┐┌┌─┐┌─┐
   *        ───  └┐┌┘├┴┐║  ├┬┘├┤ ├─┤ │ ├┤ ║│││└─┐ │ ├─┤││││  ├┤   ───
   *              └┘ ┴ ┴╚═╝┴└─└─┘┴ ┴ ┴ └─┘╩┘└┘└─┘ ┴ ┴ ┴┘└┘└─┘└─┘   vkCreateInstance
   * |-------------------------------------------------------------------|
   * call \fn add_InstanceExt() before this
   */
  VkInstance create_Instance(void);
       bool destroy_Instance(void);
       
  /**
   * NOTE: impl. is BruteForce   based on   iExtName_to_index()
   * \return true if Added/AddedBefore. false if not Supported on current PC/Device/Computer/System whatever 
   *         [false and AddedBefore has LOG()]
   * 
   * \note calls: \fn enum_InstanceExts() if IEP.data == nullptr;
   */
  bool add_InstanceExt(char *extName);
  bool add_ValLayer(char *vLayerName);
  bool isEnabled_InstanceExt(char *extName) {return m_isEnabled_iExt[iExtName_to_index(extName)];}
  bool isEnabled_ValLayer(char *vLayerName) {return m_isEnabled_vLayer[vLayerName_to_index(vLayerName)];}
  std::vector<char *> getEnabled_InstanceExts(void) {return m_enabled_iExts;}
  std::vector<char *> getEnabled_ValLayers(void) {return m_enabled_vLayers;}
  /** currently uses BruteForce */
  uint32_t iExtName_to_index(char *iExtName);
  uint32_t vLayerName_to_index(char *vLayerName);


  /**
   *   \│/  ╦  ╔═╗╦  ╦╔═╗╦    ┏━┓
   *   ─ ─  ║  ║╣ ╚╗╔╝║╣ ║    ┏━┛
   *   /│\  ╩═╝╚═╝ ╚╝ ╚═╝╩═╝  ┗━╸
   * 
   * stuffs that I didn;t wanna expose
   * stuffs that U shouldn't be using.
   * stuffs that R called internally..
   */
  /**
   * \see IEP
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
   * Filter out the must Include platform (OS) specific SurfaceExts [Pretty easy code, you can take a look yourself ;)]
   * \return false if must needed [2] surface_exts are not available to system....   [_req_surface_exts wont have value]
   */
  bool filter_SurfaceExts(void);

  //---------------- Validation Layers ----------------
  /** Well the name says it all. just check out vlayers_p var */
  void enum_ValLayers(void);
  
  













  /**
   * sets into PD (member var)
   * \return true if already/successfully loaded.... false if enum_PhysicalDevs \returns false
   * \param force_load: pretty much sure you get this one ;)
   * \param auto_choose: does benchmark too, if not already done
   *                     our benchmark just sucks....  [ \todo]
   */
  bool load_PD_info(bool force_load, bool auto_choose);
  amVK_Array<VkPhysicalDevice> get_PD_array() {
    if (PD.list == nullptr) { load_PD_info(false, true); }
    return amVK_Array<VkPhysicalDevice>(PD.list, PD.n, PD.n);
  }

  /** 
   * Only way to set activeD, function not internally called....
   *    PURE VIRTUAL FUNCTION 
   */
  void activate_device(amVK_DeviceMK2 *D) {
    if (D_list.doesExist(D)) activeD = D;
  }

  /**
   *               ██████╗ ██╗  ██╗██╗   ██╗███████╗██╗ ██████╗ █████╗ ██╗         ██████╗ ███████╗██╗   ██╗██╗ ██████╗███████╗
   *     ▄ ██╗▄    ██╔══██╗██║  ██║╚██╗ ██╔╝██╔════╝██║██╔════╝██╔══██╗██║         ██╔══██╗██╔════╝██║   ██║██║██╔════╝██╔════╝
   *      ████╗    ██████╔╝███████║ ╚████╔╝ ███████╗██║██║     ███████║██║         ██║  ██║█████╗  ██║   ██║██║██║     █████╗  
   *     ▀╚██╔▀    ██╔═══╝ ██╔══██║  ╚██╔╝  ╚════██║██║██║     ██╔══██║██║         ██║  ██║██╔══╝  ╚██╗ ██╔╝██║██║     ██╔══╝  
   *       ╚═╝     ██║     ██║  ██║   ██║   ███████║██║╚██████╗██║  ██║███████╗    ██████╔╝███████╗ ╚████╔╝ ██║╚██████╗███████╗
   *               ╚═╝     ╚═╝  ╚═╝   ╚═╝   ╚══════╝╚═╝ ╚═════╝╚═╝  ╚═╝╚══════╝    ╚═════╝ ╚══════╝  ╚═══╝  ╚═╝ ╚═════╝╚══════╝
   * ══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════
   *                                    - VkPhysicalDevice [Enum, Get, Info, Queue Families, Queues] -
   * ═════════════════════════════════════════════════════════ HIGH LIGHTS ═════════════════════════════════════════════════════════
   */
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
};

#endif //#ifndef amVK_CX_HH
