#define amVK_DEVICE_CPP
#include "amVK_Device.hh"


amVK_DeviceMK2::amVK_DeviceMK2(amVK_DevicePreset_Flags DevicePreset, uint32_t ur_exts_n, uint32_t ur_qCIs_n, VkPhysicalDevice PD) : flag(DevicePreset), PD(PD) {
    if (DevicePreset == amVK_DP_UNDEFINED) {
        LOG("[amVK_DP_UNDEFINED] Thats not supported for now....");
    }
    
    if (PD == nullptr) {
        /** ---------- \requires amVK_CX::PD to have the data ---------- */
        if (!HEART->PD.list) {HEART->load_PD_info(false, true);}

        /** PD.chozen gets autoChozen by 'load_PD_info' as the 2nd Param above was true */
        PD = HEART->PD.chozen;
        LOG("GPU SELECTED (Automatically):- "  << HEART->PD.props[HEART->PD.index(HEART->PD.chozen)].deviceName);
    }

    if (ur_qCIs_n > 100) {LOG("[amVK_DeviceMK2 Constructor] ur_qCIs_n > 100..... capping to 100"); ur_qCIs_n = 100;}
    if (ur_exts_n > 200) {LOG("[amVK_DeviceMK2 Constructor] ur_exts_n > 200.... capping to  200"); ur_exts_n = 200;}
    /** internal stuffs Adds on top of it */
    this->qCIs.n = ur_qCIs_n;
    this->exts.n = ur_exts_n;

    this->konfigurieren();
}

bool amVK_DeviceMK2::create(void) {
    VkDeviceCreateInfo the_info = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, nullptr, 0,
        qCIs.neXt, qCIs.data,          0, nullptr, /* [Deprecated] Layer */
        exts.neXt, exts.data,
        &req_ftrs
    };

    VkResult res = vkCreateDevice(this->PD, &the_info, nullptr, &this->D );
    if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res)); LOG("vkCreateDevice() failed, time to call the devs, it's an highly unstable emergency. amASSERT"); amASSERT(true); return false;}

    LOG("VkDevice Created! Yessssss, Time Travel! \n");

    HEART->D_list.push_back(this);
    if (!HEART->PD.isUsed[HEART->PD.chozen_index]) {
         HEART->PD.isUsed[HEART->PD.chozen_index] = true;  /** \todo DOC This heavily.... after adding support for Multi Device */

         amVK_M = new MemoryMK2(this->PD);
    }
    
    return true;
}

bool amVK_DeviceMK2::destroy(void) {
    /** \todo BackUP_StackTrace.... to get written if program closes.... */
    vkDestroyDevice(this->D, nullptr);
    HEART->D_list.erase(HEART->D_list.begin() + HEART->D_list.index(this));
    
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
    }

    VkDeviceMemory mem = nullptr;
    VkResult res = vkAllocateMemory(this->D, &this->img_alloc_info, nullptr, &mem);
    if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res));}

    res = vkBindImageMemory(this->D, IMG, mem, 0);
    if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res));}

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
    }

    VkDeviceMemory mem = nullptr;
    VkResult res = vkAllocateMemory(this->D, &this->buf_alloc_info, nullptr, &mem);
    if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res));}

    res = vkBindBufferMemory(this->D, BUF, mem, 0);
    if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res));}

    return mem;
}















