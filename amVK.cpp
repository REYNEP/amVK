#define amVK_CPP
#define am_DEV
#include "amVK.hh"


/**
 * This file was created on Last week of JUNE 2021     ;)        [Now its SEPT 23, 2021,   v0.0.1 (first GITHUB)]
 * \see ./.readme/amVK.md
 * if you are inspecting this file, make sure, you use    'function collapse'   option
 * I came across a Cool theme for VSCODE: EvaTheme
 * also ANSI Decorative Fonts: [i am pretty sure the NAME is misused]   \see ASCIIDecorator by helixquar and 'Convert To ASCII Art' by BitBelt... I also se my CUSTOM ONE ;) 
 */

//---------------- THE ULTIMATE GOAL WE'RE WORKING TOWARDS ----------------
amVK_CX                       *amVK_CX::heart = nullptr;
amVK_Device                   *amVK_CX::activeD = nullptr;
VkInstance                     amVK_CX::instance{};         //Multiple Instance is not supported yet officially
VkApplicationInfo              amVK_CX::vk_appInfo{};


VkInstance amVK_CX::CreateInstance(void) {
    if (amVK_CX::instance != nullptr) {     //But this Function would still work
        LOG("amVK_CX::instance isn't  nullptr....   maybe you Created an Instance already");
        return nullptr;
    }

    if (amVK_CX::vk_appInfo.engineVersion == NULL) {  //amVK_CX::set_VkApplicationInfo()  sets  engineVersion by default
        amVK_CX::set_VkApplicationInfo(nullptr);      //this func wasn't called before
    }


    // ----------- Extensions for vkCreateInstance ------------
    amVK_CX::enum_InstanceExts();           //Loads into  IEP      [Force_Load]
    amVK_CX::filter_SurfaceExts();          // _req_surface_ep
    amVK_CX::add_InstanceExts(_req_surface_ep[0].extensionName);
    amVK_CX::add_InstanceExts(_req_surface_ep[1].extensionName);
    amVK_CX::add_InstanceExts("VK_EXT_debug_report");
    amVK_CX::add_InstanceExts("VK_EXT_debug_utils");
    LOG_LOOP("Enabled Instance Extensions:- ", i, enabled_iExts.size(), enabled_iExts[i]);


    // ----------- CreateInfo for vkCreateInstance ------------
    VkInstanceCreateInfo the_info{};
    the_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    the_info.pApplicationInfo = &(amVK_CX::vk_appInfo);
    //the_info.pNext = nullptr;  the_info.flags [KHR Says 'reserved for future'] [No need to care about these 2 for now]
    the_info.enabledExtensionCount = static_cast<uint32_t>(enabled_iExts.size());
    the_info.ppEnabledExtensionNames = enabled_iExts.data();


    // ----------- ValidationLayers for vkCreateInstance ------------
    if (enableDebugLayers_LunarG) {
        amVK_CX::enum_ValLayers();
        the_info.enabledLayerCount = static_cast<uint32_t>(enabled_vLayers.size());
        the_info.ppEnabledLayerNames = enabled_vLayers.data();
        LOG_LOOP("Enabled Validation Layers:- ", i, enabled_vLayers.size(), enabled_vLayers[i]);
    }


    // ----------- Actually Create the VkInstance ------------
    VkResult res = vkCreateInstance(  // You will Occassionally see this general pattern of Vulkan-Function Calls
        &the_info,      //VkInstanceCreateInfo *pCreateInfo;  [safe to pass in variable that we are gonna destroy later s:(VkInstanceCreateInfo ici = *pCreateInfo;)]
        nullptr,        //VkAllocationCallbacks *pAllocator
        &instance       //VkInstance *pInstance               [s:(*pInstance = created_instance;) 's' means line of code from GITHUB_VULKAN_LOADER]
    );
    if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res)); return nullptr;}


    /** 
     * [Mar 4th Week I Think, 3 Months Ago from now, Check again ASAP]
     * BLENDER's CURRENT (MARCH 2021) VULKAN STATE, uses vkCreateInstance inside initializeDrawingContext() from what I have seen, 
     * I believe that is a Function called for Every GHOST_Context created    [so caution is, vkCreateInstance might be called multiple times]
     * https://developer.blender.org/T68990 & https://developer.blender.org/P1590
     */

    amVK_CX::instance = instance;   LOG("VkInstance Created!");
    amVK_CX::heart = this;          LOG("amVK_CX::heart set! \n\n");
    return instance;
}



