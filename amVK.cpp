#define am_DEV
#include "amVK.hh"
#include "amVK_Logger.hh"

/**
 * This file was created on Last week of JUNE 2021     ;)        [Now its SEPT 23, 2021,   v0.0.1 (first GITHUB)]
 * \see ./.readme/amVK.md
 * if you are inspecting this file, make sure, you use    'function collapse'   option
 * I came across a Cool theme for VSCODE: EvaTheme
 */

//---------------- THE ULTIMATE GOAL WE'RE WORKING TOWARDS ----------------
amVK_CX                       *amVK_CX::heart = nullptr;
VkInstance                     amVK_CX::instance{};         //Multiple Instance is not supported yet
VkApplicationInfo              amVK_CX::vk_appInfo{};


VkInstance amVK_CX::CreateInstance(void) {
    if (amVK_CX::vk_appInfo.engineVersion == NULL) {  //amVK_CX::set_VkApplicationInfo()  sets  engineVersion by default
        amVK_CX::set_VkApplicationInfo(nullptr);      //this func wasn't called before
    }


    // ----------- Extensions for vkCreateInstance ------------
    amVK_CX::enum_InstanceExts();           //Loads into  _iep
    amVK_CX::filter_SurfaceExts();          // _req_surface_ep
    amVK_CX::add_InstanceExts(_req_surface_ep[0].extensionName);
    amVK_CX::add_InstanceExts(_req_surface_ep[1].extensionName);
    amVK_CX::add_InstanceExts("VK_EXT_debug_report");
    amVK_CX::add_InstanceExts("VK_EXT_debug_utils");
    LOG_LOOP("Enabled Instance Extensions:- ", i, _iExts.size(), _iExts[i]);


    // ----------- CreateInfo for vkCreateInstance ------------
    VkInstanceCreateInfo the_info{};
    the_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    the_info.pApplicationInfo = &(amVK_CX::vk_appInfo);
    //the_info.pNext = nullptr;  the_info.flags [KHR Says 'reserved for future'] [No need to care about these 2 for now]
    the_info.enabledExtensionCount = static_cast<uint32_t>(_iExts.size());
    the_info.ppEnabledExtensionNames = _iExts.data();


    // ----------- ValidationLayers for vkCreateInstance ------------
    if (enableDebugLayers_LunarG) {
        amVK_CX::enum_ValLayers();
        the_info.enabledLayerCount = static_cast<uint32_t>(_vLayers.size());
        the_info.ppEnabledLayerNames = _vLayers.data();
        LOG_LOOP("Enabled Validation Layers:- ", i, _vLayers.size(), _vLayers[i]);
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



amVK_Device *amVK_CX::CreateDevice(VkDevice *D, bool surfaceSupport) {
    // ----------- Physical Device Info ------------
    amVK_CX::enum_PhysicalDevs();
    amVK_CX::enum_PD_qFamilies();
    
    if (pdChozen == nullptr) {
        bool PD_reUse = this->auto_choosePD();
        if (!PD_reUse) {
    #ifdef REUSE_PHYSICAL_DEVICE
            LOG("reUsing GPU a.k.a PhysicalDevice for VkDevice Creation");
    #else
            LOG("Can't use Same PhysicalDevice to create LogicalDevice. returning nullptr. [See macro REUSE_PHYSICAL_DEVICE]");
            return nullptr;
    #endif
        }
    }

    LOG("GPU SELECTED. Going in For vkCreateDevice");
    // ----------- CreateInfo for vkCreateDevice Queues ------------
    // [GDC16, Downloaded talk], stackoverflow.com/a/55273688 & see 3.2 Fundamentals.Queues of VKSpecs [additional res: github.com/jcoder58/VulkanResources]
    // vkSpecs[Sec3.2 BoxNote] says that one VkDevice
    // ---- qFamily Selection ---- [why different qFamilies exists stackoverflow.com/a/65086765]
    // queues are So Fucking Confusing : https://www.reddit.com/r/vulkan/comments/kjb388/reason_for_only_creating_1_queue/ggw81mc?utm_source=share&utm_medium=web2x&context=3
    VkDeviceCreateInfo the_info = {};
        the_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;  //[khr-reg]/vulkan/specs/1.2-extensions/man/html/VkStructureType.html
        the_info.pNext = nullptr;
        //the_info.flags [reserved for Future usage] & 40.3.1. Device Layer Deprecation
        the_info.queueCreateInfoCount = 1;      //Probably this enables multiple queueFamilies \see [khr-reg]/vulkan/specs/1.2-extensions/man/html/VkDeviceCreateInfo.html
        VkDeviceQueueCreateInfo info2 = {};
            info2.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            info2.pNext = nullptr;
            info2.flags = 0;             //currently only 1 option [khr-vk]/1.2-extensions/man/html/VkDeviceQueueCreateFlagBits.html
            info2.queueFamilyIndex = 0;  //TODO: Add support for Choices & multiple queueFamilies
            info2.queueCount = 1;        //TODO: Add support for choices
            float qPriorityList = 0.f;      //TODO: QueuePriority
            info2.pQueuePriorities = &qPriorityList;
        the_info.pQueueCreateInfos = &info2;
        the_info.enabledExtensionCount = dExtc_alpha;     //TODO + Below
        the_info.ppEnabledExtensionNames = dExts_alpha;
        VkPhysicalDeviceFeatures info3 = {};
        the_info.pEnabledFeatures = &info3;
        the_info.ppEnabledLayerNames = _vLayers.data();
        the_info.enabledLayerCount = 1;

    
    // ----------- Actually Create the VkInstance ------------
    VkDevice device = nullptr;
    VkResult res = vkCreateDevice( //Occupies Almost 20MB
        pdChozen,           //VkPhysicalDevice physicalDevice           [Obviously, The Physical Device that is gonna be Used]
        &the_info,          //const VkDeviceCreateInfo*  pCreateInfo    [Just like the regular Other createInfo's that other vkCreateFuncs has]
        nullptr,            //const VkAllocationCallbacks* pAllocator   [Duh!!!!]
        &device             //VkDevice* pDevice                         [Our Very New LOGICAL DEVICE]
    );
    if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res)); LOG("vkCreateDevice() failed, time to call the devs, it's an highly unstable emergency"); return nullptr;}
    
    if (D != nullptr) {*(D) = device;}

    LOG("VkDevice Created! Yessssss, Time Travel! \n");

    amVK_Device *the_device = new amVK_Device(device, pdChozen);
    _device_list.push_back(the_device);

    return the_device;
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
  * |-----------------------------------------|
  *    - FORBIDDEN STUFFS [CreateInstance] -
  * |-----------------------------------------|
  * Vulkan Extensions: https://www.khronos.org/registry/vulkan/ 
  * Validation Layers by LunarG
