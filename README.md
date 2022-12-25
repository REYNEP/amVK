<h3 align=center> 
amVK is not a Renderer.... make your own using amVK, thats the Idea. amVK is just a Thin layer on top of Khronos-Vulkan-API
</h3>
<p align=center>MULTI-DEVICE (VkDevice) isn't supported officially, But you can work your way around....</p>

## [v0.0.5a] well, it builds & works... so yay! Time Travel! 
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
#include "amVK_Instance.hh"   /** Doesn't Include other amVK Headers.... */
#include "amVK_Device.hh"

class RTC_amVK {
    static inline amVK_InstanceMK2  amVK = amVK_InstanceMK2();             /** C++17 */
    static inline amVK_DeviceMK2 *device = nullptr;        /** Single Device Support */

    RTC_amVK(void) {
        amVK.Create_VkInstance();

        /** if you got Multi GPU system, check out amVK_SurfaceMK2::select_DisplayDevice() [amVK_WI.hh] */
        this->device    = new amVK_DeviceMK2(nullptr, 3, 0, amVK_DP_GRAPHICS);
            amVK_ARRAY_PUSH_BACK(device->exts) = ("VK_KHR_imageless_framebuffer");
            amVK_ARRAY_PUSH_BACK(device->exts) = ("VK_KHR_maintenance2");
            amVK_ARRAY_PUSH_BACK(device->exts) = ("VK_KHR_image_format_list");
            /** device->exts is    amVK_Array<>    [amVK_Utils.hh] */
        this->device->Create_VkDevice();
        /** Creates VkDevice [device->_D] finally....  */

    this->device->Activate_Device(); 
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

## ex2
```cpp
int main(void) {
    /**
     * 1. Create INSTANCE
     * 2. Create DEVICE      [use amVK_Surface::select_DisplayDevice() if you wish (must for multiGPU system support, ig)]
     * 3. Create SWAPCHAIN   [Surface, RenderPass]
     * 4. Create PIPELINEs
     * 5. Use the amVK_RD    [Renderer]
     * 6. Set the VkViewport
     * 
     * 7. Call as much as vkCmd stuff as you want 😉 & see ex3 for textures n images
     */
    RTC_amVK muhaha();

    amVK_SurfaceMK2     *amVK_S = new amVK_SurfaceMK2(VkSurfaceKHR, muhaha.device);
    amVK_RenderPassMK2 *amVK_RP = new amVK_RenderPassMK2(amVK_S, muhaha.device);
    amVK_RP->Create_RenderPass();

    amVK_WI_MK2        *amVK_WI = new amVK_WI_MK2("windowName", amVK_S, amVK_RP, muhaha.device);
    amVK_WI->Create_Swapchain();
    amVK_WI->create_Attachments();
    amVK_WI->create_Framebuffers();


    amVK_GraphicsPipes *amVK_PS = new amVK_GraphicsPipes(amVK_RP, amVK_D);
    amVK_PS->konfigurieren();
    amVK_PS->vert = amVK_PS->glslc_Shader("pathToVERT", Shader_Vertex);
    amVK_PS->frag = amVK_PS->glslc_Shader("pathToFRAG", Shader_Fragment);
    VkPipeline _SMTH_IN_THE_WAY = amVK_PS->build_Pipeline();



    amVK_RD RD(amVK_D);
    VkViewport xD = {0, 0, 
        amVK_WI->extent.width, amVK_WI->extent.height, 0, 1
    };


    RD.Render_BeginRecord(amVK_WI);     //These last 5 line should be inside LOOP....
    
    vkCmdSetViewport(RD.m_cmdBuf.BUF, 0, 1, &xD);
    vkCmdBindPipeline(RD.m_cmdBuf.BUF, VK_PIPELINE_BIND_POINT_GRAPHICS, SMTH_IN_THE_WAY);
    vkCmdDraw(RD.m_cmdBuf.BUF, 3, 1, 0, 0);

    RD.EndRecord_N_Submit();
}
```

## renderer example [amVK_RendererT1.hh](intern/amVK_RendererT1.hh)

</br>
</br>

## ex3
```cpp
int main(void) {
    .
    .
    .


    /** amVK_Pipeline example.... */
}
```