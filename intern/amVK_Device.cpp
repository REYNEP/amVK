#define amVK_DEVICE_CPP
#include "amVK_Device.hh"

/**
  \│/  ┌─┐┌─┐┌┐┌┌─┐┌┬┐┬─┐┬ ┬┌─┐┌┬┐┌─┐┬─┐
  ─ ─  │  │ ││││└─┐ │ ├┬┘│ ││   │ │ │├┬┘
  /│\  └─┘└─┘┘└┘└─┘ ┴ ┴└─└─┘└─┘ ┴ └─┘┴└─
 */
amVK_DeviceMK2::amVK_DeviceMK2(VkPhysicalDevice PD, uint32_t ur_exts_n, uint32_t ur_qCIs_n, amVK_DevicePreset_Flags DevicePreset) : flag(DevicePreset), PD(PD) {
    /** Pre-Cautions.... */
        // amVK Support for now
        if (DevicePreset == amVK_DP_UNDEFINED) {
            amVK_LOG("[amVK_DP_UNDEFINED] Thats not supported for now....");
        }
        if (DevicePreset != amVK_DP_GRAPHICS) {
            amVK_LOG("[!amVK_DP_GRAPHICS] Thats not supported for now....");
        }
        
        // see if user passed parameter is valid
        if (PD != nullptr) {
            this->PD_index = HEART->SPD.index(PD);
            if (PD_index == UINT32_T_NULL) {
                amVK_LOG_EX("You passed in a invalid VkPhysicalDevice pointer.... [trying to choose our own]");
            }

            PD = nullptr;
        }

        // XD
        if (PD == nullptr) {
            /** ---------- \requires amVK_InstanceMK2::PD to have the data ---------- */
            if (!HEART->SPD.list) {HEART->load_PD_info(false, true);}

            /** PD.chozen gets autoChozen by 'load_PD_info' as the 2nd Param above was true */
            PD = HEART->SPD.chozen;
            amVK_LOG("GPU SELECTED (Automatically):- "  << HEART->SPD.props[HEART->SPD.index(HEART->SPD.chozen)].deviceName);
            this->PD_index = HEART->SPD.index(HEART->SPD.chozen);
        }
        
        // into Konfigurieren....
        if (ur_qCIs_n > 100) {amVK_LOG("[amVK_DeviceMK2 Constructor] ur_qCIs_n > 100..... capping to 100"); ur_qCIs_n = 100;}
        if (ur_exts_n > 200) {amVK_LOG("[amVK_DeviceMK2 Constructor] ur_exts_n > 200.... capping to  200"); ur_exts_n = 200;}
        /** internal stuffs Adds on top of it */
        this->qCIs.n = ur_qCIs_n;
        this->exts.n = ur_exts_n;
    /** Pre-Cautions END.... */

    this->konfigurieren();
}

/**
  \│/  ╦  ╦┬┌─╔╦╗┌─┐┬  ┬┬┌─┐┌─┐
  ─ ─  ╚╗╔╝├┴┐ ║║├┤ └┐┌┘││  ├┤ 
  /│\   ╚╝ ┴ ┴═╩╝└─┘ └┘ ┴└─┘└─┘
 */
bool amVK_DeviceMK2::Create_VkDevice(void) {
    VkDeviceCreateInfo the_info = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, nullptr, 0,
        qCIs.neXt, qCIs.data,          0, nullptr, /* [Deprecated] Layer */
        exts.neXt, exts.data,
        &m_req_ftrs
    };

    VkResult res = vkCreateDevice(this->PD, &the_info, nullptr, &this->D );
    if (res != VK_SUCCESS) {amVK_LOG_EX(amVK_Utils::vulkan_result_msg(res)); amVK_LOG("vkCreateDevice() failed, time to call the devs, it's an highly unstable emergency. amASSERT"); amASSERT(true); return false;}


    amVK_LOG("VkDevice Created! Yessssss, Time Travel! \n");
    amVK_ARRAY_PUSH_BACK(HEART->s_DeviceList) = this;


    if (!HEART->SPD.isUsed[HEART->SPD.chozen_index]) {
         HEART->SPD.isUsed[HEART->SPD.chozen_index] = true;  /** \todo DOC This heavily.... after adding support for Multi Device */

         amVK_M = new MemoryMK2(this->PD);
    }
    
    return true;
}

