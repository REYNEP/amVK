#ifndef amVK_DEVICE_HH
#define amVK_DEVICE_HH

#ifdef amVK_DEVICE_CPP
  #ifdef amVK_AMD_VMA_SUP
    #define VMA_IMPLEMENTATION
  #endif
  #define IMPL_VEC_amVK_DEVICE  /** \see  vec_amVK Device  at eof */
#endif

#include "vulkan/vulkan.h"
/** We use VMA for now    [VMA_IMPLIMENTATION used in this Translation Unit] */
#include "vk_mem_alloc.h"
#define amVK_DO_INIT_VMA true

class amVK_DeviceMK2;
#include "amVK_IN.hh"


/**
 *        █████╗ ███╗   ███╗██╗   ██╗██╗  ██╗        ██████╗ ███████╗██╗   ██╗██╗ ██████╗███████╗
 *       ██╔══██╗████╗ ████║██║   ██║██║ ██╔╝        ██╔══██╗██╔════╝██║   ██║██║██╔════╝██╔════╝
 *       ███████║██╔████╔██║██║   ██║█████╔╝         ██║  ██║█████╗  ██║   ██║██║██║     █████╗  
 *       ██╔══██║██║╚██╔╝██║╚██╗ ██╔╝██╔═██╗         ██║  ██║██╔══╝  ╚██╗ ██╔╝██║██║     ██╔══╝  
 *       ██║  ██║██║ ╚═╝ ██║ ╚████╔╝ ██║  ██╗███████╗██████╔╝███████╗ ╚████╔╝ ██║╚██████╗███████╗
 *       ╚═╝  ╚═╝╚═╝     ╚═╝  ╚═══╝  ╚═╝  ╚═╝╚══════╝╚═════╝ ╚══════╝  ╚═══╝  ╚═╝ ╚═════╝╚══════╝
 * ══════════════════════════════════════════════════════════════════════════════════════════════════
 *                           - VkDevice & Merged: [Originally amVK_DeviceMods] -
 * ═════════════════════════════════════════ HIGH  LIGHTS ═══════════════════════════════════════════
 * \brief
 * Handles everything of VkDeviceCreateInfo.... QueueCreateInfos, DeviceExtensions & DeviceFeatures
 * and \external Initiate VMA [VulkanMemoryAllocator by AMD]    --    [external as in we are using an 'External' Library
 * 
 * since we dont have a amVK_PhysicalDevice class, Physical Device related stuffs are mostly in amVK_CX, But you can find sm stuffs here too like the USED Internally section
 * \todo doc this in the MAIN DOC
 * 
 * \TODO: Should we change `calc_n_malloc` to `alloc`?
 */
class amVK_DeviceMK2 {
  public:
    VkDevice _D = nullptr;
    VkPhysicalDevice _PD;
    amVK_DevicePreset_Flags _flag = amVK_DP_UNDEFINED;

    VmaAllocator _allocator = nullptr;    /** \see \fn init_VMA() */
      
    /**
     * \param presets: \see amVK_TDevicePreset....  [its more like, when  \param CI is nullptr, we gotta use sm deafault options right....]
     *                 [ \todo you can mix multiple Presets ]
     * 
     * \requires amVK_CX::PD to have the data, so we call amVK_CX::load_PD_info();
     * 
     * \param ur_exts_n: USE: amVK_ARRAY_PUSH_BACK(device->exts) = ("VK_KHR_imageless_framebuffer");
     * \param ur_qCIs_n: USE: amVK_ARRAY_PUSH_BACK(device->qCIs)
     * 
     * \for reset of the Modification Vars, \todo
     */
    amVK_DeviceMK2(amVK_DevicePreset_Flags DevicePreset, uint32_t ur_exts_n, uint32_t ur_qCIs_n, VkPhysicalDevice PD = nullptr) : _flag(DevicePreset), _PD(PD) {
      if (DevicePreset == amVK_DP_UNDEFINED) {
          LOG("Thats not supported for now....");
      }

      if (!HEART->PD.list) {HEART->load_PD_info(false, true);}

      if (PD == nullptr) {
        _PD = HEART->PD.chozen;
        LOG("GPU SELECTED:- \u0027"  << HEART->PD.props[HEART->PD_to_index(HEART->PD.chozen)].deviceName << "\u0027 Going in For vkCreateDevice");
      }

      this->qCIs.n = ur_qCIs_n;
      this->exts.n = ur_exts_n;

      this->konfigurieren();
    }

