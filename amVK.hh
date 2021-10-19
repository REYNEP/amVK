#pragma once
#pragma warning(push)
#pragma warning(disable : 4102)

#ifndef amVK_LIB
#define amVK_LIB

#include <cstring>          // strcmp()     [cstdlib in .cpp]
#include <vector>

#ifdef amVK_CPP   //inside amVK.cpp file
  #define amVK_LOGGER_IMPLIMENTATION  // amASSERT() impl.
#endif
#include "amVK_Common.hh"   // amVK_IN, #define HEART, HEART_CX,   amVK_Logger.hh, amVK_Types.hh, amVK_Utils.hh
#ifndef amVK_DEVICE_HH
  class amVK_Device;
  class amVK_DeviceMods;    // defined below amVK_CX class
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
 * Literally everything in VULKAN needs VkDevice, we wrap that around in amVK_Device....
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
  /** \see in amVK_Internel / amVK_IN   [Separated, to make no other file dependant on this one]
    static amVK_IN *heart;
    static amVK_Device *activeD;
    static VkInstance instance;
    static VkApplicationInfo vk_appInfo;

    amVK_Utils::vec_amVK_Device         D_list{};   /** std::vector<amVK_Device *>      yes, [pointer], respect amVK_Device as an INDIVIDUAL 'BIG SMTH' */
    /** [above one] Was STATIC, needed to DEFINE it in amVK.cc....  thus amVK::CreateDevice() didnt work if used in GLOBAL Space of other modules outside main function
  */

  /** Only way to set activeD, not internally used.... this should be explicit */
  void activate_device(amVK_Device *D) {if (D_list.doesExist(D)) {activeD = D;}}

  bool check_VkSupport(void); /** \todo */
  void set_VkApplicationInfo(VkApplicationInfo *appInfo = nullptr);


  /**
   *             ┬  ┬┬┌─╔═╗┬─┐┌─┐┌─┐┌┬┐┌─┐╦┌┐┌┌─┐┌┬┐┌─┐┌┐┌┌─┐┌─┐
   *        ───  └┐┌┘├┴┐║  ├┬┘├┤ ├─┤ │ ├┤ ║│││└─┐ │ ├─┤││││  ├┤   ───
   *              └┘ ┴ ┴╚═╝┴└─└─┘┴ ┴ ┴ └─┘╩┘└┘└─┘ ┴ ┴ ┴┘└┘└─┘└─┘   vkCreateInstance
   * |-------------------------------------------------------------------|
   * CAUTION: More than 1 VkInstance is NEVER needed! [ \see 'first if-else block' of CreateInstance impl.]
   */
  VkInstance CreateInstance(void);
  /**
   *  █░█ ▄▀█ █▀█ █▀
   *  ▀▄▀ █▀█ █▀▄ ▄█
   */
  amVK_Array<VkExtensionProperties>             IEP;   //'Instance Extension' Properties     [Available to this System]
  amVK_Array<bool>                  _isEnabled_iExt;
  amVK_Array<VkExtensionProperties> _req_surface_ep;   // 'Surface Extension' Properties     [for now we know this will be 2; \see filter_SurfaceExts(), allocated there]
  std::vector<char *>                 enabled_iExts;   // USE add_ValLayer

  amVK_Array<VkLayerProperties> vLayerP;
  amVK_Array<bool>    _isEnabled_vLayer;
  std::vector<char *> enabled_vLayers = {"VK_LAYER_KHRONOS_validation"};   //WELP, the default one.
#ifdef amVK_RELEASE
  const bool enableDebugLayers_LunarG = false;
#else
  const bool enableDebugLayers_LunarG = true;