bool amVK_DeviceMK2::Destroy_VkDevice(void) {
    /** \todo BackUP_StackTrace.... to get written if program closes.... */
    vkDestroyDevice(this->D, nullptr);
    HEART->s_DeviceList.erase(HEART->s_DeviceList.index(this));
    
    free(qCIs.data);  /** We only malloced once in calc_n_alloc() */
    return true;
}




VkDeviceMemory amVK_DeviceMK2::BindImageMemory(VkImage IMG, VkMemoryPropertyFlags flags) {
    VkMemoryRequirements req;
    vkGetImageMemoryRequirements(this->D, IMG, &req);
    
    img_alloc_info.allocationSize  = req.size;

    find_memory_type:
    {
        if (flags != this->img_mem_flag) {
            img_alloc_info.memoryTypeIndex = amVK_M->_find_mem_type(req.memoryTypeBits, flags);
        } else {
            if (img_mem_type == UINT32_T_NULL) {
                img_mem_type = amVK_M->_find_mem_type(req.memoryTypeBits, this->img_mem_flag);
            }
            img_alloc_info.memoryTypeIndex = img_mem_type;
        }

        VkPhysicalDeviceMemoryProperties *mem_props =  &( HEART->SPD.mem_props[ HEART->SPD.index(PD)] );
        int i = img_alloc_info.memoryTypeIndex;
        //amVK_LOG_EX("Memory Type[" << i << "]:- " << mem_props->memoryTypes[i].propertyFlags << "\n"
        //         << "       Heap[" << mem_props->memoryTypes[i].heapIndex << "]:- " << mem_props->memoryHeaps[mem_props->memoryTypes[i].heapIndex].flags << "      ["
        //                                                                               << mem_props->memoryHeaps[mem_props->memoryTypes[i].heapIndex].size);
    }

    VkDeviceMemory mem = nullptr;
    VkResult res = vkAllocateMemory(this->D, &this->img_alloc_info, nullptr, &mem);
    if (res != VK_SUCCESS) {amVK_LOG_EX(amVK_Utils::vulkan_result_msg(res));}

    res = vkBindImageMemory(this->D, IMG, mem, 0);
    if (res != VK_SUCCESS) {amVK_LOG_EX(amVK_Utils::vulkan_result_msg(res));}

    return mem;
}

VkDeviceMemory amVK_DeviceMK2::BindBufferMemory(VkBuffer BUF, VkMemoryPropertyFlags flags) {
    VkMemoryRequirements req;
    vkGetBufferMemoryRequirements(this->D, BUF, &req);
    
    buf_alloc_info.allocationSize  = req.size;

    find_memory_type:
    {
        if (flags != this->buf_mem_flag) {
            buf_alloc_info.memoryTypeIndex = amVK_M->_find_mem_type(req.memoryTypeBits, flags);
        } else {
            if (buf_mem_type == UINT32_T_NULL) {
                buf_mem_type = amVK_M->_find_mem_type(req.memoryTypeBits, this->buf_mem_flag);
            }
            buf_alloc_info.memoryTypeIndex = buf_mem_type;
        }

        //VkPhysicalDeviceMemoryProperties *mem_props =  &( HEART->PD.mem_props[ HEART->PD.index(PD)] );
        //int i = buf_alloc_info.memoryTypeIndex;
        //amVK_LOG_EX("Memory Type[" << i << "]:- " << mem_props->memoryTypes[i].propertyFlags << "\n"
        //         << "       Heap[" << mem_props->memoryTypes[i].heapIndex << "]:- " << mem_props->memoryHeaps[mem_props->memoryTypes[i].heapIndex].flags << "      ["
        //                                                                               << mem_props->memoryHeaps[mem_props->memoryTypes[i].heapIndex].size);
    }

    VkDeviceMemory mem = nullptr;
    VkResult res = vkAllocateMemory(this->D, &this->buf_alloc_info, nullptr, &mem);
    if (res != VK_SUCCESS) {amVK_LOG_EX(amVK_Utils::vulkan_result_msg(res));}

    res = vkBindBufferMemory(this->D, BUF, mem, 0);
    if (res != VK_SUCCESS) {amVK_LOG_EX(amVK_Utils::vulkan_result_msg(res));}

    return mem;
}














