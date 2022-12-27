#ifdef amVK_IN_NOSTDLIB
    #error "You might have defined amVK_IN_NOSTDLIB in your makefile.... its getting defined for amVK_CX.cpp compilation.... which shouldn't happen"
#endif
#include "amVK_Instance.hh"

/** 
 * This file was created on Last week of JUNE 2021     ;)        [Now its SEPT 23, 2021,   v0.0.1 (first GITHUB)]
 *    A-WRINKLE: if you are inspecting this file, make sure, you use    'function collapse'   option
 * 
 * APRIL 10, 2022,    v0.0.4 (I am just trynna make it look like standard Code or SMTH like that....)
 * APRIL 21, 2022,    v0.0.5a (IDK, what I am doing anymore.... GSoC is gone for me....)
 */

VkInstance amVK_InstanceMK2::Create_VkInstance(void) {
    _LOG("amVK_InstanceMK2::Create_VkInstance");

    if (s_Instance != nullptr) { 
        amVK_LOG_EX("amVK_IN::s_Instance isn't nullptr....");
        return nullptr;
    }


    // ----------- Extensions for vkCreateInstance ------------
    _LOG_LOOP("Enabled Instance Extensions:- ", i, ICS.enabled_iExts.size(), ICS.enabled_iExts[i]);


    // ----------- ValidationLayers for vkCreateInstance ------------
    VkValidationFeaturesEXT validationFeatures = {VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT, nullptr, 1, &ICS.GLSL_debug_printf_EXT};
    if (ICS.enableDebugLayers_LunarG) {
        amVK_InstanceMK2::enum_ValLayers();

        add_ValLayer("VK_LAYER_KHRONOS_validation");
        the_info.enabledLayerCount = static_cast<uint32_t>(ICS.enabled_vLayers.size());
        the_info.ppEnabledLayerNames = ICS.enabled_vLayers.data;
        _LOG_LOOP("Enabled Validation Layers:- ", i, ICS.enabled_vLayers.size(), ICS.enabled_vLayers[i]);

        /** \see ICS.GLSL_debug_printf_EXT */
        amVK_pNext_Search(VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT, the_info.pNext, _pNext);
        if (_pNext == nullptr) {
            the_info.pNext = &validationFeatures;
        }
    }


    // ----------- Actually Create the VkInstance ------------
    VkResult res = vkCreateInstance(
        &the_info, nullptr, 
        &s_Instance
    );
    if (res != VK_SUCCESS) {amVK_LOG_EX(amVK_Utils::vulkan_result_msg(res)); return nullptr;}

    _LOG0("VkInstance Created!");
    return s_Instance;
}



bool amVK_InstanceMK2::Destroy_VkInstance(void) {
    if (s_Instance == nullptr) {
        return false;
    }

    #ifndef amVK_RELEASE
    if (ICS.s_DebugKonsument1) {
        PFN_vkDestroyDebugReportCallbackEXT _func_ = VK_NULL_HANDLE;
        _func_ = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(s_Instance, "vkDestroyDebugReportCallbackEXT");

        _func_(s_Instance, ICS.s_DebugKonsument1, nullptr);

        _LOG0("[amVK_CX::destroy_Instance]  vkDestroyDebugReportCallbackEXT");
    }
    #endif
    
    _LOG0("[amVK_CX::destroy_Instance]  vKDestroyInstance");
    vkDestroyInstance(s_Instance, nullptr);
    s_Instance = nullptr;
    return true;
}








/**
    ██╗     ███████╗██╗   ██╗███████╗██╗         ██████╗ 
    ██║     ██╔════╝██║   ██║██╔════╝██║         ╚════██╗
    ██║     █████╗  ██║   ██║█████╗  ██║          █████╔╝
    ██║     ██╔══╝  ╚██╗ ██╔╝██╔══╝  ██║         ██╔═══╝ 
    ███████╗███████╗ ╚████╔╝ ███████╗███████╗    ███████╗
    ╚══════╝╚══════╝  ╚═══╝  ╚══════╝╚══════╝    ╚══════╝

  * |-----------------------------------------|
  *    - FORBIDDEN STUFFS [CreateInstance] -
  * |-----------------------------------------|
  * Vulkan Extensions: https://www.khronos.org/registry/vulkan/ 
  * Validation Layers by LunarG
*/

