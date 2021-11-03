#pragma once

#ifndef amVK_CX_HH
#define amVK_CX_HH

#include <cstring>          // strcmp()     [cstdlib in .cpp]
#include <vector>

#ifdef amVK_CPP             //amVK_CX.cpp file
  #define amVK_LOGGER_IMPLIMENTATION  // amASSERT() impl.
#endif
#include "amVK_IN.hh"       // amVK_IN, #define HEART, HEART_CX,   amVK_Logger.hh, amVK_Types.hh, amVK_Utils.hh
#ifndef amVK_DEVICE_HH
  class amVK_DeviceMK2;
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
 *                 - amVK_CX [Initialization (amVK_Init)] -
 * ═══════════════════════════ HIGH LIGHTS ═════════════════════════════
 * \brief FUNCTIONS:
 * \fn CreateInstance       [ \see  enum_InstanceExts() & add_InstanceExts()    for enabling VULKAN EXTENSIONS ]
 * \fn CreateDevice         [ \see       load_PD_info() & amVK_DeviceMods()     for under the Hood modifications    based on amVK_DevicePreset_Flags]
 * MODS: \see amVK_DeviceMods() & amVK_DevicePreset_Flags
 * \see CreateDeviceMK2()
 *  MK2: lets you Modify DeviceMods, after DeviceMods does PresetModifications \todo FIX DOCS about MK2 here...
 *       \fn CreateDeviceMK2() 
 *       \fn DeviceModsMK2()
 * 
 * \brief WHY:
 * Literally everything in VULKAN needs VkDevice, we wrap that around in amVK_DeviceMK2....
 * Now till you go and create the VkDevice, you would want to set up sm stuffs like, EXTENSIONS, Queues to use & Features. thats all there is to it
 * So, to query for available Information, you need to first create a Vulkan Instance
 * 
 * 
 * \brief RULES:
 * RULE-I: we dont do much error handling ourself, cz of FAST & FURIOUS performance.... please do that yourselves, we return BOOL    [tho You can enable ValidationLayers    ( andWe do plan on making our own ValLayers) ]                
 * RULE-II: so, if you call same (enum_PhysicalDevs() or even benchMark_PD().... stuffs like that)     its gonna do force_load/calc again....
 * 
 * RULE-III: Function Naming Convention - we use the pythonic way [Only CreateInstance & CreateDevice is exception]
 * RULE-IV: We Assume that you wont change any public variables.... and there is no get_*** or set_*** functions
 * 
 * RULE-V: We dont use _NDEBUG,   we use amVK_RELEASE
 * RULE-VI: uint32_t used mostly for index or size kinda stuffs, as vulkan also follows that
 * 
 * RULE-VII: Here at amVK, we try to Minimize as much IMPLICIT behavior we can.... "EXPLICIT is Better than IMPLICIT" - [PEP20]
 * 
 * RULE-IX: Memory overWrites on amVK can Crash your program.... I am looking at Memory protected allocation ways, tho
 * 
 * RULE-X: smtimes funcs like vkGetPDSurfaceFormats, vkGetSwapchainImagesKHR  calling them for the 2nd time [actually 4th time, cz of query-ing for 'n' first]   wont work.... like it will give you null data
 *         But internally we may need to use these.... then how would you call those vk*** funcs youtself? so we store everything we vkEnum/vkGet in member vars....
 *            for example.... we store the data in `amVK_CX::PD` struct
 * 
 * RUKE-XI: Another thing about amVK is that, we try to not think of any of the STRUCTs or FUNCs or NAMEs in vulkan as like NEW CONCEPTs or OBJECTs or IDEAs
 *          cz, soon it will start feeling like.... 'Whooo, thats a lot of stuffs to remember'
 *            instead we try to think of these things like, 'just sm functions'
 *            How do we find these then? or rathermore relate these?   well we just need to understand the base concepts like RenderPass-FrameBuffer-Swapchain, Pipeline, MemoryManagement
 *                and soon as we see smth, it will come together on its own to form a better idea....
 * 
 * 
 * NAMING-CONVENTIONS: 
 *    FUNCS: mostly will have create_/load_/build_/get_/set_ prefix.... and the latters are like load_ShaderModule, build_Pipeline
 *     VARS: \internal type members are like _amVK_D, _amVK_RP etc.
 *           \private vars are like _pushConst, _descSet, 
 *                    will have '_' prefix.
 *                    \if not related to vulkan struct/type/CI [createInfo]  maybe be 'python_snake_case'
 *           \vars decl. inside funcs are like 'python_snake_case' mostly, but can be 'camelCase' or anything, doesn't matter really!  
 * 
 *           \public vars are like 'camelCase' always
 *           mods vars are like shaderInputs, vert, frag, VIBindings, vertTopo, viewportCount
 *              VIBindings is dif. cz, VI is short_form.... such smtimes can apply to things like amVK_WI_MK2::IMGs....
 *              \exception the_info, OG, OG.MemberName
 * 
 * \brief VARS:
 * HEART: [static amVK_IN *heart]   THE One & Only HEART....   [cz Multi-instance isn't officially supported yet, tho you can \see CreateInstance impl. and use under the hood functions]
 * activeD: [We dont want to Introduce this if you are really NEW to Vulkan], soon as you realize that lots of stuff in VULKAN will be linked by the VkDevice
 *          e.g. RenderPass-Swapchain-FrameBuffers, Attachments, Pipeline, CommandPool-CmdBuf-Queue    REF: [VUID-vkQueueSubmit-pCommandBuffers-00074], [VUID-VkRenderPassBeginInfo-commonparent]
 *          So We introduce this.... this will reduce the Multi Device Hazard in Future when we Introduce Multi VkDevice & SLI/Crossfire (DeviceGroup) Support
 *          \see \fn active_device()    &    Note that, you can freely make it into NOT: STATIC, cz we used HEART->activeD everywhere
 * 
 *    Under the HOOD: [VARS:]
 *        CI: CreateInfo
 *        PD: associated with \fn load_PD_info() stores all the info related to all available VkPhysicalDevice
 *        IEP: well, what we get     from vkEnumInstanceExtensionProperties
 *        vLayerP: ValidationLayer   from vkEnumerateInstanceLayerProperties
 *        enabled_iExts:   std::vector,  but you shouldn't push_back, instead \use \fn enum_InstanceExts() & \fn add_InstanceExts() ]
 *                         [SAME] for enabled_vLayers
 * 
 *        TODO: Do MALLOC once??? [https://softwareengineering.stackexchange.com/questions/191231/how-many-malloc-calls-is-too-many-if-any]
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
   * CAUTION: More than 1 VkInstance is NEVER needed! [ \see 'first if-else block' of CreateInstance impl.]
   */
  VkInstance CreateInstance(void);
  bool DestroyInstance(void);
  /**
   *  █░█ ▄▀█ █▀█ █▀
   *  ▀▄▀ █▀█ █▀▄ ▄█
   */
     /** instance Extension */
      amVK_Array<VkExtensionProperties>             IEP;   //'Instance Extension' Properties     [Available to this System]
      amVK_Array<bool>                  _isEnabled_iExt;
      amVK_Array<VkExtensionProperties> _req_surface_ep;   // 'Surface Extension' Properties     [for now we know this will be 2; \see filter_SurfaceExts(), allocated there]
      std::vector<char *>                 enabled_iExts;   // USE add_ValLayer

     /** Vulkan Layers */
      amVK_Array<VkLayerProperties> vLayerP;
      amVK_Array<bool>    _isEnabled_vLayer;
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


  //---------------- Validation Layers ----------------
  /** Well the name says it all. just check out vlayers_p var */
  void enum_ValLayers(void);
  /** Same as add_InstanceExts(), uses _enabled_valLayers var */
  bool add_ValLayer(char *vLayerName);
  






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