amVK_Device *amVK_CX::CreateDevice(amVK_DevicePreset_Flags presets, VkDeviceCreateInfo *CI) {
    // ----------- Physical Device Info ------------
    if (PD.chozen == nullptr) {
        if (amVK_CX::instance == nullptr) {
            LOG("amVK_CX::CreateInstance() has to be called before. [or set amVK_CX::instance & amVK_CX::heart]");
        }
        amVK_CX::enum_PhysicalDevs();
        amVK_CX::enum_PD_qFamilies();
        this->auto_choosePD();
    }
    LOG("GPU SELECTED:- \u0027"  << PD.props[PD_to_index(PD.chozen)].deviceName << "\u0027 Going in For vkCreateDevice");


    // ----------- THE CREATE INFO ------------
    amVK_DeviceMods *MODS = nullptr;    /** [for] new amVK_Device   \see ~amVK_DEVICE()   */
    if (CI != nullptr) {
        goto finally_CreateDevice;
    }

    VkDeviceCreateInfo the_info = {};
    CI = &the_info;
        the_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        the_info.pNext = nullptr;
        the_info.flags = 0;

    MODS = new amVK_DeviceMods(presets, PD.chozen, true);
        the_info.queueCreateInfoCount = MODS->qCIs.n;
        the_info.pQueueCreateInfos = MODS->qCIs.data;

        // ----------- DEVICE EXTENSIONS, ENABLED FEATURES ------------
        the_info.enabledExtensionCount = MODS->exts.n;
        the_info.ppEnabledExtensionNames = MODS->exts.data;
        the_info.pEnabledFeatures = &MODS->req_ftrs;

    
    // ----------- Actually Create the VkDevice ------------
    finally_CreateDevice:
    {
        VkDevice device = nullptr;
        VkResult res = vkCreateDevice( //Occupies Almost 20MB
            PD.chozen, CI, nullptr, &device
        );
        if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res)); LOG("vkCreateDevice() failed, time to call the devs, it's an highly unstable emergency. amASSERT"); amASSERT(true); return nullptr; }

        LOG("VkDevice Created! Yessssss, Time Travel! \n");

        amVK_Device *the_device = new amVK_Device(device, PD.chozen, MODS);
        D_list.push_back(the_device);
        PD.isUsed[PD.chozen_index] = true;

        return the_device;
    }
}


/**
 *             ███╗   ███╗██╗  ██╗██████╗ 
 *   ▄ ██╗▄    ████╗ ████║██║ ██╔╝╚════██╗
 *    ████╗    ██╔████╔██║█████╔╝  █████╔╝
 *   ▀╚██╔▀    ██║╚██╔╝██║██╔═██╗ ██╔═══╝ 
 *     ╚═╝     ██║ ╚═╝ ██║██║  ██╗███████╗
 *             ╚═╝     ╚═╝╚═╝  ╚═╝╚══════╝                        
 */
amVK_Device *amVK_CX::CreateDeviceMK2(amVK_DeviceMods *MODS) {
    VkDeviceCreateInfo the_info = {
        VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, nullptr, 0,
        MODS->qCIs.n, MODS->qCIs.data,          0, nullptr, /* [Deprecated] Layer */
        MODS->exts.n, MODS->exts.data,
       &MODS->req_ftrs
    };
    amVK_Device *the_device = this->CreateDevice(amVK_DP_UNDEFINED, &the_info);
    the_device->_MODS = MODS;
    LOG("MK2 NO ERROR");
    return the_device;
}
amVK_DeviceMods *amVK_CX::DeviceModsMK2(amVK_DevicePreset_Flags presets, uint32_t ur_exts_n, uint32_t ur_qCIs_n) {
    if (!PD.list) load_PD_info(false, true);

    amVK_DeviceMods *MODS = new amVK_DeviceMods(presets, PD.chozen, false);
        MODS->qCIs.n = ur_qCIs_n;
        MODS->exts.n = ur_exts_n;
        MODS->do_everything();

    return MODS;
}

























