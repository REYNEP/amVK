# One of the Core Concept here is, You don't need to do everything with Functions, You can Directly Access/Write to CLASS VARS in amVK

# Hey so, about the Monitors, Monitors should have Contrasty HIGH Whites..... But the Browser should not use the Maximum Contrast, only the Movies should, Everyone, every developer should be aware of what its gonna look like on the eyes, Every Monitor should come with Contrast BenchMark & a Standard Point for Different COLOR-Profiles from Factory

# 1. Many things could have been just impl as NAMESPACE... but we kept them as CLASS (members not static still).... bcz of the USABILITY and MOBILITY of devs.... amVK is supposed to be VERY UNDERSTANDING and a TOOL for FASTER creation....      - So again we PUT OPTimization-Tricks all around.... [e.g. see the amVK_Pipeline.hh]

# Remember that TIME-SLIDE option in Timeline (Blender?).... Maybe have that way to Map Colors in Compositor

It's just safe to Include      amVK.hh      [just like you include vulkan/vulkan.hh]      Currently you will need to use amVK_INCLUDE_LIB
    Current Dependency of Each Modules:-
        amVK_DeviceMK2:
            |-- 
            |-- 
        amVK_WI:
            |-- [.hh]  amVK_DeviceMK2  [pointerOnly]
            |-- [.cpp] amVK_DeviceMK2  [CLASS__INFO]
        ANY:
            |-- amVK_Utils.hh
              |-- amVK.cpp  [vulkan_result_reg();] function in this TranslationUnit [The Sole Goal was to Reduce another TRANSLATION-UNIT]
        amVK_Logger:
            |-- [.hh]  amVK_Utils
        amVK_Pipeline:
            |-- [.hh]  amVK_DeviceMK2  [pointerOnly]
            |-- [.cpp] amVK_DeviceMK2  [CLASS__INFO]


# BENCHMARKS   [WIP-TODO_CustomBenchMarks]
    vkEnumeratePhysicalDevices - ~0.0016s  [First Call]   0.0010 [2nd Call]
    vkCreateSwapchain          - ~0.04s    [First Call]   ~0.008 [Re-Creattion on 768p Window]    [+vkCreateImageViews + vkCreateFrameBuffer]



[WIP]
# Passing Data Between Shader Stages
- be careful with it, as the more data you need to pass between stages, the more overhead you will have. 
- Old GPUs even have a very small number of how many variables can be passed.

# Multiple Pipelines
- Just a Couple Hours ago I thought, For switching from Solid to WireFrame view, my solution would be to have 2 Different Pipelines already there
- Just now I realized this is fairly Standard, [https://vkguide.dev/docs/chapter-2/toggling_shaders/#multiple-pipelines]

# Vertex Buffer on HOST/CPU Memory
- In Vulkan, you can allocate memory visible to the GPU, and read it from the shaders.
    1. on RX Vega 64 there is 256MB that kinda fits the needs [https://www.youtube.com/watch?v=zSG6dPq57P8&t=308s]

- You can allocate memory for two purposes, for images and buffers.  [vblanco20-1]  True for entire VULKAN API???
- ## Memory Allocation
    1. The number of total allocations is fixed by the driver, and can be a number as low as 1024.
    2. So we gonna use VMA [Vulkan Memory Allocator] (by AMD) & use SubAllocation
    3. The VMA library abstracts this slightly for us, but it’s still very important to know.

- ## Vertex Input Layout
    1. We can fill it out to let Vulkan know how to interpret the vertex data for your shader.

# VMA [Vulkan_Memory_Allocator_AMD]
- Current DEMO Usage is in RTR
- WIP [To Be impl. in amVK with cmake CHOICES]
- KEYPOINTS
    1. VMA_MEMORY_USAGE_ enums
    2. 


</br></br></br></br></br></br>

<hr></hr

</br></br></br></br></br>

<hr></hr>

</br></br></br></br></br></br>


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
    2. \private vars are like _pushConst, _dSetLayout, [will have ' _' prefix.]</br>
                   
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
             


```md
# We mostly used Forward declarations in Header files.... to make every file independant of one another

#####        MK-1: There was an idea.... nope, nope, nope, there wasn't! MK1 simply was JustCause4 I needed to divide stuffs into classes & modules & functions, and thats it!

#####  [WIP] MK-2: MOD[ifiable] Vars in every classes.... incrementing some ModVar_n before`malloc` will increase malloced memory.... and DESTRUCTOR won't do anything, theres a `destroy()` func....

##### [PLAN] MK-3: A JSON/Similar file support.... for Modifications
##### [PLAN] MK-4: [SameIdeaAS_MK3] only a new Graphical USER-Interface to create the JSON Files + Link Inputs [such 1 case is SWAPCHAIN, Renderpass, FrameBuffer imageFormat]

##### [PLAN] MK-5: A Bpy like api.... where everything is connected e.g. amVK_CommandPool can store qFamily number its using.... pointers to amVK_DeviceMK2 * such is basic     [But this can increase Memory usage]
```