void amVK_DeviceMK2::calc_n_alloc(void) {
    amFUNC_HISTORY_INTERNAL();

    // ----------- PreMod Settings [a.k.a Configurations] ------------
    configure_preMod_settings_based_on_presets:
    {
        if (this->flag & amVK_DP_GRAPHICS) {
            req_Queues += VK_QUEUE_GRAPHICS_BIT;
            qCIs.n++;

            req_exts.VK_KHR_SWAPCHAIN = true;
            exts.n++;
        }
        if (this->flag & amVK_DP_COMPUTE) {
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
void amVK_DeviceMK2::set_qCIs(void) {
    amFUNC_HISTORY_INTERNAL();

    uint32_t PD_index = HEART->PD.index(this->PD);
    amVK_Array<VkQueueFamilyProperties> qFAM_list = HEART->PD.qFamily_lists[PD_index];



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

            if (_graphics_qFAM == 0xFFFFFFFF) {    if (qFLAGS & VK_QUEUE_GRAPHICS_BIT) {_graphics_qFAM = i;}   }
            if (_compute_qFAM == 0xFFFFFFFF)  {    if (qFLAGS & VK_QUEUE_COMPUTE_BIT)  { _compute_qFAM = i;}   }
        }

        if (dedicated_qFAM.graphics != 0xFFFFFFFF) { _graphics_qFAM = dedicated_qFAM.graphics; }
        if (dedicated_qFAM.compute != 0xFFFFFFFF)  {  _compute_qFAM = dedicated_qFAM.compute;  }
    }


    // ----------- Main MODs ------------
    modifications:
    {
        if (req_Queues & VK_QUEUE_GRAPHICS_BIT) { //amVK_DevicePreset_Graphics
            if (_graphics_qFAM == 0xFFFFFFFF) {LOG_EX("Couldn't Find any GRAPHICS qFamily"); konfigurieren_err = true;}
            amVK_ARRAY_PUSH_BACK(qCIs) = {
                VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                nullptr,
                0,        /** flags */
                _graphics_qFAM,
                1,        /** queueCount */
                &_qPRIORITIES
            };
        }
        if (req_Queues & VK_QUEUE_COMPUTE_BIT) {  //amVK_DevicePreset_Compute
            if (!_compute_qFAM == 0xFFFFFFFF) {LOG_EX("Couldn't Find any COMPUTE qFamily"); konfigurieren_err = true;}
            amVK_ARRAY_PUSH_BACK(qCIs) = {
                VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                nullptr,
                0,        /** flags */
                _compute_qFAM,
                1,        /** queueCount */
                &_qPRIORITIES
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
    amFUNC_HISTORY_INTERNAL();

    amVK_Array<VkExtensionProperties> xd = {};
    vkEnumerateDeviceExtensionProperties(this->PD, nullptr, &xd.n, nullptr);
    xd.data = new VkExtensionProperties[xd.n];
    vkEnumerateDeviceExtensionProperties(this->PD, nullptr, &xd.n, xd.data);

    LOG_LOOP_MK2("Device Extensions:- ", i, xd.n, xd.data[i].extensionName);

    // ----------- FIND Device Exts SUP ------------
    find_xd: 
    {
        uint32_t last_found = 0;
        bool *sup_exts_p = reinterpret_cast<bool *> (&sup_exts);

        /** this is better than doing a HashMap, if xd.data is sorted.... */

        for (uint32_t k = 0; k < _DEVICE_EXTS_SORTED_N; k++) {
            for (int i = last_found, lim = xd.n; i < lim; i++) {
                /** most of the time the first 6 CHARS will match, so.... */
                if (strcmp(&xd.data[i].extensionName[6], _deviceExtensions_sorted[k]) == 0) {
                    LOG_MK2(i << " " << _deviceExtensions_sorted[k]);
                    *(sup_exts_p + k) = true;
                    last_found = i;
                    goto nextOne;
                }
            }
            for (int i = 0, lim = last_found; i < lim; i++) {
                if (strcmp(&xd.data[i].extensionName[6], _deviceExtensions_sorted[k]) == 0) {
                    LOG_MK2(i << "   " << _deviceExtensions_sorted[k]);
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
        const bool *req_exts_p = reinterpret_cast<bool *> (&req_exts);
        const bool *sup_exts_p = reinterpret_cast<bool *> (&sup_exts);
        bool result_success = true;

        for (int i = 0; i < sizeof(req_exts); i++) {
            if (req_exts_p[i] && !sup_exts_p[i]) {
                LOG_EX("Device EXT: \u0027" << _deviceExtensions_sorted[i] << "isn't supported.... but is needed by chozen amVK_DevicePresetFlags");
                result_success = false;
                konfigurieren_err = true;
            }
        }
    }

    // ----------- Main MODs ------------
    modifications:
    {
        if (req_exts.VK_KHR_SWAPCHAIN) {
            amVK_ARRAY_PUSH_BACK(exts) = "VK_KHR_swapchain";
        }
    }
}


/** 
    \│/  ┌─┐┌─┐┌─┐┌┬┐┬ ┬┬─┐┌─┐┌─┐
    ─ ─  ├┤ ├┤ ├─┤ │ │ │├┬┘├┤ └─┐
    /│\  └  └─┘┴ ┴ ┴ └─┘┴└─└─┘└─┘
*/
void amVK_DeviceMK2::set_ftrs(void) {
    amFUNC_HISTORY_INTERNAL();

    uint32_t PD_index = HEART->PD.index(this->PD);
    VkPhysicalDeviceFeatures sup_ftrs = HEART->PD.features[PD_index];

    // ----------- Main MODs ------------
    modifications:
    {
        if (this->flag & amVK_DP_3DEngine) {
            if (sup_ftrs.geometryShader) req_ftrs.geometryShader = true;
            if (sup_ftrs.tessellationShader) req_ftrs.tessellationShader = true;
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
    amVK_DeviceMK2 **data = this->data();
    for (int i = 0, lim = this->size(); i < lim; i++) {
      if (data[i]->D == D) {
        return data[i];
      }
    }
    LOG_EX("LogicalDevice doesn't Exist");
    return nullptr;
  }


  #include <typeinfo>
  bool vec_amVK_Device::doesExist(amVK_DeviceMK2 *amVK_D) {
    amVK_DeviceMK2 **data = this->data();
    for (int i = 0, lim = this->size(); i < lim; i++) {
      if (data[i] == amVK_D) {
        return true;
      }
    }
    LOG_EX("amVK_DeviceMK2 doesn't Exist in " << typeid(this).name());
  }

  uint32_t vec_amVK_Device::index(amVK_DeviceMK2 *D) {
    amVK_DeviceMK2 **data = this->data();
    for (int i = 0, lim = this->size(); i < lim; i++) {
      if (data[i] == D) {
        return i;
      }
    }
    LOG_EX("amVK_DeviceMK2 doesn't Exist in " << typeid(this).name());
    return 0xFFFFFFFF;  //UINT_32T_NULL
  }