/**
  \│/  ┌┬┐┌─┐┬  ┬  ┌─┐┌─┐
  ─ ─  │││├─┤│  │  │ ││  
  /│\  ┴ ┴┴ ┴┴─┘┴─┘└─┘└─┘
 */
void amVK_DeviceMK2::calc_n_alloc(void) {
    _LOG("amVK_DeviceMK2::calc_n_alloc()");

    // ----------- PreMod Settings [a.k.a Configurations] ------------
    configure_preMod_settings_based_on_presets:
    {
        if (this->flag & amVK_DP_GRAPHICS) {
            m_req_Queues += VK_QUEUE_GRAPHICS_BIT;
            qCIs.n++;

            m_req_exts.VK_KHR_SWAPCHAIN = true;
            exts.n++;
        }
        if (this->flag & amVK_DP_COMPUTE) {
            m_req_Queues += VK_QUEUE_COMPUTE_BIT;
            qCIs.n++;
        }
    }


    // ----------- Memory Allocation [MALLOC] ------------
    memory_allocation_malloc:
    {
        /* Mixed with Configuration above
            if (m_req_Queues & VK_QUEUE_GRAPHICS_BIT) { qCIs.n++; }
            if (m_req_Queues & VK_QUEUE_COMPUTE_BIT)  { qCIs.n++; } 
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
void amVK_DeviceMK2::set_qCIs(void) {
    _LOG("amVK_DeviceMK2::set_qCIs()");

    uint32_t PD_index = HEART->SPD.index(this->PD);
    amVK_Array<VkQueueFamilyProperties> qFAM_list = HEART->SPD.qFamily_lists[PD_index];



    // ----------- Indexes graphicsQueueFamily, computeQueueFamily ------------
    struct dedicated_qFAM_T {
        uint32_t graphics = 0xFFFFFFFF;
        uint32_t  compute = 0xFFFFFFFF;
    } dedicated_qFAM;

    // ----------- FIND Device qFamily INFO & SUP ------------
    find_req_qFAMs:
    {
        for (int i = 0, lim = qFAM_list.n; i < lim; i++) {
            // We try to find if any qFAM support ONLY DEDICATED qTYPE   [bcz: https://www.reddit.com/r/vulkan/comments/bw47tg/comment/epvnikg/]
            VkQueueFlags qFLAGS = qFAM_list[i].queueFlags;
            
            // TODO: smtimes graphics & presentation qFAM is same.... But still graphics might be a DEDICATED one....
            if      (qFLAGS == VK_QUEUE_GRAPHICS_BIT) { dedicated_qFAM.graphics = i;} 
            else if (qFLAGS == VK_QUEUE_COMPUTE_BIT)  { dedicated_qFAM.compute  = i;}

            if (m_graphics_qFAM == 0xFFFFFFFF) {    if (qFLAGS & VK_QUEUE_GRAPHICS_BIT) {m_graphics_qFAM = i;}   }
            if ( m_compute_qFAM == 0xFFFFFFFF) {    if (qFLAGS & VK_QUEUE_COMPUTE_BIT)  { m_compute_qFAM = i;}   }
        }

        if (dedicated_qFAM.graphics != 0xFFFFFFFF) { m_graphics_qFAM = dedicated_qFAM.graphics; }
        if (dedicated_qFAM.compute != 0xFFFFFFFF)  {  m_compute_qFAM = dedicated_qFAM.compute;  }
    }


    // ----------- Main MODs ------------
    modifications:
    {
        if (m_req_Queues & VK_QUEUE_GRAPHICS_BIT) { //amVK_DevicePreset_Graphics
            if (m_graphics_qFAM == 0xFFFFFFFF) {amVK_LOG_EX("Couldn't Find any GRAPHICS qFamily"); konfigurieren_err = true;}
            amVK_ARRAY_PUSH_BACK(qCIs) = {
                VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                nullptr,
                0,        /** flags */
                m_graphics_qFAM,
                1,        /** queueCount */
                &m_qPRIORITIES
            };
        }
        if (m_req_Queues & VK_QUEUE_COMPUTE_BIT) {  //amVK_DevicePreset_Compute
            if (m_compute_qFAM == 0xFFFFFFFF) {amVK_LOG_EX("Couldn't Find any COMPUTE qFamily"); konfigurieren_err = true;}
            amVK_ARRAY_PUSH_BACK(qCIs) = {
                VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                nullptr,
                0,        /** flags */
                m_compute_qFAM,
                1,        /** queueCount */
                &m_qPRIORITIES
            };
        }
    }
}


