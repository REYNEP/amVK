#ifdef amVK_IN_NOSTDLIB
    #error "You might have defined amVK_IN_NOSTDLIB in your makefile.... its getting defined for amVK_CX.cpp compilation.... which shouldn't happen"
#endif
#include "amVK_CX.hh"

/** 
 * This file was created on Last week of JUNE 2021     ;)        [Now its SEPT 23, 2021,   v0.0.1 (first GITHUB)]
 *    A-WRINKLE: if you are inspecting this file, make sure, you use    'function collapse'   option
 * 
 * APRIL 10, 2022,    v0.0.4 (I am just trynna make it look like standard Code or SMTH like that....)
 */

VkInstance amVK_CX::create_Instance(void) {
    _LOG("amVK_CX::create_Instance");

    if (amVK_IN::instance != nullptr) { 
        amVK_LOG_EX("amVK_IN::instance isn't nullptr....");
        return nullptr;
    }

    if (amVK_CX::vk_appInfo.engineVersion == NULL) {
        amVK_CX::set_VkApplicationInfo(nullptr);
    }


    // ----------- Extensions for vkCreateInstance ------------
    // Moved to CONSTRUCTOR....
    _LOG_LOOP("Enabled Instance Extensions:- ", i, m_enabled_iExts.size(), m_enabled_iExts[i]);


    // ----------- CreateInfo for vkCreateInstance ------------
    VkInstanceCreateInfo the_info{};
    the_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    the_info.pApplicationInfo = &(amVK_CX::vk_appInfo);
    //the_info.pNext = nullptr;  the_info.flags [KHR Says 'reserved for future'] [No need to care about these 2 for now]
    the_info.enabledExtensionCount = static_cast<uint32_t>(m_enabled_iExts.size());
    the_info.ppEnabledExtensionNames = m_enabled_iExts.data;


    // ----------- ValidationLayers for vkCreateInstance ------------
    if (m_enableDebugLayers_LunarG) {
        amVK_CX::enum_ValLayers();
        add_ValLayer("VK_LAYER_KHRONOS_validation");
        the_info.enabledLayerCount = static_cast<uint32_t>(m_enabled_vLayers.size());
        the_info.ppEnabledLayerNames = m_enabled_vLayers.data;
        _LOG_LOOP("Enabled Validation Layers:- ", i, m_enabled_vLayers.size(), m_enabled_vLayers[i]);
    }


    // ----------- Actually Create the VkInstance ------------
    VkResult res = vkCreateInstance(    // You will Occassionally see this general pattern of Vulkan-Function Calls
        &the_info,                      //VkInstanceCreateInfo *pCreateInfo;  [safe to pass in variable that we are gonna destroy later s:(VkInstanceCreateInfo ici = *pCreateInfo;)]
        nullptr,                        //VkAllocationCallbacks *pAllocator
        &amVK_IN::instance              //VkInstance *pInstance               [s:(*pInstance = created_instance;) 's' means line of code from GITHUB_VULKAN_LOADER]
    );
    if (res != VK_SUCCESS) {amVK_LOG_EX(amVK_Utils::vulkan_result_msg(res)); return nullptr;}


    /** 
     * [Mar 4th Week I Think, 3 Months Ago from now, Check again ASAP]
     * BLENDER's CURRENT (MARCH 2021) VULKAN STATE, uses vkCreateInstance inside initializeDrawingContext() from what I have seen, 
     * I believe that is a Function called for Every GHOST_Context created    [so caution is, vkCreateInstance might be called multiple times]
     * https://developer.blender.org/T68990 & https://developer.blender.org/P1590
     */

    _LOG0("VkInstance Created! \n" << amVK::endl);
    return amVK_IN::instance;
}



bool amVK_CX::destroy_Instance(void) {
    if (amVK_IN::instance == nullptr) {
        return false;
    }
    
    _LOG("amVK_CX::destroy_Instance");
    vkDestroyInstance(amVK_IN::instance, nullptr);
    amVK_IN::instance = nullptr;
    return true;
}



