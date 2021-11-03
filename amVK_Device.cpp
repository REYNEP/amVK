#define amVK_DEVICE_CPP
#include "amVK_Device.hh"

bool amVK_DeviceMK2::create(void) {
    VkDeviceCreateInfo the_info = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, nullptr, 0,
        qCIs.n, qCIs.data,          0, nullptr, /* [Deprecated] Layer */
        exts.n, exts.data,
        &req_ftrs
    };

    //Occupies Almost 20MB
    VkResult res = vkCreateDevice(_PD, &the_info, nullptr, &this->_D );
    if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res)); LOG("vkCreateDevice() failed, time to call the devs, it's an highly unstable emergency. amASSERT"); amASSERT(true); return false;}

    LOG("VkDevice Created! Yessssss, Time Travel! \n");

    HEART->D_list.push_back(this);
    HEART->PD.isUsed[HEART->PD.chozen_index] = true;  /** \todo DOC This heavily.... after adding support for Multi Device */

    this->init_VMA();
    return true;
}

bool amVK_DeviceMK2::destroy(void) {
    /** \todo BackUP_StackTrace.... to get written if program closes.... */
    vkDestroyDevice(_D, nullptr);
    HEART->D_list.erase(HEART->D_list.begin() + HEART->D_list.index(this));
    
    free(qCIs.data);  /** We only malloced once in calc_n_malloc() */
    return true;
}













/** Also    \see amVK_DeviceExtensionsBools     in the HEADER */
static const char *amVK_DeviceExtensions[9] = {
  "VK_NONE_undefined",
  /** amVK_DevicePreset_Graphics */
  "VK_KHR_swapchain",
  /** amVK_DevicePreset_Encode_Decode */
  "VK_KHR_video_decode_queue",
  "VK_KHR_video_encode_queue",
  "VK_KHR_video_queue",
  /** amVK_DevicePreset_Image_Shaders | amVK_DevicePreset_Compositor */
  "VK_KHR_image_format_lists",
  /** VK_KHR_PERFORMANCE_QUERY", */
  /** amVK_DevicePreset_RayTracing */
  "VK_KHR_ray_query",
  "VK_KHR_ray_tracing_pipeline"
};



void amVK_DeviceMK2::calc_n_malloc(void) {
    amFUNC_HISTORY_INTERNAL();

    // ----------- PreMod Settings [a.k.a Configurations] ------------
    configure_preMod_settings_based_on_presets:
    {
        if (_flag & amVK_DP_GRAPHICS) {
            req_Queues += VK_QUEUE_GRAPHICS_BIT;
            qCIs.n++;

            req_exts.VK_KHR_SWAPCHAIN = true;
            exts.n++;
        }
        if (_flag & amVK_DP_COMPUTE) {
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

    uint32_t PD_index = HEART->PD_to_index(_PD);
    amVK_Array<VkQueueFamilyProperties> qFAM_list = HEART->PD.qFamily_lists[PD_index];



    // ----------- Indexes graphicsQueueFamily, computeQueueFamily ------------
    struct dedicated_qFAM_T {
        uint32_t graphics = 0xFFFFFFFF;
        uint32_t  compute = 0xFFFFFFFF;
    } dedicated_qFAM;

    // ----------- FIND Device qFamily INFO & SUP ------------
    find_req_qFAMs:
    {
        for (int i = 0, lim = qFAM_list.size(); i < lim; i++) {
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
            if (_graphics_qFAM == 0xFFFFFFFF) {LOG_EX("Couldn't Find any GRAPHICS qFamily"); mod_err = true;}
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
            if (!_compute_qFAM == 0xFFFFFFFF) {LOG_EX("Couldn't Find any COMPUTE qFamily"); mod_err = true;}
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


/**
    \│/  ┌─┐─┐ ┬┌┬┐┌─┐
    ─ ─  ├┤ ┌┴┬┘ │ └─┐
    /│\  └─┘┴ └─ ┴ └─┘
*/
void amVK_DeviceMK2::set_exts(void) {
    amFUNC_HISTORY_INTERNAL();

    amVK_Array<VkExtensionProperties> sup_exts = {};
    vkEnumerateDeviceExtensionProperties(_PD, nullptr, &sup_exts.n, nullptr);
    sup_exts.data = new VkExtensionProperties[sup_exts.n];
    vkEnumerateDeviceExtensionProperties(_PD, nullptr, &sup_exts.n, sup_exts.data);

    // ----------- FIND Device Exts SUP ------------
    amVK_DeviceExtensionsBools isSup = {};
    find_sup_exts: 
    {
        uint32_t found_n = 0;

        for (int i = 0, lim = sup_exts.n; i < lim; i++) {
            if (found_n == exts.n) break;

            /** most of the time the first 6 CHARS will match, so.... */
            if (strcmp(&sup_exts.data[i].extensionName[6], "_swapchain")) {
                isSup.VK_KHR_SWAPCHAIN = true;
                found_n++;
            }
            /** else if ()
             * MORE SOON, For now only amVK_DevicePreset_Graphics */
        }
    }
    delete[] sup_exts.data;

    find_req_exts:
    {
        const bool *req_exts_p = reinterpret_cast<bool *> (&req_exts);
        const bool *sup_exts_p = reinterpret_cast<bool *> (&isSup);
        bool result_success = true;

        for (int i = 0; i < sizeof(req_exts); i++) {
            if (req_exts_p[i] && !sup_exts_p[i]) {
                LOG_EX("Device Extension: \u0027" << amVK_DeviceExtensions[i] << "isn't supported....");
                result_success = false;
                mod_err = true;
            }
        }
    }

    // ----------- Main MODs ------------
    modifications:
    {
        if (req_exts.VK_KHR_SWAPCHAIN) {
            amVK_ARRAY_PUSH_BACK(exts) = const_cast<char *>(amVK_DeviceExtensions[1]);
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

    uint32_t PD_index = HEART->PD_to_index(_PD);
    VkPhysicalDeviceFeatures sup_ftrs = HEART->PD.features[PD_index];

    // ----------- Main MODs ------------
    modifications:
    {
        if (_flag & amVK_DP_3DEngine) {
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
      if (data[i]->_D == D) {
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