*/

// This is here cz, this one is the simplest form of   enum_InstanceExts
void amVK_CX::enum_ValLayers(void) {
    if (_vLayers_p.size() == 0) {
        uint32_t n;
        vkEnumerateInstanceLayerProperties(&n, NULL);

        _vLayers_p.resize(n);
        vkEnumerateInstanceLayerProperties(&n, _vLayers_p.data());
    }

    amLOG_LOOP("All Available Layers:- ", i, _vLayers_p.size(), _vLayers_p[i].layerName);
    //Also see validationLayers_LunarG in FORBIDEN VARIABLES Section
}

bool amVK_CX::enum_InstanceExts(bool do_log, VkExtensionProperties **pointer_iep, int *pointer_iec) {
    // ----------- Get Instance Extensions anyway ------------
    uint32_t n;
    vkEnumerateInstanceExtensionProperties(nullptr, &n, nullptr);
    LOG(n << " Instance extensions supported\n");

    _iep.resize(n);       _isEnabled_iExt = static_cast<bool *> (calloc(n, 1));

    VkResult res = vkEnumerateInstanceExtensionProperties(nullptr, &n, _iep.data());
    if (res != VK_SUCCESS) {
        LOG_EX("vkEnumerateInstanceExtensionProperties() Failed.... \nVulkan Result Message:- " << amVK_Utils::vulkan_result_msg(res) << std::endl);
        return false;
    }
 
    // ----------- LOG/ENUMERATE (to CommandLine) ------------
    if (do_log) {  LOG_LOOP("All the Instance Extensions:- ", i, _iep.size(), _iep[i].extensionName); }

    if (pointer_iep != nullptr)             {   *(pointer_iep) = _iep.data(); }
    if (pointer_iec != nullptr)             {   *(pointer_iec) = _iep.size(); }

    return true;
}

bool amVK_CX::filter_SurfaceExts(void) {
    if (_iep.size() == 0) {
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
        for (uint32_t j = 0; j < _iep.size(); j++) {
            if (strcmp(_iep[j].extensionName, xd[i]) == 0) {
                *(flagsSurfaceExtsPtr + i) = true;
                tmp_surface_ep[i] = _iep[j];      //_iep[j] == xd[i]
            }
        }
    }



    /**
        ╻ ╻   ┏━┓╺┳╸╻ ╻┏━╸┏━╸┏━┓
        ╺╋╸   ┗━┓ ┃ ┃ ┃┣╸ ┣╸ ┗━┓   that you need to care about
        ╹ ╹   ┗━┛ ╹ ┗━┛╹  ╹  ┗━┛
    */
    _req_surface_ep.resize(2);   //For now we know this will be 2

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
        LOG(extName << " is already added to _iExts list. Before RELEASE, make sure you see the default _iExts that amVK adds & also don't try to add something twice");
        return true; //cz already added before and reported LOG
    } 

    else {
        _iExts.push_back(_iep[index].extensionName);    //don't wanna depend on extName, that might be on stack
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
        LOG(vLayerName << " is already added to _vLayers list. Before RELEASE, make sure you see the default _vLayers that amVK adds & also don't try to add something twice");
        return true; //cz already added before and reported LOG
    } 

    else {
        _vLayers.push_back(_vLayers_p[index].layerName);    //don't wanna depend on extName, that might be on stack
        return true;
    }
}
















