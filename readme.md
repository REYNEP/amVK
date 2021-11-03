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