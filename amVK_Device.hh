#ifndef amVK_DEVICE_HH
#define amVK_DEVICE_HH

#ifdef amVK_DEVICE_CPP
  #define IMPL_VEC_amVK_DEVICE  /** \see  vec_amVK Device  at eof */
#endif

#include "vulkan/vulkan.h"

class amVK_DeviceMK2; //This is a Forward Declaration. Now amVK_IN.hh checks if amVK_DEVICE_HH is defined....
#include "amVK_IN.hh"

#include "intern/amVK_Memory.hh"


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
 * 
 * since we dont have a amVK_PhysicalDevice class, Physical Device related stuffs are mostly in amVK_CX, But you can find sm stuffs here too like the USED Internally section
 * \todo doc this in the MAIN DOC
 */
class amVK_DeviceMK2 {
  public:
    VkDevice _D = nullptr;
    VkPhysicalDevice _PD;
    amVK_DevicePreset_Flags _flag = amVK_DP_UNDEFINED;


    MemoryMK2 *amVK_M = nullptr;          /** as per PD.... so we set this in CONSTRUCTOR */

    /** Modified in \fn amVK_DeviceMK2::BindImageMemory.... even for amVK_WI_MK2::create_Attachments... its ok */
    static inline VkMemoryPropertyFlags     img_mem_flag = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    /** Modified in \fn amVK_DeviceMK2::BindImageMemory */
    static inline VkMemoryAllocateInfo    img_alloc_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, nullptr,
        0, UINT32_T_NULL    /** [.size], [.memoryTypeIndex ] */
    };

    uint32_t img_mem_type = UINT32_T_NULL;
    /** 
     * \brief
     * AllocateImageMemory & Bind that to the \param IMG 
     * 
     * Don't call this for every image Creation.... 
     *     1. \if FORMAT & Extent images are gonna be same size....
     *     2. and \if you didn't change the \param`flags`
     */
    VkDeviceMemory BindImageMemory(VkImage IMG, VkMemoryPropertyFlags flags = img_mem_flag);
    /** 
     * WIP: use when creating new similar images....
     *    img_mem_type should be set, so \fn BindImageMemory should be called once before 
     *      VULKAN ERROR will occur otherwise
     * 
     * \note idont think size = width * height * 4; [a.l.a memSize = imageRes]
     *      so most prolly this will fail!!!!
     */
    VkDeviceMemory BindImageMemoryOpt(VkImage IMG) {
      //alloc_info.allocationSize  = size_bytes;  /** use the last size */
      //alloc_info.memoryTypeIndex = img_mem_type;  already should be set

      VkDeviceMemory mem = nullptr;
      vkAllocateMemory(_D, &this->img_alloc_info, nullptr, &mem);
      vkBindImageMemory(_D, IMG, mem, 0);
      return mem;
    }

    /** Modified in \fn amVK_DeviceMK2::BindImageMemory.... even for amVK_WI_MK2::create_Attachments... its ok */
    static inline VkMemoryPropertyFlags     buf_mem_flag = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    /** Modified in \fn amVK_DeviceMK2::BindImageMemory */
    static inline VkMemoryAllocateInfo    buf_alloc_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, nullptr,
        0, UINT32_T_NULL    /** [.size], [.memoryTypeIndex ] */
    };
    uint32_t buf_mem_type = UINT32_T_NULL;
    VkDeviceMemory BindBufferMemory(VkBuffer BUF, VkMemoryPropertyFlags flags = buf_mem_flag);
      




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
        LOG("GPU SELECTED:- \u0027"  << HEART->PD.props[HEART->PD.index(HEART->PD.chozen)].deviceName << "\u0027 Going in For vkCreateDevice");
      }

      this->qCIs.n = ur_qCIs_n;
      this->exts.n = ur_exts_n;

      this->konfigurieren();

      //MemoryMK2 setup for this PhysicalDevice, \todo don't create if already created on same PD
      amVK_M = new MemoryMK2(_PD);
    }

    /** actually create the VkDevice _D */
    bool create(void);

    ~amVK_DeviceMK2() {}
    /** CLEAN-UP */
    bool destroy(void);

    /** INITIALIZE */
    void konfigurieren(void) { 
      LOG("amVK_DeviceMK2(" << flag_2_strName(_flag) << ", " << "\u0022" << "Physical Device [" << _PD << "] :- \u0027" << HEART->PD.props[HEART->PD.index(_PD)].deviceName << "\u0027 " << "\u0022)");
      calc_n_alloc(); set_qCIs(); set_exts(); set_ftrs();
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
  void calc_n_alloc(void);  /** \todo have a _free() ? */
  void set_qCIs(void);
  void set_exts(void);
  void set_ftrs(void);
};

#endif //#ifndef amVK_DEVICE_HH