/**
  * |-----------------------------------------|
  *     - FORBIDDEN STUFFS [CreateDevice] -
  * |-----------------------------------------|
*/
bool amVK_CX::load_PD_info(bool force_load) {
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
        bool any_PD_sup = amVK_CX::enum_PhysicalDevs();
        if (!any_PD_sup) {
            LOG_EX("Vulkan not supported by any Available Physical Device");
            return false;
        }
        amVK_CX::enum_PD_qFamilies();

        LOG("Loaded PD_info \n");
        return true;
    }
}

bool amVK_CX::enum_PhysicalDevs() {
    if (PD.list != nullptr) free(PD.list);      //By Default force_load()

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
                       +(PD_n * sizeof(amVK_Array<VkQueueFamilyProperties>)));      //sizeof this one will be 16
    

    // ----------- Use Malloc ------------
    PD.list = static_cast<VkPhysicalDevice *> (test);
    PD.props = reinterpret_cast<VkPhysicalDeviceProperties *> (PD.list + PD_n);
    PD.features = reinterpret_cast<VkPhysicalDeviceFeatures *> (PD.props + PD_n);
    PD.qFamily_lists = reinterpret_cast< amVK_Array<VkQueueFamilyProperties>* > (PD.features + PD_n);


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
#ifndef NDEBUG
    if (PD.list == nullptr) LOG("call amVK_CX::enum_PhysicalDevs() first");  return false;
#endif

    // ----------- GET HOW MUCH TO MALLOC ------------
    uint32_t n = 0;
    uint32_t tmp_n = 0;
    for (uint32_t i = 0, lim = PD.n; i < lim; i++) {
        vkGetPhysicalDeviceQueueFamilyProperties(PD.list[i], &n, nullptr);
        tmp_n += n;
    }


    if (PD.qFamily_lists[0].data != nullptr) free(PD.qFamily_lists[0].data);

    // ----------- The Malloc ------------
    void *test = malloc(tmp_n * sizeof(VkQueueFamilyProperties));
    PD.qFamily_lists[0].data = static_cast<VkQueueFamilyProperties *> (test);

    // ----------- Use Malloc & Get qFamily_props ------------
    n = 0;
    tmp_n = 0;
    for (uint32_t i = 0; i < PD.n; i++) {
        vkGetPhysicalDeviceQueueFamilyProperties(PD.list[i], &n, nullptr);
        tmp_n += n;

        PD.qFamily_lists[i].data = reinterpret_cast<VkQueueFamilyProperties *> (PD.qFamily_lists[0].data + tmp_n);
        vkGetPhysicalDeviceQueueFamilyProperties(PD.list[i], &PD.qFamily_lists[i].n, PD.qFamily_lists[i].data);
    }
    return true;
}

void amVK_CX::benchMark_PD(void) {
    if (PD.list != nullptr) {
        pd_benchMarks = (uint32_t *) calloc(PD.n, sizeof(uint32_t));

        for (uint32_t i = 0; i < PD.n; i++) {
            uint32_t mk = 0;

            LOG("BenchMarking " << PD.props[i].deviceName);
            VkPhysicalDevice device = PD.list[i];
            VkPhysicalDeviceProperties device_props = PD.props[i];

            if (device_props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                mk += 2073600;     //1920*1080 [See the comment below]
            }
            //we really need to work on Score values, instead of += limits.maxImageDimension2D
            mk += device_props.limits.maxImageDimension2D;

            pd_benchMarks[i] = mk;
        }

        //Sorting [TODO: Test mergeSOrt, Impl memcpy, don't include <string> in amVK_utils]

        bool sort_success = false;

        
        pd_sortedByMark = static_cast<uint32_t *> (calloc(PD.n, sizeof(uint32_t)));
        for (int i = 0; i < PD.n; i++) {pd_sortedByMark[i] = i;}
        sort_success = amVK_Utils::mergeSort<uint32_t> (0, PD.n-1, pd_benchMarks, pd_sortedByMark);
    } else {
        LOG_EX("call amVK_CX::load_PD_info() before calling this function");
    }
}

bool amVK_CX::auto_choosePD(void) {
    benchMark_PD();
    //Iterate over PD.list[pd_sortedByMark[i]]
    for (uint32_t i = 0; i < PD.n; i++) {
        bool matched = false;
        //Check if used before
        if (_usedPD_list.size() > 0) {
            for (VkPhysicalDevice used_PD : _usedPD_list) {
                if (used_PD == PD.list[pd_sortedByMark[i]]) {
                    matched = true;
                    break;
                }
            }
        }
        
        if (!matched) {pdChozen = PD.list[pd_sortedByMark[i]]; break;}
    }
    //If no non used_PD is found. then clear the usedPD_list & use the same ones in BenchMark order
    if (pdChozen == nullptr) {
        pdChozen = PD.list[pd_sortedByMark[0]]; //that means we take the first one in the SortedList
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