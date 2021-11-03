#### amVK is not a Renderer.... make your own using amVK, thats the Idea. amVK is just a Thin layer on top of Khronos-Vulkan-API

##### will target lots of patches till v0.0.5

## [v0.0.3] Well, it builds.... [& Works... so yay!] Time Travel! 
use  `python make.py`  ***or***  `cmake --build ./build-x64/ --target install --config release`

## inside ./extern   [downloaded_by_make.py]
- vulkan-sdk-lunarg - comes with [VulkanSDK](https://www.lunarg.com/vulkan-sdk/) by LunarG
- VulkanMemoryAllocator [VMA](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)


#### Roadmap... i guess

```md
# We mostly used Forward declarations in Header files.... to make every file independant of one another

#####        MK-1: There was an idea.... nope, nope, nope, there wasn't! MK1 simply was JustCause4 I needed to divide stuffs into classes & modules & functions, and thats it!

#####  [WIP] MK-2: MOD[ifiable] Vars in every classes.... incrementing some ModVar_n before`malloc` will increase malloced memory.... and DESTRUCTOR won't do anything, theres a `destroy()` func....

##### [PLAN] MK-3: A JSON/Similar file support.... for Modifications
##### [PLAN] MK-4: [SameIdeaAS_MK3] only a new Graphical USER-Interface to create the JSON Files + Link Inputs [such 1 case is SWAPCHAIN, Renderpass, FrameBuffer imageFormat]

##### [PLAN] MK-5: A Bpy like api.... where everything is connected e.g. amVK_CommandPool can store qFamily number its using.... pointers to amVK_DeviceMK2 * such is basic     [But this can increase Memory usage]
```

#### ex1
```cpp
#include "amVK_CX.hh"   //Doesn't Include other amVK Headers....

int main(void) {
    amVK_CX amVK = amVK_CX();
    VkInstance instance = amVK.CreateInstance();

    this->device    = new amVK_DeviceMK2(amVK_DP_GRAPHICS, NULL, 3, 0); //erro if 3rd param  bigger than 0 & you dont make use of that
        amVK_ARRAY_PUSH_BACK(device->exts) = ("VK_KHR_imageless_framebuffer");
        amVK_ARRAY_PUSH_BACK(device->exts) = ("VK_KHR_maintenance2");
        amVK_ARRAY_PUSH_BACK(device->exts) = ("VK_KHR_image_format_list");
        //device->exts is    amVK_Array<>    [amVK_Utils.hh]
    this->device->create();
        //Creates VkDevice finally.... [device->_D]

    amVK.activate_device(this->device); /** doing this lets you relax, cz you dont need to pass amVK_DeviceMK2 to amVK object creations anymore.... */
    // WIP.... almost done....

    //Then Create RenderPass, Swapchain (also Attachments & Framebuffers), CommandBuffers
    //Then implement MainLoop
    return 0;
}
```


#### ex2
```cpp
For now.... check RTC repo....
```

### [Will open a GIST Later]  IDE & other Stuffs....  if you are new to C++
If U R using VSCode /w C/C++ Ext (for VSCode), & you use CMake, you should also install that CMakeTools Extension, or you will need to set includePath & defines in your [c_cpp_proprties.json](https://code.visualstudio.com/docs/cpp/c-cpp-properties-schema-reference) (note: if you open a folder in vscode, that folder a.k.a. the Project can have a `.vscode` folder which can have `settings.json`, but `c_cpp_properties.json` is a C/C++ Extension specific file....)

C/C++ Extension comes with its own EnhancedColorization option. But those are not Overridable using the VSCode API as of 2021 November. So I opened an [issue](https://github.com/microsoft/vscode/issues/135599).

I came across a [Less-Contrasty] theme for VSCODE: [EvaTheme by fisheva](https://github.com/fisheva/Eva-Theme)
Or, if you want Cool Contrasty themes, ask [vrecusko](https://github.com/vrecusko) 😉

##### Github
If you are searching on GITHUB, it Has a problem, Sometimes it won't show stuffs from BIG files when you search like vkCreateInstnace in trampoline.c

### Where should you start, [a.k.a. a Brief about code structure]
amVK_IN.hh - Common stuffs that other amVK files relies on.   this #include(s) amVK_Logger.hh, amVK_Utils.hh, amVK_Types.hh, amVK_Defines.hh & vulkan/vulkan.h .... also has `class amVK_IN` baseClass of `amVK_CX`

amVK_CX.hh - This is what you should include to ***Initialize*** amVK. *See* ***ex1*** block. You can access `PD` member_var [type: `loaded_PD_info_plus_plus`, (amVK_Types.hh)] 
amVK_CX.cpp - Implementations


### Notes
- ***I:*** I didn't do much error handling, (cz of *FAST & FURIOUS* **performance....**)  [tho You can enable ValidationLayers] </br> mostly tried to return false/nullptr (and I do plan on making our own ValLayers)

- ***II:*** Varaibles are **exposed.** As in, there are no `get_**` or `set_**` *funcs.* You can just go ahead and change *values* & call *funcs* after that.... </br> Most of the times *vars* can & will affects *funcs* so we think of these *vars* as ***ModVars***

- ***III:*** We dont use `_NDEBUG`, we use `amVK_RELEASE`, [added_by_buildTool] </br>
[its Kinda in a wip stage now.... cz not really used anywhere]

- ***IV:*** Mostly `uint32_t` used instead of `int`, as vulkan also follows that.

- ***V:*** I Tried to minimize as much IMPLICIT behavior as I could.... "EXPLICIT is Better than IMPLICIT" - [PEP20]

- ***VI:*** Vulkan can feel like _'Whooo, thats a lot of stuffs to remember....'_ but, dont think that way. in amVK I tried to keep related stuffs inside single amVK File.... & if two are connected I am thinking of making Videos on them....

- ***VII:*** ***Naming Conventions:*** </br>
  - Classes: Destructors are not the best way.... so we have `destroy()` function. Sometimes instead of *Constructor*,`create()` [e.g. `ImageMK2` for now] should be used.

  - Funcs: mostly will have `create_ / load_ / build_ / get_ / set_ / destroy_ / alloc_` prefix....
           
  - ***Vars:***
    1. \internal type members are like _amVK_D, _amVK_RP etc. </br>
    2. \private vars are like _pushConst, _descSet, [will have ' _' prefix.]</br>
                   
    3. \vars decl. inside funcs are like 'python_snake_case' mostly. </br> but can be 'camelCase' or anything, doesn't matter really!

    4. ***ModVars*** are 'camelCase' always [e.g. `shaderInputs, vert, frag, VIBindings, vertTopo, viewportCount`]
    5. `VIBindings` is dif. cz, VI is short_form.... such smtimes can apply to things like `amVK_WI_MK2::IMGs....`
    6. ***\exception*** `the_info, amVK_WI::OG`

    7. \public vars are like 'camelCase' too, most times.... [gotta change & match I guess....]

    8. There might be abbreviations like `CI` for CreateInfo.... [I prolly should list them here] </br>
        ***OG*** - *Oh Geez...*. cz this struct holds lots of stuffs </br>
        ***CI*** - *CreateInfo*  </br>
        ***IEP*** - *Instance Extension Properties* [only in `amVK_CX::IEP`]</br>
        ***PD*** - *PhysicalDevice* [only in `amVK_IN::PD`] </br>

- ***VIII:*** **HEART:** `static amVK_IN *heart` [`class amVK_IN`]   THE One & Only HEART.... </br>
[cz Multi-instance isn't officially supported yet, tho you can \see CreateInstance impl. and use under the hood functions]

- ***IX:*** **activeD:** `static amVK_DeviceMK2 *activeD` [`class amVK_IN`] Most stuffs will need to be created on the same VkDevice.... </br>
    Use `amVK_CX::activate_device()` [`virtual of class amVK_IN`] to set *value* </br>
    if this is set, you can relax & don't pass `amVK_DeviceMK2` as parameter whenever you create an amVK Object.
    
- ***X:*** `amFUNC_HISTORY` & `amFUNC_HISTORY_INTERNAL` called in amVK *funcs* - Prints MangledName [defined in `amVK_Logger.hh`] </br>
    Same named Options in CMakeLists.txt
             