/** \param appInfo: default value is nullptr. */
void amVK_CX::set_VkApplicationInfo(VkApplicationInfo *appInfo) {
    _LOG("amVK_CX::set_VkApplication");

    // ----------- Muhaha, we use OUR Own stuffs.... if nullptr ------------
    if (appInfo == nullptr) {
        _LOG("(amVK Dev Blubbering: muHahahaha, you didn't think about VkApplicationInfo, I am definitely gonna use that for Advertising purposes)");

        VkApplicationInfo newAppInfo{};
        newAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        newAppInfo.pApplicationName = "RealTimeRendering";
        newAppInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
        newAppInfo.apiVersion =  VK_MAKE_VERSION(1, 2, 0);  //VK_API_VERSION_1_x [x is minor], [patch] is ignored... 
        /** 
         * Read the official Specs [This is supposed to be the Highest ur app uses, Even we said 1_2, a device that supports only 1_0 can still Run ur APP in VK1_0 mode... [just dont use 1_1 extensions on that DEVICE!]
         * But if you specify this as 1_1, then you should not call any VK1_2 Functionality, And VK1_2 Drivers/Devices will run ur app in VK1_1 mode....
         * [kh-reg-vk/specs/1.2-extensions/man/html/VkApplicationInfo.html#_description]
         * VKGuideBook Was Wrong (or just.... not right enough I guess) (Vulkan Guide (2016 - VK1.0.22) said:- should be absolute Minimum Vulkan that your App needs)
         */
        appInfo = &newAppInfo;
    }
    appInfo->pEngineName = "amVK";
    appInfo->engineVersion = VK_MAKE_VERSION(0, 0, 4);
    _LOG0("amVK Engine Version 0.0.4");
    
    amVK_CX::vk_appInfo = *(appInfo);
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
void amVK_CX::enum_ValLayers(void) {
    _LOG("amVK_CX::enum_ValLayers");

    if (m_vLayerP.n == 0) {
        uint32_t n;
        vkEnumerateInstanceLayerProperties(&n, NULL);

        m_vLayerP.data = static_cast<VkLayerProperties *> (calloc(n, 1 + sizeof(VkLayerProperties)));
        m_vLayerP.n = n;
        m_isEnabled_vLayer.data = reinterpret_cast<bool *> (m_vLayerP.data + n);
        m_isEnabled_vLayer.n = n;

        vkEnumerateInstanceLayerProperties(&n, m_vLayerP.data);
    }

    _LOG_LOOP("All Available Layers:- ", i, m_vLayerP.n, m_vLayerP[i].layerName);
    //Also see validationLayers_LunarG in FORBIDEN VARIABLES Section
}

bool amVK_CX::enum_InstanceExts(bool do_log, VkExtensionProperties **pointer_iep, int *pointer_iec) {
    _LOG("amVK_CX::enum_InstanceExts");

    // ----------- Get Instance Extensions anyway ------------
    uint32_t n;
    vkEnumerateInstanceExtensionProperties(nullptr, &n, nullptr);
    _LOG(n << " Instance extensions supported\n");

    m_IEP.data = static_cast<VkExtensionProperties *> (calloc(n, 1 + sizeof(VkExtensionProperties)));   
    m_IEP.n = n;    
    m_isEnabled_iExt.data = reinterpret_cast<bool *> (m_IEP.data + n);
    m_isEnabled_iExt.n = n;

    VkResult res = vkEnumerateInstanceExtensionProperties(nullptr, &n, m_IEP.data);
    if (res != VK_SUCCESS) {
        amVK_LOG_EX("vkEnumerateInstanceExtensionProperties() Failed.... \nVulkan Result Message:- " << amVK_Utils::vulkan_result_msg(res) << amVK::endl);
        return false;
    }
 
    // ----------- LOG/ENUMERATE (to CommandLine) ------------
    if (do_log) { _LOG_LOOP("All the Instance Extensions:- ", i, m_IEP.n, m_IEP[i].extensionName); }

    if (pointer_iep != nullptr)             {   *(pointer_iep) = m_IEP.data; }
    if (pointer_iec != nullptr)             {   *(pointer_iec) = m_IEP.n; }

    return true;
}

bool amVK_CX::filter_SurfaceExts(void) {
    _LOG("amVK_CX::filter_SurfaceExts");

    if (m_IEP.n == 0) {
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

    static char *xd[] = {   //Should be Aligned with as T_surfaceExts are
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
        for (uint32_t j = 0; j < m_IEP.n; j++) {
            if (strcmp(m_IEP[j].extensionName, xd[i]) == 0) {
                *(flagsSurfaceExtsPtr + i) = true;
                tmp_surface_ep[i] = m_IEP[j];      //m_IEP[j] == xd[i]
            }
        }
    }



    /**
        ╻ ╻   ┏━┓╺┳╸╻ ╻┏━╸┏━╸┏━┓
        ╺╋╸   ┗━┓ ┃ ┃ ┃┣╸ ┣╸ ┗━┓   that you need to care about
        ╹ ╹   ┗━┛ ╹ ┗━┛╹  ╹  ┗━┛
    */
    //m_req_surface_ep.data = new VkExtensionProperties[2];   //For now we know this will be 2
    /** \see _m_req_surface_ep */

    //-------- Filter out the needed 2 Surface Extensions --------
#if defined(amVK_BUILD_WIN32)
    if (!flagsSurfaceExts.KHR_surface || !flagsSurfaceExts.KHR_win32_surface) {
        amVK_LOG_EX("vkEnumerateInstanceExtensionProperties didn't report [VK_KHR_surface] or [VK_KHR_win32_surface] as available");
        return false;
    }
    else {
        m_req_surface_ep[0] = tmp_surface_ep[0];
        m_req_surface_ep[1] = tmp_surface_ep[1];
    }
#elif defined(amVK_BUILD_X11)
    if (!flagsSurfaceExts.KHR_surface || !flagsSurfaceExts.KHR_xlib_surface) {
        amVK_LOG_EX("vkEnumerateInstanceExtensionProperties didn't report [VK_KHR_surface] or [VK_KHR_xlib_surface] as available");
        return false;
    }
    else if (flagsSurfaceExts.KHR_xcd_surface) {
        m_req_surface_ep[0] = tmp_surface_ep[0];
        m_req_surface_ep[1] = tmp_surface_ep[1];
    }
    else {
        m_req_surface_ep[0] = tmp_surface_ep[0];
        m_req_surface_ep[1] = tmp_surface_ep[2];
    }
#elif defined(amVK_BUILD_WAYLAND)
    if (!flagsSurfaceExts.KHR_surface || !flagsSurfaceExts.KHR_wayland_surfacee) {
        amVK_LOG_EX("vkEnumerateInstanceExtensionProperties didn't report [VK_KHR_surface] or [VK_KHR_wayland_surface] as available");
        return false;
    }
    else {
        m_req_surface_ep[0] = tmp_surface_ep[0];
        m_req_surface_ep[1] = tmp_surface_ep[1];
    }
#elif defined(amVK_BUILD_COCOA)
    if (!flagsSurfaceExts.KHR_surface || !flagsSurfaceExts.MVK_macos_surface) {
        amVK_LOG_EX("vkEnumerateInstanceExtensionProperties didn't report [VK_KHR_surface] or [VK_MVK_macos_surface] as available");
        return false;
    }
    else if (flagsSurfaceExts.EXT_metal_surface) {
        m_req_surface_ep[0] = tmp_surface_ep[0];
        m_req_surface_ep[1] = tmp_surface_ep[1];
    }
    else {
        m_req_surface_ep[0] = tmp_surface_ep[0];
        m_req_surface_ep[1] = tmp_surface_ep[2];
    }
#endif

    _LOG_LOOP("SurfaceExts:- ", i, 2, m_req_surface_ep[i].extensionName);
    return true;
}

bool amVK_CX::add_InstanceExt(char *extName) {
    _LOG("amVK_CX::add_InstanceExt");

    if (m_IEP.data == nullptr) {
        enum_InstanceExts();
    }

    uint32_t index = amVK_CX::iExtName_to_index(extName);
    if (index == 0xFFFFFFFF) {  //Error Checking
        amVK_LOG_EX("Instance EXT: " << extName << " isn't sup. on this PC/Device/Computer/System whatever....");
        return false;
    }

    else if (m_isEnabled_iExt[index])  {
        amVK_LOG_EX(extName << " is already added to \u0027m_enabled_iExts\u0027 list. Before RELEASE, make sure you see the default \u0027m_enabled_iExts\u0027  that amVK adds & also don't try to add something twice");
        return true; //cz already added before and reported LOG
    } 

    else {
        if (m_enabled_iExts.should_resize()) {m_enabled_iExts.resize(2);}
        amVK_ArrayDYN_PUSH_BACK(m_enabled_iExts) = m_IEP[index].extensionName;    //don't wanna depend on extName, that might be on stack
        m_isEnabled_iExt[index] = true;
        return true;
    }
}

bool amVK_CX::add_ValLayer(char *vLayerName) {
    _LOG("amVK_CX::add_ValLayer");

    uint32_t index = vLayerName_to_index(vLayerName);
    if (index == 0xFFFFFFFF) {
        amVK_LOG_EX(vLayerName << " is not Available on this PC/Device/Computer/System whatever..... [Also check the spelling]");
        return false;
    }

    else if (m_isEnabled_vLayer[index])  {
        amVK_LOG_EX(vLayerName << " is already added to \u0027m_enabled_vLayers\u0027 list. Before RELEASE, make sure you see the default \u0027_vLayers\0027 that amVK adds & also don't try to add something twice");
        return true; //cz already added before and reported LOG
    } 

    else {
        if (m_enabled_vLayers.should_resize()) {m_enabled_vLayers.resize(2);}
        amVK_ArrayDYN_PUSH_BACK(m_enabled_vLayers) = m_vLayerP[index].layerName;    //don't wanna depend on extName, that might be on stack
        m_isEnabled_vLayer[index] = true;
        return true;
    }
}

uint32_t amVK_CX::iExtName_to_index(char *iExtName) {
    for (uint32_t i = 0; i < m_IEP.n; i++) {
        if (strcmp(iExtName, m_IEP[i].extensionName) == 0) {
            return i;
        }
    }
    return 0xFFFFFFFF;
}
uint32_t amVK_CX::vLayerName_to_index(char *vLayerName) {
    for (uint32_t i = 0; i < m_vLayerP.n; i++) {
        if (strcmp(vLayerName, m_vLayerP[i].layerName) == 0) {
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
bool amVK_CX::load_PD_info(bool force_load, bool auto_choose) {
    _LOG("amVK_CX::load_PD_info");

    if (PD.list != nullptr) {
        _LOG("Already PD Info loaded Once....");

        if (force_load) {
            _LOG("force_load param enabled");
            free(PD.list); PD = {};     // Zero Initialize
            goto load_PD;
        }
        else {
            _LOG0("PD info loaded Once.... and force_load isn't called");
        }
        return &PD;
    }

    load_PD:
    {
        if (!HEART || !amVK_IN::instance) {amVK_LOG_EX("amVK_CX::CreateInstance() has to be called before. [or set amVK_IN::instance & amVK_CX::heart]"); return false;}
        bool any_PD_sup = amVK_CX::enum_PhysicalDevs();
        if (!any_PD_sup) {
            amVK_LOG_EX("Vulkan not supported by any Available Physical Device");
            return false;
        }
        amVK_CX::enum_PD_qFamilies();
        if(auto_choose) {this->auto_choosePD();}    //Does Benchmark TOO!

        _LOG0("Loaded PD_info \n");
        return &PD;
    }
}

bool amVK_CX::enum_PhysicalDevs() {
    _LOG("amVK_CX::enum_PhysicalDevs");

    if (PD.list != nullptr) free(PD.list);      //By Default force_load

    vkEnumeratePhysicalDevices(instance, &PD.n, nullptr);
    if (PD.n == 0) {
        amVK_LOG_EX("Vulkan Loader failed to find GPUs with Vulkan support!");
        return false;
    }  else if (PD.n > 1000) {
        amVK_LOG_EX("MORE THAN A THOUSAND GPUs? Are you REALLY SURE?");
    }

    // ----------- THE GRAND MALLOC ------------
    uint32_t PD_n = PD.n;
    void *test = malloc((PD_n * sizeof(VkPhysicalDevice))
                       +(PD_n * sizeof(VkPhysicalDeviceProperties))
                       +(PD_n * sizeof(VkPhysicalDeviceFeatures))
                       +(PD_n * sizeof(amVK_Array<VkQueueFamilyProperties>))    //sizeof this one will be 16
                       +(PD_n * sizeof(VkPhysicalDeviceMemoryProperties))       //used in ImagesMK2
                       +(PD_n * sizeof(bool))                                   //bool to store 'isUsed' list
                       +(PD_n * sizeof(uint32_t))                               //benchMarks
                       +(PD_n * sizeof(uint32_t)));                             //index_sortedByMark
    

    PD.list = static_cast<VkPhysicalDevice *> (test);
    PD.props = reinterpret_cast<VkPhysicalDeviceProperties *> (PD.list + PD_n);
    PD.features = reinterpret_cast<VkPhysicalDeviceFeatures *> (PD.props + PD_n);
    PD.qFamily_lists = reinterpret_cast< amVK_Array<VkQueueFamilyProperties>* > (PD.features + PD_n);

    PD.mem_props = reinterpret_cast<VkPhysicalDeviceMemoryProperties *> (PD.qFamily_lists + PD_n);

    PD.isUsed = reinterpret_cast<bool *> (PD.mem_props + PD_n);
    PD.benchMarks = reinterpret_cast<uint32_t *> (PD.isUsed + PD_n);
    PD.index_sortedByMark = reinterpret_cast<uint32_t *> (PD.benchMarks + PD_n);

    /**
     * ----------- NULL / nullptr / 0 (ZERO) ------------
    for (uint32_t i = 0; i < PD_n; i++) {   
        PD.qFamily_lists[i].data = 0;     //used in enum_PD_qFamilies()
        PD.isUsed[i] = false;             //used in auto_choosePD()
    }
    */
    memset(PD.qFamily_lists, 0, PD_n * sizeof(amVK_Array<VkQueueFamilyProperties>));
    memset(PD.isUsed,        0, PD_n * sizeof(bool));


    // ----------- Get Stuffs ------------
    vkEnumeratePhysicalDevices(instance, &PD_n, PD.list);
    for (uint32_t i = 0; i < PD_n; i++) {
        vkGetPhysicalDeviceProperties(PD.list[i], (PD.props+i));
        vkGetPhysicalDeviceFeatures(PD.list[i], (PD.features+i));
        
        vkGetPhysicalDeviceMemoryProperties(PD.list[i], (PD.mem_props+i));
    }

    _LOG_LOOP0("All the Physical Devices:- ", i, PD.n, PD.props[i].deviceName);
    return true;
}

bool amVK_CX::enum_PD_qFamilies() {
    _LOG("amVK_CX::enum_PD_qFamilies");

    if (PD.list == nullptr) {
        amVK_LOG_EX("call amVK_CX::enum_PhysicalDevs() first    [trying to solve, by calling that]");
        amVK_CX::enum_PhysicalDevs();
    }

    // ----------- GET HOW MUCH TO MALLOC ------------
    uint32_t n = 0;
    uint32_t tmp_n = 0;
    for (uint32_t i = 0, lim = PD.n; i < lim; i++) {
        vkGetPhysicalDeviceQueueFamilyProperties(PD.list[i], &n, nullptr);
        tmp_n += n;
    }

    //----------- By Default force_load   &  The Malloc ------------
    if (PD.qFamily_lists[0].data != nullptr) {free(PD.qFamily_lists[0].data);}
        PD.qFamily_lists[0].data  = static_cast<VkQueueFamilyProperties *>    ( malloc(tmp_n * sizeof(VkQueueFamilyProperties)) );

    // ----------- Use Malloc & Get qFamily_props ------------
    n = 0;
    tmp_n = 0;
    for (uint32_t i = 0; i < PD.n; i++) {
        vkGetPhysicalDeviceQueueFamilyProperties(PD.list[i], &n, nullptr);

        PD.qFamily_lists[i].data = reinterpret_cast<VkQueueFamilyProperties *> (PD.qFamily_lists[0].data + tmp_n);
        vkGetPhysicalDeviceQueueFamilyProperties(PD.list[i], &n, PD.qFamily_lists[i].data);

        PD.qFamily_lists[i].n = n;
        tmp_n += n;
    }
    return true;
}

void amVK_CX::benchMark_PD(void) {
    _LOG("amVK_CX::benchMark_PD");

    if (PD.list == nullptr) {
        amVK_LOG_EX("call amVK_CX::enum_PhysicalDevs() & enum_PD_qFamilies() first      [trying to solve, by calling those]");
        amVK_CX::enum_PhysicalDevs();
        amVK_CX::enum_PD_qFamilies();
    }

    // ----------- THE GREAT TIME TRAVELING BENCHMARK ------------
    for (uint32_t i = 0; i < PD.n; i++) {
        _LOG("BenchMarking " << PD.props[i].deviceName);
        uint32_t mk = 0;
        VkPhysicalDeviceFeatures features = PD.features[i];
        VkPhysicalDeviceProperties *props = &PD.props[i];

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
        PD.benchMarks[i] = mk;
    }

    /** ----------- SORTING ------------ [TODO: Test mergeSort, Impl memcpy, don't include <string> in amVK_utils] */
    for (uint32_t i = 0; i < PD.n; i++) {PD.index_sortedByMark[i] = i;}
    bool sort_success = amVK_Utils::mergeSort<uint32_t> (0, PD.n-1, PD.benchMarks, PD.index_sortedByMark);
}

bool amVK_CX::auto_choosePD(void) {
    _LOG("amVK_CX::auto_choosePD");

    benchMark_PD(); //even if already benchmarked ONCE

    // ----------- FIND NEXT notUsed PD ------------
    for (uint32_t i = 0; i < PD.n; i++) {
        if (!PD.isUsed[i]) {PD.chozen = PD.list[PD.index_sortedByMark[i]];    PD.chozen_index = i;  break;}
    }

    // ----------- USE STRONGEST PD for RECREATION ------------
    if (PD.chozen == nullptr) {
        PD.chozen = PD.list[PD.index_sortedByMark[0]];
        PD.chozen_index = 0;
        amVK_LOG_EX("reUsing \u0027"  << PD.props[PD.index_sortedByMark[0]].deviceName << "\u0027 (PhysicalDevice) for VkDevice Creation");
        return false;
    } else {
        return true;
    }
}
