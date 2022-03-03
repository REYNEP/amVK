<h3 align=center> 
amVK is not a Renderer.... make your own using amVK, thats the Idea. amVK is just a Thin layer on top of Khronos-Vulkan-API
</h3>
<p align=center>MULTI-DEVICE (VkDevice) isn't supported officially, But you can work your way around....</p>

## [v0.0.4a] well, it builds & works... so yay! Time Travel! 
use `python make.py`  ***or***
```sh
git submodule init                 #After git clone
git submodule update
python extern/download.py GDrive   #can clone github stuffs or download from the drive link
cmake --build ./build-x64/ --target install --config release
# You can use add_subdirectory(amVK) [cmake Command], after the first 3 lines
```

## External Stuffs
- `VK [submodule]`: Khronos/Vulkan-Headers, we don't use the CMakeLists.txt, bcz this CMakeLists.txt doesn't build any library
- `glslang [download.py]`: Khronos/glslang: Like Godot uses it....
- `download.py`: uses (extern.zip - GDrive)[https://drive.google.com/file/d/14A_2sXYsnkEeBK6FR2Pq7AUQO1E0OB2O/view]


## ex1
```cpp
#include "amVK_CX.hh"   //Doesn't Include other amVK Headers....

class RTC_amVK {
    static inline amVK_CX amVK = amVK_CX();         /** C++17 */
    static inline VkInstance instance = nullptr;    /** C++17 */
    static inline VkDevice device = nullptr;        /** Single Device Support */

    RTC_amVK(void) {
        amVK.CreateInstance();
        this->instance = amVK.instance;     /** amVK.CreateInstance function also returns it, tho */

        /** if you got Multi GPU system, check out amVK_SurfaceMK2::select_DisplayDevice() [amVK_WI.hh] */
        this->device    = new amVK_DeviceMK2(amVK_DP_GRAPHICS, NULL, 3, 0);
            amVK_ARRAY_PUSH_BACK(device->exts) = ("VK_KHR_imageless_framebuffer");
            amVK_ARRAY_PUSH_BACK(device->exts) = ("VK_KHR_maintenance2");
            amVK_ARRAY_PUSH_BACK(device->exts) = ("VK_KHR_image_format_list");
            /** device->exts is    amVK_Array<>    [amVK_Utils.hh] */
        this->device->create();
        /** Creates VkDevice [device->_D] finally....  */

    amVK.activate_device(this->device); 
    /** doing this lets you relax, [W.I.P.]
     * cz you don't need to pass amVK_DeviceMK2 to amVK object creations anymore.... */

    /**
     * .
     * .
     * .
     * Then Create RenderPass, Swapchain (also Attachments & Framebuffers), CommandBuffers
     * Then implement MainLoop
     * 
     * You may wanna check out my RTC repo for now.... 😄
     */
    return 0;
};
```


## Files [i dont like calling these *'modules'*]
- `amVK_CX`: This is what you should include to ***Initialize*** amVK. *See* ***ex1*** block.
    - Entirely dedicated to VkInstance only.... though its a ***child-class*** of amVK_IN
    - includes `amVK_IN.hh` only

- `amVK_IN`: Included in almost every amVK Files.... has some variables that stores core System info that are needed accross many amVK files, e.g. VkPhysicalDevice related info
    - Includes: `amVK_Logger.hh`, `amVK_Utils.hh`, `amVK_Types.hh`, `amVK_Defines.hh` & `vulkan/vulkan.h`
</br>

- `amVK_Device`:
    - Includes: `amVK_IN.hh` & `intern/amVK_Memory.hh` for now [cz MemoryMK2 is in W.I.P.]
</br>