#define _DEVICE_EXTS_SORTED_N 11
static const char *_deviceExtensions_sorted[_DEVICE_EXTS_SORTED_N] = {  /** must Correspond to amVK_DeviceExtensionsBools items */
  /** VK_KHR */
  "_image_format_list",
  "_imageless_framebuffer",

  "_maintenance2",

  "_performance_query",

  "_ray_query",
  "_ray_tracing_pipeline",

  "_swapchain",     /** amVK_DevicePreset_Graphics */

  "_video_decode_queue",
  "_video_encode_queue",
  "_video_queue",

  /** VK_EXT */
  "_memory_budget"
};

/**
    \│/  ┌─┐─┐ ┬┌┬┐┌─┐
    ─ ─  ├┤ ┌┴┬┘ │ └─┐
    /│\  └─┘┴ └─ ┴ └─┘
*/
void amVK_DeviceMK2::set_exts(void) {
    _LOG("amVK_DeviceMK2::set_exts");

    amVK_Array<VkExtensionProperties> xd = {};
    vkEnumerateDeviceExtensionProperties(this->PD, nullptr, &xd.n, nullptr);
    xd.data = new VkExtensionProperties[xd.n];
    vkEnumerateDeviceExtensionProperties(this->PD, nullptr, &xd.n, xd.data);

    _LOG_LOOP("Device Extensions:- ", i, xd.n, xd.data[i].extensionName);

    // ----------- FIND Device Exts SUP ------------
    find_xd: 
    {
        uint32_t last_found = 0;
        bool *sup_exts_p = reinterpret_cast<bool *> (&m_sup_exts);

        /** this is better than doing a HashMap, if xd.data is sorted.... */

        for (uint32_t k = 0; k < _DEVICE_EXTS_SORTED_N; k++) {
            for (int i = last_found, lim = xd.n; i < lim; i++) {
                /** most of the time the first 6 CHARS will match, so.... */
                if (strcmp(&xd.data[i].extensionName[6], _deviceExtensions_sorted[k]) == 0) {
                    _LOG(i << " " << _deviceExtensions_sorted[k]);
                    *(sup_exts_p + k) = true;
                    last_found = i;
                    goto nextOne;
                }
            }
            for (int i = 0, lim = last_found; i < lim; i++) {
                if (strcmp(&xd.data[i].extensionName[6], _deviceExtensions_sorted[k]) == 0) {
                    _LOG(i << "   " << _deviceExtensions_sorted[k]);
                    *(sup_exts_p + k) = true;
                    last_found = i;
                    goto nextOne;
                }
            }

            nextOne: continue;
        }
    }
    delete[] xd.data;

    find_req_exts:
    {
        const bool *req_exts_p = reinterpret_cast<bool *> (&m_req_exts);
        const bool *sup_exts_p = reinterpret_cast<bool *> (&m_sup_exts);
        bool result_success = true;

        for (int i = 0; i < sizeof(m_req_exts); i++) {
            if (req_exts_p[i] && !sup_exts_p[i]) {
                amVK_LOG_EX("Device EXT: \u0027" << _deviceExtensions_sorted[i] << "isn't supported.... but is needed by chozen amVK_DevicePresetFlags");
                result_success = false;
                konfigurieren_err = true;
            }
        }
    }

    // ----------- Main MODs ------------
    modifications:
    {
        if (m_req_exts.VK_KHR_SWAPCHAIN) {
            amVK_ARRAY_PUSH_BACK(exts) = (char *)"VK_KHR_swapchain";
        }
    }
}