// This is here cz, this one is the simplest form of   enum_InstanceExts
void amVK_InstanceMK2::enum_ValLayers(void) {
    _LOG("amVK_CX::enum_ValLayers");

    if (ICS.vLayerP.n == 0) {
        uint32_t n;
        vkEnumerateInstanceLayerProperties(&n, NULL);

        ICS.vLayerP.data = static_cast<VkLayerProperties *> (calloc(n, 1 + sizeof(VkLayerProperties)));
        ICS.vLayerP.n = n;
        ICS.isEnabled_vLayer.data = reinterpret_cast<bool *> (ICS.vLayerP.data + n);
        ICS.isEnabled_vLayer.n = n;

        vkEnumerateInstanceLayerProperties(&n, ICS.vLayerP.data);
    }

    _LOG_LOOP("All Available Layers:- ", i, ICS.vLayerP.n, ICS.vLayerP[i].layerName);
    //Also see validationLayers_LunarG in FORBIDEN VARIABLES Section
}

bool amVK_InstanceMK2::enum_InstanceExts(bool do_log, VkExtensionProperties **pointer_iep, int *pointer_iec) {
    _LOG("amVK_InstanceMK2::enum_InstanceExts");

    // ----------- Get Instance Extensions anyway ------------
    uint32_t n;
    vkEnumerateInstanceExtensionProperties(nullptr, &n, nullptr);
    _LOG(n << " Instance extensions supported\n");

    ICS.IEP.data = static_cast<VkExtensionProperties *> (calloc(n, 1 + sizeof(VkExtensionProperties)));   
    ICS.IEP.n = n;    
    ICS.isEnabled_iExt.data = reinterpret_cast<bool *> (ICS.IEP.data + n);
    ICS.isEnabled_iExt.n = n;

    VkResult res = vkEnumerateInstanceExtensionProperties(nullptr, &n, ICS.IEP.data);
    if (res != VK_SUCCESS) {
        amVK_LOG_EX("vkEnumerateInstanceExtensionProperties() Failed.... \nVulkan Result Message:- " << amVK_Utils::vulkan_result_msg(res) << amVK::endl);
        return false;
    }
 
    // ----------- LOG/ENUMERATE (to CommandLine) ------------
    if (do_log) { _LOG_LOOP("All the Instance Extensions:- ", i, ICS.IEP.n, ICS.IEP[i].extensionName); }

    if (pointer_iep != nullptr)             {   *(pointer_iep) = ICS.IEP.data; }
    if (pointer_iec != nullptr)             {   *(pointer_iec) = ICS.IEP.n; }

    return true;
}

