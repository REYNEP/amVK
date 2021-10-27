# [v0.0.3a] Well, it builds.... Time Travel! 
use    `cmake --build . --target install --config release`
currently.... its a huhe WIP

only **amVK.hh** and **amVK.cpp** is kinda like standards.... But every other file needs so many features to be added

# BUILD
- run `python make.py`    that alone should do it.... on any OS....

# inside ./extern   [downloaded_by_make.py]
- vulkan-sdk-lunarg - comes with [VulkanSDK](https://www.lunarg.com/vulkan-sdk/) by LunarG
- VulkanMemoryAllocator [VMA](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)

# MODS - is a MK2 feature [and can be a bit Slower than base MK1 impl]
Hereby we introduce MODS.... every class `amVK_CX, amVK_WI, amVK_RenderPass, amVK_Device, amVK_Pipeline` is modifiable.... most of the time, its the CI [createInfo] for the main vkCreate*** func that the class targets, _createInfos_ thats how vulkan manages its settings and stuffs, what should be what
so we got MODS section in every class documentation, look at them you will get a better idea....faster!

NOTE: For device creation [amVK_CX::CreateDevice()] we introduced Presets....

# We use Forward declarations in Header files.... to make every file independant of one another   [except_for_amVK.hh]

# Pythonic way of naming used for class member functions and vars     [We_Could_have_Used_underscore_at_beginning_too_but_duh_not_for_funcs]

# MK-1: Fastest API. cz as less Malloc as there can be, Whole Idea fits into    amVK_Class::init()... you can only modify via member variable settings  everything created is STATIC inside that init()

# [WIP] MK-2: we Introduce    amVK_DeviceMods, amVK_RenderPassMods,    You pass the Mods to the co-responding class

# [PLAN] MK-3: A JSON/Similar file support.... for Modifications
#        MK-4: [SameIdeaAS_MK3] only a new Graphical USER-Interface to create the JSON Files + Link Inputs [such 1 case is SWAPCHAIN, Renderpass, FrameBuffer imageFormat]