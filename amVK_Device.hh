#pragma once
/** 
 * As of 2022, MAR 3, amVK moved to using pragma, 
 * however amVK_Instance.hh checks if amVK_DEVICE_HH is defined or not. 
 * so we just kept the Include Guards 
 */
#ifndef amVK_DEVICE_HH
#define amVK_DEVICE_HH
class amVK_DeviceMK2;
#include "amVK_Instance.hh"
#include "intern/amVK_Memory.hh"    /** amVK_Memory is in WIP stage.... \todo it shouldn't be here */




/**
 * ══════════════════════════════════════════════════════════════════════════════════════════════════
 *                                         - amVK_Device -
 * ═════════════════════════════════════════ HIGH LIGHTS ════════════════════════════════════════════
 * \brief
 *  Handles everything of VkDeviceCreateInfo.... VkDeviceQueueQueueCreateInfo [s], DeviceExtensions & DeviceFeatures
 *  MemoryMK2, \fn BindImageMemory(), \fn BindBufferMemory()
 * 
 * since we dont have a amVK_PhysicalDevice class, Physical Device related stuffs are mostly in amVK_CX
 * \todo doc this in the MAIN DOC
 * \todo MULTI-DEVICE Support
 * \todo have a _free()   for  \fn calc_n_alloc() ?
 * \todo reset of the Modification Vars 
 * \todo you can mix multiple DevicePresets
 * 
 * \todo add error detection for get_graphics_queue()
 */
class amVK_DeviceMK2 {
    public:
    VkDevice                   D = nullptr;
    VkPhysicalDevice          PD = nullptr;
    uint32_t            PD_index = 0;      /** index into HEART->SPD...*/
    amVK_DevicePreset_Flags flag = amVK_DP_UNDEFINED;

    bool  Create_VkDevice(void);
    bool Destroy_VkDevice(void);
    inline bool  Activate_Device(void) { HEART->s_ActiveD = this; return true;}

    /**
     * check amVK_SurfaceMK2 first....
     * [this function calls konfigurieren()]
     *
     * \param  PD: check amVK_SurfaceMK2::select_DisplayDevice(), or pass nullptr to autoChoose
     * 
     * \param   ur_exts_n: [MAX 200] USE: amVK_ARRAY_PUSH_BACK(device->exts) = ("VK_KHR_imageless_framebuffer");
     * \param   ur_qCIs_n: [MAX 100] USE: amVK_ARRAY_PUSH_BACK(device->qCIs)
     * 
     * \param DevicePreset: [see amVK_TDevicePreset...]
     */
    amVK_DeviceMK2(VkPhysicalDevice PD, uint32_t ur_exts_n, uint32_t ur_qCIs_n, amVK_DevicePreset_Flags DevicePreset);
    ~amVK_DeviceMK2() {}

  protected:
    amVK_DeviceMK2(const amVK_DeviceMK2&) = delete;             // Brendan's Solution
    amVK_DeviceMK2& operator=(const amVK_DeviceMK2&) = delete;  // Brendan's Solution

    /** [INIT] Called in Constructor */
    bool konfigurieren(void) { calc_n_alloc(); /** +qFamilies */ set_qCIs(); set_exts(); set_ftrs(); return konfigurieren_err; }
    public:
    bool konfigurieren_err = false;





    private:
    void calc_n_alloc(void);
    void set_qCIs(void);
    void set_exts(void);
    void set_ftrs(void);

    public:
    /** Extensions & Features */
    amVK_DeviceExtsBools              m_req_exts = {};      /** Extensions */
    amVK_DeviceExtsBools              m_sup_exts = {};

    amVK_Array<char *>                      exts = {};      /**            [passed to VkDeviceCreateInfo] */
    VkPhysicalDeviceFeatures          m_req_ftrs = {};      /** Features   [passed to VkDeviceCreateInfo] */
    amVK_Array<VkDeviceQueueCreateInfo>     qCIs = {};      /** Queues     [passed to VKDeviceCreateInfo] */

    VkQueueFlags                    m_req_Queues = 0;       /** e.g. VK_QUEUE_GRAPHICS_BIT, [set by calc_n_alloc] */
    float                          m_qPRIORITIES = 0.0f;
    uint32_t                     m_graphics_qFAM = 0xFFFFFFFF;             
    uint32_t                      m_compute_qFAM = 0xFFFFFFFF;

    inline VkPhysicalDeviceFeatures*get_sup_ftrs(void)  { return &HEART->SPD.features[PD_index]; }
                     VkQueue  get_graphics_queue(void)  { VkQueue Q = nullptr; vkGetDeviceQueue(D, this->m_graphics_qFAM, 0, &Q); return Q; }





    public:
    MemoryMK2 *amVK_M = nullptr;          /** as per PD.... so we set this in \fn create() */

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
        vkAllocateMemory(D, &this->img_alloc_info, nullptr, &mem);
        vkBindImageMemory(D, IMG, mem, 0);
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
};

#endif //#ifndef amVK_DEVICE_HH