/** \param appInfo: default value is nullptr. */
void amVK_CX::set_VkApplicationInfo(VkApplicationInfo *appInfo) {
    // ----------- Muhaha, we use OUR Own stuffs.... if nullptr ------------
    if (appInfo == nullptr) {
    #ifndef am_DEV
        LOG("(amVK Dev Blubbering: muHahahaha, you didn't think about VkApplicationInfo, I am definitely gonna use that for Advertising purposes)");
    #endif    

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
    appInfo->engineVersion = VK_MAKE_VERSION(0, 0, 1);
    LOG("amVK Engine Version 0.0.1b \n\n");
    
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
    if (vLayerP.n == 0) {
        uint32_t n;
        vkEnumerateInstanceLayerProperties(&n, NULL);

        vLayerP.data = new VkLayerProperties[n];
        vLayerP.n = n;
        vkEnumerateInstanceLayerProperties(&n, vLayerP.data);
    }

    amLOG_LOOP("All Available Layers:- ", i, vLayerP.n, vLayerP[i].layerName);
    //Also see validationLayers_LunarG in FORBIDEN VARIABLES Section
}

bool amVK_CX::enum_InstanceExts(bool do_log, VkExtensionProperties **pointer_iep, int *pointer_iec) {
    // ----------- Get Instance Extensions anyway ------------
    uint32_t n;
    vkEnumerateInstanceExtensionProperties(nullptr, &n, nullptr);
    LOG(n << " Instance extensions supported\n");

    IEP.data = new VkExtensionProperties[n];   
    IEP.n = n;    
    _isEnabled_iExt.data = static_cast<bool *> (calloc(n, 1));
    _isEnabled_iExt.n = n;

    VkResult res = vkEnumerateInstanceExtensionProperties(nullptr, &n, IEP.data);
    if (res != VK_SUCCESS) {
        LOG_EX("vkEnumerateInstanceExtensionProperties() Failed.... \nVulkan Result Message:- " << amVK_Utils::vulkan_result_msg(res) << std::endl);
        return false;
    }
 
    // ----------- LOG/ENUMERATE (to CommandLine) ------------
    if (do_log) {  LOG_LOOP("All the Instance Extensions:- ", i, IEP.n, IEP[i].extensionName); }

    if (pointer_iep != nullptr)             {   *(pointer_iep) = IEP.data; }
    if (pointer_iec != nullptr)             {   *(pointer_iec) = IEP.n; }

    return true;
}

bool amVK_CX::filter_SurfaceExts(void) {
    if (IEP.n == 0) {
        LOG_EX("Call enum_InstanceExts() before calling this, and make sure that function worked OK....");
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
    std::vector<VkExtensionProperties> tmp_surface_ep(xd_size);

    bool *flagsSurfaceExtsPtr = reinterpret_cast<bool *> (&(flagsSurfaceExts));     //flagsSurfaceExts is a STRUCT... we dont know how many BOOL i'll end up having... so we take mem_address and use like array
    for (uint32_t i = 0; i < xd_size; i++) {
        for (uint32_t j = 0; j < IEP.n; j++) {
            if (strcmp(IEP[j].extensionName, xd[i]) == 0) {
                *(flagsSurfaceExtsPtr + i) = true;
                tmp_surface_ep[i] = IEP[j];      //IEP[j] == xd[i]
            }
        }
    }



    /**
        ╻ ╻   ┏━┓╺┳╸╻ ╻┏━╸┏━╸┏━┓
        ╺╋╸   ┗━┓ ┃ ┃ ┃┣╸ ┣╸ ┗━┓   that you need to care about
        ╹ ╹   ┗━┛ ╹ ┗━┛╹  ╹  ┗━┛
    */
    _req_surface_ep.data = new VkExtensionProperties[2];   //For now we know this will be 2

    //-------- Filter out the needed 2 Surface Extensions --------
#if defined(amVK_BUILD_WIN32)
    if (!flagsSurfaceExts.KHR_surface || !flagsSurfaceExts.KHR_win32_surface) {
        LOG("vkEnumerateInstanceExtensionProperties didn't report [VK_KHR_surface] or [VK_KHR_win32_surface] as available");
        return false;
    }
    else {
        _req_surface_ep[0] = tmp_surface_ep[0];
        _req_surface_ep[1] = tmp_surface_ep[1];
    }
#elif defined(amVK_BUILD_X11)
    if (!flagsSurfaceExts.KHR_surface || !flagsSurfaceExts.KHR_xlib_surface) {
        LOG("vkEnumerateInstanceExtensionProperties didn't report [VK_KHR_surface] or [VK_KHR_xlib_surface] as available");
        return false;
    }
    else if (flagsSurfaceExts.KHR_xcd_surface) {
        _req_surface_ep[0] = tmp_surface_ep[0];
        _req_surface_ep[1] = tmp_surface_ep[1];
    }
    else {
        _req_surface_ep[0] = tmp_surface_ep[0];
        _req_surface_ep[1] = tmp_surface_ep[2];
    }
#elif defined(amVK_BUILD_WAYLAND)
    if (!flagsSurfaceExts.KHR_surface || !flagsSurfaceExts.KHR_wayland_surfacee) {
        LOG("vkEnumerateInstanceExtensionProperties didn't report [VK_KHR_surface] or [VK_KHR_wayland_surface] as available");
        return false;
    }
    else {
        _req_surface_ep[0] = tmp_surface_ep[0];
        _req_surface_ep[1] = tmp_surface_ep[1];
    }
#elif defined(amVK_BUILD_COCOA)
    if (!flagsSurfaceExts.KHR_surface || !flagsSurfaceExts.MVK_macos_surface) {
        LOG("vkEnumerateInstanceExtensionProperties didn't report [VK_KHR_surface] or [VK_MVK_macos_surface] as available");
        return false;
    }
    else if (flagsSurfaceExts.EXT_metal_surface) {
        _req_surface_ep[0] = tmp_surface_ep[0];
        _req_surface_ep[1] = tmp_surface_ep[1];
    }
    else {
        _req_surface_ep[0] = tmp_surface_ep[0];
        _req_surface_ep[1] = tmp_surface_ep[2];
    }
#endif

    //At Last Success!!!! YESS!!!!        I mean, TIME TRAVEL!!!!
    amLOG_LOOP("SurfaceExts:- ", i, 2, _req_surface_ep[i].extensionName);

    return true;
}

bool amVK_CX::add_InstanceExts(char *extName) {
    uint32_t index = amVK_CX::iExtName_to_index(extName);
    if (index == 0xFFFFFFFF) {  //Error Checking
        LOG(extName << " is not Supported on this PC/Device/Computer/System whatever.... [Also check the spelling]");
        return false;
    }

    else if (_isEnabled_iExt[index])  {
        LOG(extName << " is already added to \u0027enabled_iExts\u0027 list. Before RELEASE, make sure you see the default \u0027enabled_iExts\u0027  that amVK adds & also don't try to add something twice");
        return true; //cz already added before and reported LOG
    } 

    else {
        enabled_iExts.push_back(IEP[index].extensionName);    //don't wanna depend on extName, that might be on stack
        _isEnabled_iExt[index] = true;
        return true;
    }
}

bool amVK_CX::add_ValLayer(char *vLayerName) {
    uint32_t index = vLayerName_to_index(vLayerName);
    if (index == 0xFFFFFFFF) {
        LOG(vLayerName << " is not Available on this PC/Device/Computer/System whatever..... [Also check the spelling]");
        return false;
    }

    else if (_isEnabled_vLayer[index])  {
        LOG(vLayerName << " is already added to \u0027enabled_vLayers\u0027 list. Before RELEASE, make sure you see the default \u0027_vLayers\0027 that amVK adds & also don't try to add something twice");
        return true; //cz already added before and reported LOG
    } 

    else {
        enabled_vLayers.push_back(vLayerP[index].layerName);    //don't wanna depend on extName, that might be on stack
        _isEnabled_vLayer[index] = true;
        return true;
    }
}
















/**
  * |-----------------------------------------|
  *     - FORBIDDEN STUFFS [CreateDevice] -
  * |-----------------------------------------|
*/
bool amVK_CX::load_PD_info(bool force_load, bool auto_choose) {
    if (PD.list != nullptr) {
        LOG_EX("Already PD Info loaded Once....");

        if (force_load) {
            LOG("force_load param enabled");
            free(PD.list); PD = {};     // Zero Initialize
            goto load_PD;
        }
        return true;
    }

    load_PD:
    {
        if (!HEART || !amVK_CX::instance) {LOG("amVK_CX::CreateInstance() has to be called before. [or set amVK_CX::instance & amVK_CX::heart]");}
        bool any_PD_sup = amVK_CX::enum_PhysicalDevs();
        if (!any_PD_sup) {
            LOG_EX("Vulkan not supported by any Available Physical Device");
            return false;
        }
        amVK_CX::enum_PD_qFamilies();
        if(auto_choose) {this->auto_choosePD();}    //Does Benchmark TOO!

        LOG("Loaded PD_info \n");
        return true;
    }
}

bool amVK_CX::enum_PhysicalDevs() {
    if (PD.list != nullptr) free(PD.list);      //By Default force_load

    vkEnumeratePhysicalDevices(instance, &PD.n, nullptr);
    if (PD.n == 0) {
        LOG_EX("Vulkan Loader failed to find GPUs with Vulkan support!");
        return false;
    }  else if (PD.n > 1000) {
        LOG_EX("MORE THAN A THOUSAND GPUs? Are you REALLY SURE?");
    }

    // ----------- THE GRAND MALLOC ------------
    uint32_t PD_n = PD.n;
    void *test = malloc((PD_n * sizeof(VkPhysicalDevice))
                       +(PD_n * sizeof(VkPhysicalDeviceProperties))
                       +(PD_n * sizeof(VkPhysicalDeviceFeatures))
                       +(PD_n * sizeof(amVK_Array<VkQueueFamilyProperties>))    //sizeof this one will be 16
                       +(PD_n * sizeof(bool))                                   //bool to store 'isUsed' list
                       +(PD_n * sizeof(uint32_t))                               //benchMarks
                       +(PD_n * sizeof(uint32_t)));                             //index_sortedByMark
    

    PD.list = static_cast<VkPhysicalDevice *> (test);
    PD.props = reinterpret_cast<VkPhysicalDeviceProperties *> (PD.list + PD_n);
    PD.features = reinterpret_cast<VkPhysicalDeviceFeatures *> (PD.props + PD_n);
    PD.qFamily_lists = reinterpret_cast< amVK_Array<VkQueueFamilyProperties>* > (PD.features + PD_n);

    PD.isUsed = reinterpret_cast<bool *> (PD.qFamily_lists + PD_n);
    PD.benchMarks = reinterpret_cast<uint32_t *> (PD.isUsed + PD_n);
    PD.index_sortedByMark = reinterpret_cast<uint32_t *> (PD.benchMarks + PD_n);

    /**
     * ----------- NULL / nullptr / 0 (ZERO) ------------
    for (uint32_t i = 0; i < PD_n; i++) {   
        PD.qFamily_lists[i].data = 0;     enum_PD_qFamilies()
        PD.isUsed[i] = false;             auto_choosePD()
        Dont need to do this to the PD.benchMarks or PD.index_sortedByMark.... 
    }
    */
    memset(PD.qFamily_lists, 0, PD_n * (sizeof(amVK_Array<VkQueueFamilyProperties>) + sizeof(bool)));


    // ----------- Get Stuffs ------------
    vkEnumeratePhysicalDevices(instance, &PD_n, PD.list);
    for (uint32_t i = 0; i < PD_n; i++) {
        vkGetPhysicalDeviceProperties(PD.list[i], (PD.props+i));
    }
    for (uint32_t i = 0; i < PD_n; i++) {
        vkGetPhysicalDeviceFeatures(PD.list[i], (PD.features+i));
    }

    LOG_LOOP("All the Physical Devices:- ", i, PD.n, PD.props[i].deviceName);
    return true;
}

bool amVK_CX::enum_PD_qFamilies() {
    #ifndef amVK_RELEASE
        if (PD.list == nullptr) {LOG("call amVK_CX::enum_PhysicalDevs() first");  return false;}
    #endif

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
#ifndef amVK_RELEASE
    if (PD.list == nullptr) {LOG("call amVK_CX::enum_PhysicalDevs() first");  return;}
#endif

    // ----------- THE GREAT TIME TRAVELING BENCHMARK ------------
    for (uint32_t i = 0; i < PD.n; i++) {
        LOG("BenchMarking " << PD.props[i].deviceName);
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

    // ----------- SORTING ------------ [TODO: Test mergeSort, Impl memcpy, don't include <string> in amVK_utils]
    for (int i = 0; i < PD.n; i++) {PD.index_sortedByMark[i] = i;}
    bool sort_success = amVK_Utils::mergeSort<uint32_t> (0, PD.n-1, PD.benchMarks, PD.index_sortedByMark);
}

bool amVK_CX::auto_choosePD(void) {
    benchMark_PD(); //even if already benchmarked ONCE

    // ----------- FIND NEXT notUsed PD ------------
    for (uint32_t i = 0; i < PD.n; i++) {
        if (!PD.isUsed[i]) {PD.chozen = PD.list[PD.index_sortedByMark[i]];    PD.chozen_index = i;  break;}
    }

    // ----------- USE STRONGEST PD for RECREATION ------------
    if (PD.chozen == nullptr) {
        PD.chozen = PD.list[PD.index_sortedByMark[0]];
        PD.chozen_index = 0;
        LOG_EX("reUsing \u0027"  << PD.props[PD.index_sortedByMark[0]].deviceName << "\u0027 (PhysicalDevice) for VkDevice Creation");
        return false;
    } else {
        return true;
    }
}

















/**
 * Reports Warnings and Errors based on VkResult
 * Error Messages From [kh-reg-vk/specs/1.2-extensions/man/html/VkResult.html]
 * \see impl in amVK.cpp
 * BASED-ON: Glfw
 */
const char *amVK_Utils::vulkan_result_msg(VkResult result) {
    switch (result)
    {
        case VK_SUCCESS:
            return "Success";

        case VK_NOT_READY:
            return "A fence or query has not yet completed. [Code: VK_NOT_READY]";

        case VK_TIMEOUT:
            return "A wait operation has not completed in the specified time [Code: VK_TIMEOUT]";

        case VK_EVENT_SET:
            return "An event is signaled [Code: VK_EVENT_SET]";

        case VK_EVENT_RESET:
            return "An event is unsignaled [Code: VK_EVENT_RESET]";

        case VK_INCOMPLETE:
            return "A return array was too small for the result [Code: VK_INCOMPLETE]";
            



        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return "A host memory allocation has failed [Code: VK_ERROR_OUT_OF_HOST_MEMORY]";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return "A device memory allocation has failed [Code: VK_ERROR_OUT_OF_DEVICE_MEMORY]";
        case VK_ERROR_INITIALIZATION_FAILED:
            return "Initialization of an object could not be completed for implementation-specific reasons [Code: VK_ERROR_INITIALIZATION_FAILED]";
        case VK_ERROR_DEVICE_LOST:
            return "The logical or physical device has been lost [Code: VK_ERROR_DEVICE_LOST]";
        case VK_ERROR_MEMORY_MAP_FAILED:
            return "Mapping of a memory object has failed [Code: VK_ERROR_MEMORY_MAP_FAILED]";




        case VK_ERROR_LAYER_NOT_PRESENT:
            return "A requested layer is not present or could not be loaded [Code: VK_ERROR_LAYER_NOT_PRESENT]";

        case VK_ERROR_EXTENSION_NOT_PRESENT:
            return "A requested extension is not supported [Code: VK_ERROR_EXTENSION_NOT_PRESENT]";

        case VK_ERROR_FEATURE_NOT_PRESENT:
            return "A requested feature is not supported [Code: VK_ERROR_FEATURE_NOT_PRESENT]";

        case VK_ERROR_INCOMPATIBLE_DRIVER:   
            //[VkApplicationInfo] Specs Specified that Above VK1.1 should not return this for any .apiVersion [passed on to vkCreateInstance in CONSTRUCTOR]
            return "The requested version of Vulkan is not supported by the driver or is otherwise incompatible [Code: VK_ERROR_IMCOMPATIBLE_DRIVER]";

        case VK_ERROR_TOO_MANY_OBJECTS:
            return "Too many objects of the type have already been created [Code: VK_ERROR_TOO_MANY_OBJECTS]";

        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            return "A requested format is not supported on this device [Code: VK_ERROR_FORMAT_NOT_SUPPORTED]";

            



        case VK_ERROR_SURFACE_LOST_KHR:
            return "A surface is no longer available [Code: VK_ERROR_SURFACE_LOST_KHR]";
            
        case VK_SUBOPTIMAL_KHR:
            return "A swapchain no longer matches the surface properties exactly, but can still be used [Code: VK_SUBOPTIMAL_KHR]";
        case VK_ERROR_OUT_OF_DATE_KHR:
            return "A surface has changed in such a way that it is no longer compatible with the swapchain [Code: VK_ERROR_OUT_OF_DATE_KHR]";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            return "The display used by a swapchain does not use the same presentable image layout [Code: VK_ERROR_INCOMPATIBLE_DISPLAY_KHR]";

        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            return "The requested window is already connected to a VkSurfaceKHR, or to some other non-Vulkan API [Code: VK_ERROR_NATIVE_WINDOW_IN_USE_KHR]";
        case VK_ERROR_VALIDATION_FAILED_EXT:
            return "A validation layer found an error [Code: VK_ERROR_VALIDATION_FAILED_EXT]";
        default:
            return "ERROR: UNKNOWN VULKAN ERROR";
    }
}




/**
 * | -------------------------------------- |
 *  SOMETHING ABOUT LOADING Vulkan Functions 
 * | -------------------------------------- |
 * 

You may need to configure the way you import Vulkan functions.

By default, VMA assumes you you link statically with Vulkan API. 
If this is not the case, #define VMA_STATIC_VULKAN_FUNCTIONS 0 before #include of the VMA implementation and use another way.

You can #define VMA_DYNAMIC_VULKAN_FUNCTIONS 1 and make sure vkGetInstanceProcAddr and vkGetDeviceProcAddr globals are defined. 
All the remaining Vulkan functions will be fetched automatically.

Finally, you can provide your own pointers to all Vulkan functions needed by VMA using structure member VmaAllocatorCreateInfo::pVulkanFunctions, 
if you fetched them in some custom way e.g. using some loader like Volk.

 * [FROM https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/quick_start.html]
 */
















































/**
    https://open.spotify.com/playlist/142cbkQ47RALYjSZ1SDfkj?si=0a3c910e6a214e3c
    https://open.spotify.com/playlist/6OlaKLLkqZMbeiYVlnYS3O?si=c9d61255910b4723
    https://open.spotify.com/playlist/6yIn9i7Z8WutrZaCB7ixVw?si=e3a1c53adddc4eec
    https://open.spotify.com/user/31i7ysye54yvmkkteb757x3z6zo4?si=c538a3a94a084030
    https://open.spotify.com/playlist/4rmpEaJO1C4lODSDVTST24?si=bcdf324e080141bb
    https://open.spotify.com/playlist/7sva0cdxDoes7IULKHdQZK?si=859584c635c249fc
    https://open.spotify.com/playlist/5O6qx7wpZ8kcC2VuhziNSK?si=36819cd6102f4580
 */


/**
 *              █████╗ ███╗   ███╗██╗   ██╗██╗  ██╗        ██████╗ ███████╗██╗   ██╗██╗ ██████╗███████╗███╗   ███╗ ██████╗ ██████╗ ███████╗
 *   ▄ ██╗▄    ██╔══██╗████╗ ████║██║   ██║██║ ██╔╝        ██╔══██╗██╔════╝██║   ██║██║██╔════╝██╔════╝████╗ ████║██╔═══██╗██╔══██╗██╔════╝
 *    ████╗    ███████║██╔████╔██║██║   ██║█████╔╝         ██║  ██║█████╗  ██║   ██║██║██║     █████╗  ██╔████╔██║██║   ██║██║  ██║███████╗
 *   ▀╚██╔▀    ██╔══██║██║╚██╔╝██║╚██╗ ██╔╝██╔═██╗         ██║  ██║██╔══╝  ╚██╗ ██╔╝██║██║     ██╔══╝  ██║╚██╔╝██║██║   ██║██║  ██║╚════██║
 *     ╚═╝     ██║  ██║██║ ╚═╝ ██║ ╚████╔╝ ██║  ██╗███████╗██████╔╝███████╗ ╚████╔╝ ██║╚██████╗███████╗██║ ╚═╝ ██║╚██████╔╝██████╔╝███████║
 *             ╚═╝  ╚═╝╚═╝     ╚═╝  ╚═══╝  ╚═╝  ╚═╝╚══════╝╚═════╝ ╚══════╝  ╚═══╝  ╚═╝ ╚═════╝╚══════╝╚═╝     ╚═╝ ╚═════╝ ╚═════╝ ╚══════╝                                                                                                                                    
 */

const char *amVK_DeviceMods::flag_2_strName(amVK_DevicePreset_Flags flag) {
    switch (flag)
    {
        case amVK_DP_UNDEFINED:
            return "amVK_DP_UNDEFINED";

        case amVK_DP_GRAPHICS:
            return "amVK_DP_GRAPHICS";
        case amVK_DP_COMPUTE:
            return "amVK_DP_COMPUTE";

        case amVK_DP_TRANSFER:
            return "amVK_DP_TRANSFER";
        case amVK_DP_SPARSE:
            return "amVK_DP_SPARSE";
        case amVK_DP_VIDEO_DECODE:
            return "amVK_DP_VIDEO_DECODE";
        case amVK_DP_VIDEO_ENCODE:
            return "amVK_DP_VIDEO_ENCODE";

        case amVK_DP_PROTECTED_MEM:
            return "amVK_DP_PROTECTED_MEM";

        case amVK_DP_3DEngine:
            return "amVK_DP_3DEngine";
        case amVK_DP_Encode_Decode:
            return "amVK_DP_Encode_Decode";
        case amVK_DP_Image_Shaders:
            return "amVK_DP_Image_Shaders";
        case amVK_DP_Compositor:
            return "amVK_DP_Compositor";
        case amVK_DP_RayTracing:
            return "amVK_DP_RayTracing";

        default: 
            return "We gotta Return something DUH!!! [this is amVK_DeviceMods::flag_2_strName]";
    }
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



void amVK_DeviceMods::configure_n_malloc(void) {
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
void amVK_DeviceMods::set_qCIs(void) {
    uint32_t PD_index = amVK_CX::heart->PD_to_index(_PD);
    amVK_Array<VkQueueFamilyProperties> qFAM_list = amVK_CX::heart->PD.qFamily_lists[PD_index];



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
            if (_graphics_qFAM == 0xFFFFFFFF) {LOG_MODS_NOTSUP("Couldn't Find any GRAPHICS qFamily"); does_PD_sup_mods = false;}
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
            if (!_compute_qFAM == 0xFFFFFFFF) {LOG_MODS_NOTSUP("Couldn't Find any COMPUTE qFamily"); does_PD_sup_mods = false;}
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
void amVK_DeviceMods::set_exts(void) {
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
                LOG_MODS_NOTSUP("Device Extension: \u0027" << amVK_DeviceExtensions[i] << "isn't supported....");
                result_success = false;
                does_PD_sup_mods = false;
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
void amVK_DeviceMods::set_ftrs(void) {
    uint32_t PD_index = amVK_CX::heart->PD_to_index(_PD);
    VkPhysicalDeviceFeatures sup_ftrs = amVK_CX::heart->PD.features[PD_index];

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