    /** actually create the VkDevice _D */
    bool create(void);

    ~amVK_DeviceMK2() {}
    /** CLEAN-UP */
    bool destroy(void);

    /** INITIALIZE */
    void konfigurieren(void) { 
      LOG("amVK_DeviceMK2(" << flag_2_strName(_flag) << ", " << "\u0022" << "Physical Device [" << _PD << "] :- \u0027" << HEART->PD.props[HEART->PD_to_index(_PD)].deviceName << "\u0027 " << "\u0022)");
      calc_n_malloc(); set_qCIs(); set_exts(); set_ftrs();
    }

    


  /** only \if you used  amVK_DevicePreset_Flags */
    uint32_t qIndex_MK2 = 0;
    uint32_t get_graphics_qFamily(void) {return this->_graphics_qFAM;};
    VkQueue  get_graphics_queue(void)   {VkQueue Q = nullptr; vkGetDeviceQueue(_D, this->_graphics_qFAM, this->qIndex_MK2, &Q); return Q;}            

  
  




  /**
   *  █░█ ▄▀█ █▀█ █▀
   *  ▀▄▀ █▀█ █▀▄ ▄█
   */
    /** Queue & Queue Families */
      VkQueueFlags req_Queues = 0;
      amVK_Array<VkDeviceQueueCreateInfo> qCIs = {};  /** is push_back-ed */
      float _qPRIORITIES = 0.0f;
      uint32_t _graphics_qFAM = 0xFFFFFFFF, _compute_qFAM = 0xFFFFFFFF;

    /** Extensions & Features */
      amVK_DeviceExtensionsBools req_exts = {};   /** Extensions */
      amVK_DeviceExtensionsBools sup_exts = {};
      amVK_Array<char *>             exts = {};   /** Pointers to amVK_DeviceExtensions (is push_back-ed) */
      VkPhysicalDeviceFeatures   req_ftrs = {};   /** Features */
      VkPhysicalDeviceFeatures   sup_ftrs = {};

  bool mod_err = false;

 private:
  /** CONFIGURE - relies on amVK_CX::PD   USE: amVK_CX::load_PD_info() before */
  void calc_n_malloc(void);
  void set_qCIs(void);
  void set_exts(void);
  void set_ftrs(void);



  /**
   *              █████╗ ███╗   ███╗██████╗     ██╗   ██╗███╗   ███╗ █████╗ 
   *   ▄ ██╗▄    ██╔══██╗████╗ ████║██╔══██╗    ██║   ██║████╗ ████║██╔══██╗
   *    ████╗    ███████║██╔████╔██║██║  ██║    ██║   ██║██╔████╔██║███████║
   *   ▀╚██╔▀    ██╔══██║██║╚██╔╝██║██║  ██║    ╚██╗ ██╔╝██║╚██╔╝██║██╔══██║
   *     ╚═╝     ██║  ██║██║ ╚═╝ ██║██████╔╝     ╚████╔╝ ██║ ╚═╝ ██║██║  ██║
   *             ╚═╝  ╚═╝╚═╝     ╚═╝╚═════╝       ╚═══╝  ╚═╝     ╚═╝╚═╝  ╚═╝
   * ═════════════════════════════════════════════════════════════════════════
   *                   - AMD VMA (Vulkan Memory Allocator) -
   * ═════════════════════════════ HIGH LIGHTS ═══════════════════════════════
   * \todo Doc....
   */
  VmaAllocator init_VMA(void) {
    VmaAllocatorCreateInfo info = {};
        info.physicalDevice = _PD;
        info.device = _D;
        info.instance = HEART->instance;
    vmaCreateAllocator(&info, &this->_allocator);

    return this->_allocator;
  }
};

#endif //#ifndef amVK_DEVICE_HH