bool amVK_InstanceMK2::filter_SurfaceExts(void) {
    _LOG("amVK_InstanceMK2::filter_SurfaceExts");

    if (ICS.IEP.n == 0) {
        amVK_LOG_EX("Call enum_InstanceExts() before calling this, and make sure that function worked OK....");
        return false;
    }

    // -------- Some Stuffs --------
    typedef struct vulkan_exts_must_platform_specific {
        bool        KHR_surface;
    #if defined(amVK_BUILD_WIN32)
        bool        KHR_win32_surface;
    #elif defined(amVK_BUILD_X11)
        bool        KHR_xcb_surface;    //Prefer This over That
        bool        KHR_xlib_surface;
    #elif defined(amVK_BUILD_WAYLAND)
        bool        KHR_wayland_surface;
    #elif defined(amVK_BUILD_COCOA)
        bool        EXT_metal_surface;  //Prefer This over That
        bool        MVK_macos_surface;
    #endif
    } T_flagsSurfaceExts;

    static T_flagsSurfaceExts flagsSurfaceExts;

    static const char *xd[] = {   //Should be Aligned with as T_surfaceExts are
        "VK_KHR_surface",
    #if defined(amVK_BUILD_WIN32)
        "VK_KHR_win32_surface"
    #elif defined(amVK_BUILD_X11)
        "VK_KHR_xcb_surface",         //Prefer This over That
        "VK_KHR_xlib_surface"
    #elif defined(amVK_BUILD_WAYLAND)
        "VK_KHR_wayland_surface"
    #elif defined(amVK_BUILD_COCOA)
        "VK_EXT_metal_surface",       //Prefer This over That
        "VK_MVK_macos_surface"
    #endif
    };




    // -------- Check and Mark available SurfaceExts --------
    uint32_t xd_size = sizeof(xd) / sizeof(char *);
    amVK_ArrayDYN<VkExtensionProperties> tmp_surface_ep(xd_size);

    bool *flagsSurfaceExtsPtr = reinterpret_cast<bool *> (&(flagsSurfaceExts));     //flagsSurfaceExts is a STRUCT... we dont know how many BOOL i'll end up having... so we take mem_address and use like array
    for (uint32_t i = 0; i < xd_size; i++) {
        for (uint32_t j = 0; j < ICS.IEP.n; j++) {
            if (strcmp(ICS.IEP[j].extensionName, xd[i]) == 0) {
                *(flagsSurfaceExtsPtr + i) = true;
                tmp_surface_ep[i] = ICS.IEP[j];      //ICS.IEP[j] == xd[i]
            }
        }
    }



    /**
        ╻ ╻   ┏━┓╺┳╸╻ ╻┏━╸┏━╸┏━┓
        ╺╋╸   ┗━┓ ┃ ┃ ┃┣╸ ┣╸ ┗━┓   that you need to care about
        ╹ ╹   ┗━┛ ╹ ┗━┛╹  ╹  ┗━┛
    */
    //ICS._req_surface_ep.data = new VkExtensionProperties[2];   //For now we know this will be 2
    /** \see _ICS._req_surface_ep */

    //-------- Filter out the needed 2 Surface Extensions --------
#if defined(amVK_BUILD_WIN32)
    if (!flagsSurfaceExts.KHR_surface || !flagsSurfaceExts.KHR_win32_surface) {
        amVK_LOG_EX("vkEnumerateInstanceExtensionProperties didn't report [VK_KHR_surface] or [VK_KHR_win32_surface] as available");
        return false;
    }
    else {
        ICS._req_surface_ep[0] = tmp_surface_ep[0];
        ICS._req_surface_ep[1] = tmp_surface_ep[1];
    }
#elif defined(amVK_BUILD_X11)
    if (!flagsSurfaceExts.KHR_surface || !flagsSurfaceExts.KHR_xlib_surface || !flagsSurfaceExts.KHR_xcb_surface) {
        amVK_LOG_EX("vkEnumerateInstanceExtensionProperties didn't report [VK_KHR_surface] or [VK_KHR_xlib_surface] as available");
        return false;
    }
    else if (flagsSurfaceExts.KHR_xcb_surface) {
        ICS._req_surface_ep[0] = tmp_surface_ep[0];
        ICS._req_surface_ep[1] = tmp_surface_ep[1];
    }
    else {
        ICS._req_surface_ep[0] = tmp_surface_ep[0];
        ICS._req_surface_ep[1] = tmp_surface_ep[2];
    }
#elif defined(amVK_BUILD_WAYLAND)
    if (!flagsSurfaceExts.KHR_surface || !flagsSurfaceExts.KHR_wayland_surfacee) {
        amVK_LOG_EX("vkEnumerateInstanceExtensionProperties didn't report [VK_KHR_surface] or [VK_KHR_wayland_surface] as available");
        return false;
    }
    else {
        ICS._req_surface_ep[0] = tmp_surface_ep[0];
        ICS._req_surface_ep[1] = tmp_surface_ep[1];
    }
#elif defined(amVK_BUILD_COCOA)
    if (!flagsSurfaceExts.KHR_surface || !flagsSurfaceExts.MVK_macos_surface || !flagsSurfaceExts.EXT_metal_surface) {
        amVK_LOG_EX("vkEnumerateInstanceExtensionProperties didn't report [VK_KHR_surface] or [VK_MVK_macos_surface] as available");
        return false;
    }
    else if (flagsSurfaceExts.EXT_metal_surface) {
        ICS._req_surface_ep[0] = tmp_surface_ep[0];
        ICS._req_surface_ep[1] = tmp_surface_ep[1];
    }
    else {
        ICS._req_surface_ep[0] = tmp_surface_ep[0];
        ICS._req_surface_ep[1] = tmp_surface_ep[2];
    }
#endif

    _LOG_LOOP("SurfaceExts:- ", i, 2, ICS._req_surface_ep[i].extensionName);
    return true;
}