#endif

  amVK_CX(void) : amVK_IN() {}
  ~amVK_CX() {}


  /**
   *               ┬  ┬┬┌─╔═╗┬─┐┌─┐┌─┐┌┬┐┌─┐╔╦╗┌─┐┬  ┬┬┌─┐┌─┐
   *          ───  └┐┌┘├┴┐║  ├┬┘├┤ ├─┤ │ ├┤  ║║├┤ └┐┌┘││  ├┤   ───        
   *                └┘ ┴ ┴╚═╝┴└─└─┘┴ ┴ ┴ └─┘═╩╝└─┘ └┘ ┴└─┘└─┘       vkCreateDevice                                                              
   * |-------------------------------------------------------------------|
   * 
   * \return amVK_Device: ( \see amVK_Device.hh )   use amVK_Device._D   for vkDevice
   * 
   * \param presets: \see amVK_TDevicePreset....  [its more like, when  \param CI is nullptr, we gotta use sm deafault options right....]
   *                 [ \todo you can mix multiple Presets ]
   * 
   * \param CI:  since this CI is like 90% stuffs that you would prolly like to MODIFY.... if making smth COOL & Fast.... we dont wanna hide this
   *             make sure its like FILLED and VALIDATED.... cz if not nullptr, we wont modify at all
   * 
   * \param init_vma:  [ext1] if true device->init_VMA() is called....   [Deprecated, \see amVK_Device::init_VMA & \see amVK_Device::CONSTRUCTOR]
   */
  amVK_Device *CreateDevice(amVK_DevicePreset_Flags presets, VkDeviceCreateInfo *CI = nullptr);
  /**
   * 1. call DeviceModsMK2()   2. change member variables of DeviceMods,    3. call CreateDeviceMK2
   * MK2: lets you Modify DeviceMods, after DeviceMods does PresetModifications
   * 
   * How do you check if supported? after \fn DeviceModsMK2 is called \var PD will have the data. use that 😉
   */
  amVK_Device *CreateDeviceMK2(amVK_DeviceMods *MODS);
  /** Dont do    new amVK_DeviceMods()    cz that class \requires amVK_CX::PD to have the data 
   * \see amVK_DeviceMods::calc_n_malloc() first....
   * \param ur_exts_n: USE: MODS->exts.push_back()    upto that number of times
   * \param ur_qCIs_n: USE: MODS->qCIs.push_back()    upto that number of times
   * \for reset of the Modification Vars, \see amVK_DeviceMods class at the end of this File
  */
  amVK_DeviceMods *DeviceModsMK2(amVK_DevicePreset_Flags presets, uint32_t ur_exts_n, uint32_t ur_qCIs_n);
  
  /**
   *  █░█ ▄▀█ █▀█ █▀
   *  ▀▄▀ █▀█ █▀▄ ▄█
   */
  loaded_PD_info_plus_plus    PD{};   /** \ref amVK_Types.hh */
  bool all_PD_inUse = false;
  /** \see PD.chozen */















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

  /** This function is used INTERNALLY in other amVK files */
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

   TBD Where to MOVE....
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




#include <sstream>          // std::stringstream
// uses _flag & _PD
#define LOG_MODS_ONCE() \
  std::stringstream log_device_name; \
  log_device_name << "Physical Device [" << _PD << "] :- \u0027" << HEART_CX->PD.props[HEART_CX->PD_to_index(_PD)].deviceName << "\u0027 "; \
  LOG("amVK_DeviceMods(" << flag_2_strName(_flag) << ", " << "\u0027" << log_device_name.str() << "\u0027)"); \

#define LOG_MODS_NOTSUP(x) \
  LOG_EX("    " << x); \


