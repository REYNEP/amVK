#pragma once

#ifndef amVK_CX_HH
#define amVK_CX_HH

#include <cstring>          // strcmp()     [cstdlib in .cpp]
#include <vector>

#ifdef amVK_CPP             // amVK_CX.cpp file
  #define amVK_LOGGER_IMPLIMENTATION  // amASSERT() impl.
#endif
#include "amVK_IN.hh"       // amVK_IN, #define HEART, HEART_CX,   amVK_Logger.hh, amVK_Types.hh, amVK_Utils.hh
#include "amVK_Device.hh"



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
 * \brief:
 *   Funcs intended for usage by you are in amVK_IN.   \see README.md :: ex1 block for initialization example
 */
class amVK_CX : public amVK_IN {
 public:
  amVK_CX(void) : amVK_IN() {}
  ~amVK_CX() {}

  bool check_VkSupport(void) { return true; }; /** \todo */
  void set_VkApplicationInfo(VkApplicationInfo *appInfo = nullptr);


  /**
   *             ┬  ┬┬┌─╔═╗┬─┐┌─┐┌─┐┌┬┐┌─┐╦┌┐┌┌─┐┌┬┐┌─┐┌┐┌┌─┐┌─┐
   *        ───  └┐┌┘├┴┐║  ├┬┘├┤ ├─┤ │ ├┤ ║│││└─┐ │ ├─┤││││  ├┤   ───
   *              └┘ ┴ ┴╚═╝┴└─└─┘┴ ┴ ┴ └─┘╩┘└┘└─┘ ┴ ┴ ┴┘└┘└─┘└─┘   vkCreateInstance
   * |-------------------------------------------------------------------|
   * 
   * call \fn add_InstanceExt() before, if you want to enable 
   * 
   * CAUTION: More than 1 VkInstance is NEVER needed! [ \see 'first if-else block' of CreateInstance impl.]
   */
  VkInstance CreateInstance(void);
  /**
   * NOTE: impl. is BruteForce   based on   iExtName_to_index()
   * \return true if Added/AddedBefore. false if not Supported on current PC/Device/Computer/System whatever 
   *         [false and AddedBefore has LOG()]
   * 
   * \note calls: \fn enum_InstanceExts() if IEP.data == nullptr;
   */
  bool add_InstanceExt(char *extName);
  bool add_ValLayer(char *vLayerName);
  bool DestroyInstance(void);
  /**
   *  █░█ ▄▀█ █▀█ █▀
   *  ▀▄▀ █▀█ █▀▄ ▄█
   */
     /** instance Extension */
      amVK_Array<VkExtensionProperties>             IEP{};   //'Instance Extension' Properties     [Available to this System]
      amVK_Array<bool>                  _isEnabled_iExt{};
      amVK_Array<VkExtensionProperties> _req_surface_ep{};   // 'Surface Extension' Properties     [for now we know this will be 2; \see filter_SurfaceExts(), allocated there]
      std::vector<char *>                 enabled_iExts{};   // USE add_ValLayer

     /** Vulkan Layers */
      amVK_Array<VkLayerProperties> vLayerP{};
      amVK_Array<bool>    _isEnabled_vLayer{};
      std::vector<char *> enabled_vLayers = {"VK_LAYER_KHRONOS_validation"};   //WELP, the default one.
      #ifdef amVK_RELEASE
        const bool enableDebugLayers_LunarG = false;
      #else
        const bool enableDebugLayers_LunarG = true;
      #endif


  /**
   *   \│/  ╦  ╔═╗╦  ╦╔═╗╦    ┏━┓
   *   ─ ─  ║  ║╣ ╚╗╔╝║╣ ║    ┏━┛
   *   /│\  ╩═╝╚═╝ ╚╝ ╚═╝╩═╝  ┗━╸
   * 
   * stuffs that I didn;t wanna expose
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
   * \return true if already/successfully loaded.... false if enum_PhysicalDevs \returns false
   * \param force_load: pretty much sure you get this one ;)
   * \param auto_choose: does benchmark too, if not already done
   */
  bool load_PD_info(bool force_load, bool auto_choose);
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







  uint32_t iExtName_to_index(char *iExtName) {
    for (uint32_t i = 0; i < IEP.n; i++) {
      if (strcmp(iExtName, IEP[i].extensionName) == 0) {
        return i;
      }
    }
    return 0xFFFFFFFF;
  }
  uint32_t vLayerName_to_index(char *vLayerName) {
    for (uint32_t i = 0; i < vLayerP.n; i++) {
      if (strcmp(vLayerName, vLayerP[i].layerName) == 0) {
        return i;
      }
    }
    return 0xFFFFFFFF;
  }
};

#endif //#ifndef amVK_CX_HH