bool amVK_InstanceMK2::add_InstanceExt(const char *extName) {
    _LOG("amVK_InstanceMK2::add_InstanceExt");

    if (ICS.IEP.data == nullptr) {
        enum_InstanceExts();
    }

    uint32_t index = amVK_InstanceMK2::iExtName_to_index(extName);
    if (index == 0xFFFFFFFF) {  //Error Checking
        amVK_LOG_EX("Instance EXT: " << extName << " isn't sup. on this PC/Device/Computer/System whatever....");
        return false;
    }

    else if (ICS.isEnabled_iExt[index])  {
        amVK_LOG_EX(extName << " is already added to \u0027ICS.enabled_iExts\u0027 list. Before RELEASE, make sure you see the default \u0027ICS.enabled_iExts\u0027  that amVK adds & also don't try to add something twice");
        return true; //cz already added before and reported LOG
    } 

    else {
        if (ICS.enabled_iExts.should_resize()) {ICS.enabled_iExts.resize(2);}
        amVK_ArrayDYN_PUSH_BACK(ICS.enabled_iExts) = ICS.IEP[index].extensionName;    //don't wanna depend on extName, that might be on stack
        ICS.isEnabled_iExt[index] = true;
        return true;
    }
}

bool amVK_InstanceMK2::add_ValLayer(const char *vLayerName) {
    _LOG("amVK_InstanceMK2::add_ValLayer");

    uint32_t index = vLayerName_to_index(vLayerName);
    if (index == 0xFFFFFFFF) {
        amVK_LOG_EX(vLayerName << " is not Available on this PC/Device/Computer/System whatever..... [Also check the spelling]");
        return false;
    }

    else if (ICS.isEnabled_vLayer[index])  {
        amVK_LOG_EX(vLayerName << " is already added to \u0027ICS.enabled_vLayers\u0027 list. Before RELEASE, make sure you see the default \u0027_vLayers\0027 that amVK adds & also don't try to add something twice");
        return true; //cz already added before and reported LOG
    } 

    else {
        if (ICS.enabled_vLayers.should_resize()) {ICS.enabled_vLayers.resize(2);}
        amVK_ArrayDYN_PUSH_BACK(ICS.enabled_vLayers) = ICS.vLayerP[index].layerName;    //don't wanna depend on extName, that might be on stack
        ICS.isEnabled_vLayer[index] = true;
        return true;
    }
}