/**
 *              █████╗ ███╗   ███╗██╗   ██╗██╗  ██╗        ██████╗ ███████╗██╗   ██╗██╗ ██████╗███████╗███╗   ███╗ ██████╗ ██████╗ ███████╗
 *   ▄ ██╗▄    ██╔══██╗████╗ ████║██║   ██║██║ ██╔╝        ██╔══██╗██╔════╝██║   ██║██║██╔════╝██╔════╝████╗ ████║██╔═══██╗██╔══██╗██╔════╝
 *    ████╗    ███████║██╔████╔██║██║   ██║█████╔╝         ██║  ██║█████╗  ██║   ██║██║██║     █████╗  ██╔████╔██║██║   ██║██║  ██║███████╗
 *   ▀╚██╔▀    ██╔══██║██║╚██╔╝██║╚██╗ ██╔╝██╔═██╗         ██║  ██║██╔══╝  ╚██╗ ██╔╝██║██║     ██╔══╝  ██║╚██╔╝██║██║   ██║██║  ██║╚════██║
 *     ╚═╝     ██║  ██║██║ ╚═╝ ██║ ╚████╔╝ ██║  ██╗███████╗██████╔╝███████╗ ╚████╔╝ ██║╚██████╗███████╗██║ ╚═╝ ██║╚██████╔╝██████╔╝███████║
 *             ╚═╝  ╚═╝╚═╝     ╚═╝  ╚═══╝  ╚═╝  ╚═╝╚══════╝╚═════╝ ╚══════╝  ╚═══╝  ╚═╝ ╚═════╝╚══════╝╚═╝     ╚═╝ ╚═════╝ ╚═════╝ ╚══════╝
 * ══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════
 *                                                  - For VkDeviceCreateInfo modifications -
 * ═════════════════════════════════════════════════════════════ HIGH LIGHTS ════════════════════════════════════════════════════════════════
 * \brief
 * You can now use MK2
 *    MK2: lets you Modify DeviceMods, after DeviceMods does PresetModifications 
 *       \fn amVK_CX::CreateDeviceMK2() 
 *       \fn amVK_CX::DeviceModsMK2()  
 *       \see those two Func's DOCS
 * 
 * \requires amVK_CX::PD to have the data,  thats why we have amVK_CX::DeviceModsMK2
 * 
 * \todo MULTIPLE/MIX-OF Flags support....
 */
class amVK_DeviceMods {
 public:
  amVK_DevicePreset_Flags _flag = amVK_DP_UNDEFINED;
  VkPhysicalDevice _PD = nullptr;
  bool does_PD_sup_mods = true;    //Whats not Supported is LOGGED too.... to stdout/stderr as konfigurieren() calls every function anyway

  /** CONSTRUCTOR */
  amVK_DeviceMods(amVK_DevicePreset_Flags DevicePreset, VkPhysicalDevice PD, bool DoKonfiguration = true) : _flag(DevicePreset), _PD(PD) {if (DoKonfiguration) konfigurieren();}
  /** INITIALIZE */
  void konfigurieren(void) {LOG_MODS_ONCE(); calc_n_malloc(); set_qCIs(); set_exts(); set_ftrs();}


  /**
   *  █░█ ▄▀█ █▀█ █▀
   *  ▀▄▀ █▀█ █▀▄ ▄█
   * Handle with Care !!!!  Dont try to Write 
   */
    /** Queue & Queue Families */
      VkQueueFlags req_Queues = 0;
      amVK_Array<VkDeviceQueueCreateInfo> qCIs = {};  /** is push_back-ed */
      float _qPRIORITIES = 0.0f;
      uint32_t _graphics_qFAM = 0xFFFFFFFF, _compute_qFAM = 0xFFFFFFFF;

    /** Extensions & Features */
      amVK_DeviceExtensionsBools req_exts = {};
      amVK_DeviceExtensionsBools sup_exts = {};
      amVK_Array<char *>             exts = {};   /** Pointers to amVK_DeviceExtensions (is push_back-ed) */
      VkPhysicalDeviceFeatures   req_ftrs = {};   /** Features */
      VkPhysicalDeviceFeatures   sup_ftrs = {};


  /** CLEAN-UP  [DESTRUCTOR] */
    ~amVK_DeviceMods() {free(qCIs.data);}
  
  /** amVK_DevicePreset_Flags to STRING  for print/Log purposes */
    const char *flag_2_strName(amVK_DevicePreset_Flags flag);
 private:
  /** CONFIGURE - relies on amVK_CX::PD [USE: load_PD_info()] */
  void calc_n_malloc(void);
  void set_qCIs(void);
  void set_exts(void);
  void set_ftrs(void);
};


#endif //#ifndef amVK_LIB
#pragma warning(pop)