/** 
    \│/  ┌─┐┌─┐┌─┐┌┬┐┬ ┬┬─┐┌─┐┌─┐
    ─ ─  ├┤ ├┤ ├─┤ │ │ │├┬┘├┤ └─┐
    /│\  └  └─┘┴ ┴ ┴ └─┘┴└─└─┘└─┘
*/
void amVK_DeviceMK2::set_ftrs(void) {
    _LOG("amVK_DeviceMK2::set_ftrs");

    uint32_t PD_index = HEART->SPD.index(this->PD);
    VkPhysicalDeviceFeatures *sup_ftrs = &HEART->SPD.features[PD_index];

    // ----------- Main MODs ------------
    modifications:
    {
        if (this->flag & amVK_DP_3DEngine) {
            if (sup_ftrs->geometryShader) m_req_ftrs.geometryShader = true;
            if (sup_ftrs->tessellationShader) m_req_ftrs.tessellationShader = true;
        }

        /** Sparse \todo */
        /** shaderStorageImageExtendedFormats \todo */
    }
}






/**
 *   \│/  ┬  ┬┌─┐┌─┐    ┌─┐┌┬┐╦  ╦╦╔═    ╔╦╗┌─┐┬  ┬┬┌─┐┌─┐
 *   ─ ─  └┐┌┘├┤ │      ├─┤│││╚╗╔╝╠╩╗     ║║├┤ └┐┌┘││  ├┤ 
 *   /│\   └┘ └─┘└─┘────┴ ┴┴ ┴ ╚╝ ╩ ╩────═╩╝└─┘ └┘ ┴└─┘└─┘
 */
  amVK_DeviceMK2 *vec_amVK_Device::operator[](VkDevice D) {
    for (int i = 0, lim = this->size(); i < lim; i++) {
      if (data[i]->D == D) {
        return data[i];
      }
    }
    amVK_LOG_EX("LogicalDevice doesn't Exist");
    return nullptr;
  }


  /** \todo remove <typeinfo> ??? */
  #include <typeinfo>
  bool vec_amVK_Device::doesExist(amVK_DeviceMK2 *amVK_D) {
    for (int i = 0, lim = this->size(); i < lim; i++) {
      if (data[i] == amVK_D) {
        return true;
      }
    }
    amVK_LOG_EX("amVK_DeviceMK2 doesn't Exist in " << typeid(this).name());
    return false;
  }

  uint32_t vec_amVK_Device::index(amVK_DeviceMK2 *D) {
    for (int i = 0, lim = this->size(); i < lim; i++) {
      if (data[i] == D) {
        return i;
      }
    }
    amVK_LOG_EX("amVK_DeviceMK2 doesn't Exist in " << typeid(this).name());
    return 0xFFFFFFFF;  //UINT_32T_NULL
  }

  bool vec_amVK_Device::erase(uint32_t nth) {
    if (neXt <= 1) {
        amVK_LOG_EX("removing the last element...." << typeid(this).name());
        neXt = 0;
        return true;
    }

    uint32_t lastElem_i = neXt - 1;
    if (nth != lastElem_i) {
        data[nth] = data[lastElem_i];
    }

    neXt--;
    return true;
  }