uint32_t amVK_InstanceMK2::iExtName_to_index(const char *iExtName) {
    for (uint32_t i = 0; i < ICS.IEP.n; i++) {
        if (strcmp(iExtName, ICS.IEP[i].extensionName) == 0) {
            return i;
        }
    }
    return 0xFFFFFFFF;
}
uint32_t amVK_InstanceMK2::vLayerName_to_index(const char *vLayerName) {
    for (uint32_t i = 0; i < ICS.vLayerP.n; i++) {
        if (strcmp(vLayerName, ICS.vLayerP[i].layerName) == 0) {
            return i;
        }
    }
    return 0xFFFFFFFF;
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
bool amVK_InstanceMK2::load_PD_info(bool force_load, bool auto_choose) {
    _LOG("amVK_InstanceMK2::load_PD_info");

    if (SPD.list != nullptr) {
        _LOG("Already SPD Info loaded Once....");

        if (force_load) {
            _LOG("force_load param enabled");
            free(SPD.list); SPD = {};     // Zero Initialize
            goto load_PD;
        }
        else {
            _LOG0("PD info loaded Once.... and force_load isn't called");
        }
        return &SPD;
    }

    load_PD:
    {
        if (!HEART || !s_Instance) {amVK_LOG_EX("amVK_CX::CreateInstance() has to be called before. [or set amVK_IN::instance & amVK_CX::heart]"); return false;}
        bool any_PD_sup = amVK_InstanceMK2::enum_PhysicalDevs();
        if (!any_PD_sup) {
            amVK_LOG_EX("Vulkan not supported by any Available Physical Device");
            return false;
        }
        amVK_InstanceMK2::enum_PD_qFamilies();
        if(auto_choose) {this->auto_choosePD();}    //Does Benchmark TOO!

        _LOG0("Loaded PD_info \n");
        return &SPD;
    }
}

bool amVK_InstanceMK2::enum_PhysicalDevs() {
    _LOG("amVK_InstanceMK2::enum_PhysicalDevs");

    if (SPD.list != nullptr) free(SPD.list);      //By Default force_load

    vkEnumeratePhysicalDevices(s_Instance, &SPD.n, nullptr);
    if (SPD.n == 0) {
        amVK_LOG_EX("Vulkan Loader failed to find GPUs with Vulkan support!");
        return false;
        
    } else if (SPD.n > 1000000) {
        amVK_LOG_EX("MORE THAN A  Million GPUs? Are you REALLY SURE?  [return-ing false]");
        return false;

    } else if (SPD.n > 1000) {
        amVK_LOG_EX("MORE THAN A THOUSAND GPUs? Are you REALLY SURE?  [Proceeding anyway]");
    }

    // ----------- THE GRAND MALLOC ------------
    uint32_t PD_n = SPD.n;
    void *test = malloc((PD_n * sizeof(VkPhysicalDevice))
                       +(PD_n * sizeof(VkPhysicalDeviceProperties))
                       +(PD_n * sizeof(VkPhysicalDeviceFeatures))
                       +(PD_n * sizeof(amVK_Array<VkQueueFamilyProperties>))    //sizeof this one will be 16
                       +(PD_n * sizeof(VkPhysicalDeviceMemoryProperties))       //used in ImagesMK2
                       +(PD_n * sizeof(bool))                                   //bool to store 'isUsed' list
                       +(PD_n * sizeof(uint32_t))                               //benchMarks
                       +(PD_n * sizeof(uint32_t)));                             //index_sortedByMark
    

    SPD.list = static_cast<VkPhysicalDevice *> (test);
    SPD.props = reinterpret_cast<VkPhysicalDeviceProperties *> (SPD.list + PD_n);
    SPD.features = reinterpret_cast<VkPhysicalDeviceFeatures *> (SPD.props + PD_n);
    SPD.qFamily_lists = reinterpret_cast< amVK_Array<VkQueueFamilyProperties>* > (SPD.features + PD_n);

    SPD.mem_props = reinterpret_cast<VkPhysicalDeviceMemoryProperties *> (SPD.qFamily_lists + PD_n);

    SPD.isUsed = reinterpret_cast<bool *> (SPD.mem_props + PD_n);
    SPD.benchMarks = reinterpret_cast<uint32_t *> (SPD.isUsed + PD_n);
    SPD.index_sortedByMark = reinterpret_cast<uint32_t *> (SPD.benchMarks + PD_n);

    /**
     * ----------- NULL / nullptr / 0 (ZERO) ------------
    for (uint32_t i = 0; i < PD_n; i++) {   
        PD.qFamily_lists[i].data = 0;     //used in enum_PD_qFamilies()
        PD.isUsed[i] = false;             //used in auto_choosePD()
    }
    */
    memset(SPD.qFamily_lists, 0, PD_n * sizeof(amVK_Array<VkQueueFamilyProperties>));
    memset(SPD.isUsed,        0, PD_n * sizeof(bool));


    // ----------- Get Stuffs ------------
    vkEnumeratePhysicalDevices(s_Instance, &PD_n, SPD.list);
    for (uint32_t i = 0; i < PD_n; i++) {
        vkGetPhysicalDeviceProperties(SPD.list[i], (SPD.props+i));
        vkGetPhysicalDeviceFeatures(SPD.list[i], (SPD.features+i));
        
        vkGetPhysicalDeviceMemoryProperties(SPD.list[i], (SPD.mem_props+i));
    }

    _LOG_LOOP0("All the Physical Devices:- ", i, SPD.n, SPD.props[i].deviceName);
    return true;
}

bool amVK_InstanceMK2::enum_PD_qFamilies() {
    _LOG("amVK_InstanceMK2::enum_PD_qFamilies");

    if (SPD.list == nullptr) {
        amVK_LOG_EX("call amVK_InstanceMK2::enum_PhysicalDevs() first    [trying to solve, by calling that]");
        amVK_InstanceMK2::enum_PhysicalDevs();
    }

    // ----------- GET HOW MUCH TO MALLOC ------------
    uint32_t n = 0;
    uint32_t tmp_n = 0;
    for (uint32_t i = 0, lim = SPD.n; i < lim; i++) {
        vkGetPhysicalDeviceQueueFamilyProperties(SPD.list[i], &n, nullptr);
        tmp_n += n;
    }

    //----------- By Default force_load   &  The Malloc ------------
    if (SPD.qFamily_lists[0].data != nullptr) {free(SPD.qFamily_lists[0].data);}
        SPD.qFamily_lists[0].data  = static_cast<VkQueueFamilyProperties *>    ( malloc(tmp_n * sizeof(VkQueueFamilyProperties)) );

    // ----------- Use Malloc & Get qFamily_props ------------
    n = 0;
    tmp_n = 0;
    for (uint32_t i = 0; i < SPD.n; i++) {
        vkGetPhysicalDeviceQueueFamilyProperties(SPD.list[i], &n, nullptr);

        SPD.qFamily_lists[i].data = reinterpret_cast<VkQueueFamilyProperties *> (SPD.qFamily_lists[0].data + tmp_n);
        vkGetPhysicalDeviceQueueFamilyProperties(SPD.list[i], &n, SPD.qFamily_lists[i].data);

        SPD.qFamily_lists[i].n = n;
        tmp_n += n;
    }
    return true;
}

void amVK_InstanceMK2::benchMark_PD(void) {
    _LOG("amVK_InstanceMK2::benchMark_PD");

    if (SPD.list == nullptr) {
        amVK_LOG_EX("call amVK_CX::enum_PhysicalDevs() & enum_PD_qFamilies() first      [trying to solve, by calling those]");
        amVK_InstanceMK2::enum_PhysicalDevs();
        amVK_InstanceMK2::enum_PD_qFamilies();
    }

    // ----------- THE GREAT TIME TRAVELING BENCHMARK ------------
    for (uint32_t i = 0; i < SPD.n; i++) {
        _LOG("BenchMarking " << SPD.props[i].deviceName);
        uint32_t mk = 0;
        VkPhysicalDeviceFeatures features = SPD.features[i];
        VkPhysicalDeviceProperties *props = &SPD.props[i];

        // ----------- PD Properties [MAIN STUFFS] ------------
        if (props->deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) mk += 2021;
        mk += props->limits.maxImageDimension2D;
    
        // ----------- PD Features ------------
        if (features.shaderStorageImageExtendedFormats) mk += 15360;    //16K Width   cz    mk += maxImageDimension2D
        if (features.shaderFloat64) mk += 4;
        if (features.shaderInt64) mk += 2;
        if (features.shaderInt16) mk += 2;
        if (features.tessellationShader) mk += 8;
        if (features.geometryShader) mk += 6;
        if (features.shaderTessellationAndGeometryPointSize) mk += 1;
        if (!features.sampleRateShading) mk -= 8;

        // ----------- PD Properties Extended [TODO] ------------
        // ----------- In this way 3060TI & 3070 will most Likely hit the SAME mk.... so TODO ------------
        SPD.benchMarks[i] = mk;
    }

    /** ----------- SORTING ------------ [TODO: Test mergeSort, Impl memcpy, don't include <string> in amVK_utils] */
    for (uint32_t i = 0; i < SPD.n; i++) {SPD.index_sortedByMark[i] = i;}
    bool sort_success = amVK_Utils::mergeSort<uint32_t> (0, SPD.n-1, SPD.benchMarks, SPD.index_sortedByMark);
}

bool amVK_InstanceMK2::auto_choosePD(void) {
    _LOG("amVK_InstanceMK2::auto_choosePD");

    benchMark_PD(); //even if already benchmarked ONCE

    // ----------- FIND NEXT notUsed PD ------------
    for (uint32_t i = 0; i < SPD.n; i++) {
        if (!SPD.isUsed[i]) {SPD.chozen = SPD.list[SPD.index_sortedByMark[i]];    SPD.chozen_index = i;  break;}
    }

    // ----------- USE STRONGEST PD for RECREATION ------------
    if (SPD.chozen == nullptr) {
        SPD.chozen = SPD.list[SPD.index_sortedByMark[0]];
        SPD.chozen_index = 0;
        amVK_LOG_EX("reUsing \u0027"  << SPD.props[SPD.index_sortedByMark[0]].deviceName << "\u0027 (PhysicalDevice) for VkDevice Creation");
        return false;
    } else {
        return true;
    }
}









#ifndef amVK_RELEASE
static bool _registered1 = false;

// And this is the callback that the validator will call
VkBool32 _amVK_DebugKonsument1(VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objectType,
    uint64_t object, 
    size_t location, 
    int32_t messageCode,
    const char* pLayerPrefix,
    const char* pMessage, 
    void* pUserData)
{
    //if(flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
    //{
    //    amVK_LOG("debugPrintfEXT: " << pMessage);
    //}
    amVK_LOG("[amVK] debugPrintfEXT: " << pMessage);
 
    return false;
}
#endif      // Debug Part, amVK_RELEASE

VkDebugReportCallbackEXT amVK_InstanceMK2::Set_DebugKonsument(PFN_vkDebugReportCallbackEXT DebugKonsument) {
#ifndef amVK_RELEASE
    _LOG("amVK_InstanceMK2::Set_DebugKonsument");
    
    if(ICS.enableDebugLayers_LunarG) {
        VkDebugReportCallbackEXT Handle = nullptr;
        VkDebugReportCallbackEXT  *pH   = &Handle;

        if (DebugKonsument == nullptr) {
            DebugKonsument = _amVK_DebugKonsument1;
            pH = &ICS.s_DebugKonsument1;

            if (_registered1) {
                amVK_LOG_EX("param DebugKonsument == nullptr   &  the default _amVK_DebugKonsument1 is already registered");
                return nullptr;
            }
        }
    

        VkDebugReportCallbackCreateInfoEXT ci = {VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT, nullptr,
            VK_DEBUG_REPORT_INFORMATION_BIT_EXT,
            _amVK_DebugKonsument1, nullptr
        };
        
        PFN_vkCreateDebugReportCallbackEXT _func_ = VK_NULL_HANDLE;
        _func_ = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(s_Instance, "vkCreateDebugReportCallbackEXT");


        // Create the callback handle
        VkResult res = _func_(s_Instance, &ci, nullptr, &Handle);
        if (res != VK_SUCCESS) {amVK_LOG_EX(amVK_Utils::vulkan_result_msg(res)); return nullptr;}
        _LOG0("vkCreateDebugReportCallbackEXT...");

        return Handle;
    }
#endif
}