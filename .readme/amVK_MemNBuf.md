So, BUFFERS.... my dear friend you've finally reached it!
Well, in short here goes the workFlow related to Buffer:-

 - decide buffer usage  ░░   eg. `UNIFORM / STORAGE / INDEX / VERTEX / RAY_TRACING / TRANSFER_SRC / TEXEL ` [via VkBufferCreateInfo.usage]     (+ buffer size)
 - decide memory usage  ░░   eg. `GPU_ONLY / CPU_TO_GPU / CPU_COPY / GPU_LAZILY`     [these are vmaBased]   *vma = Vulkan Memory Allocator by AMD
    - now VMA will select the `VkMemoryPropertyFlags` itself       e.g.  `DEVICE_LOCAL / HOST_VISIBLE / HOST_COHERENT / HOST_CACHED`        check [vmaFindMemoryTypeIndex] for how VMA does it
</br>

- Allocate the buffer   ░░   we simply use vma`vmaCreateBuffer` for this    but could like do `vkCreateBuffer`, `vkGetBufferMemoryRequirements` and `vkAllocateMemory`
- MapMemory (if needed) ░░   GPU Memory needs to unmapped, vma can do that too `vmaMapMemory`
    - Now buffer on CPU (`CPU_ONLY` in **vma**) wouldn't need unmapping as per say but vma handles that
- Use memcpy?           ░░   After a `vmaMapMemory` we can use memcpy cz it actually ends up giving you a RAM-piece
- then unMapMemory      ░░   now the given RAM-piece was filled with memcpy, you can push it back to GPU    via PICe?
- Or you could just create PERSISTANTLY MAPPED MEMORY  \see 'maybe_U_wanna_keep_diving0'
</br>

- How many Buffers to Create? When to Create?
    - [Don't See this rn, think, best way to create buffer if you got all index, vertex & uniform data needed to be passed](https://developer.nvidia.com/vulkan-memory-management)
</br>

- Extras to look at
    - [maybe_U_wanna_keep_diving0](https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/memory_mapping.html)
    - [maybe_U_wanna_keep_diving1](https://computergraphics.stackexchange.com/questions/6081/how-does-id3d12resourcemap-work)
    - [maybe_U_wanna_keep_diving2](https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12resource-map)
    - [maybe_U_wanna_keep_diving3](https://docs.microsoft.com/en-us/windows/win32/direct3d12/memory-management-strategies)
    - [maybe_U_wanna_keep_diving4](https://asawicki.info/articles/memory_management_vulkan_direct3d_12.php5)
    - [optimization_go_1](https://stackoverflow.com/questions/27590055/cpu-to-gpu-memory-transfer-cudamemcpy-vs-direct3d-dynamic-resource-with-map)
    - [opt_go_1_extras](https://developer.nvidia.com/blog/introducing-low-level-gpu-virtual-memory-management/)
    - Google Search: vulkan map vs CUDAmemcpy
    - Google Search: Vulkan Map unMap alternative
    - Google Search: Mapping and Unmapping Memory d3d
    - Now this is where you first Encounter    hmmmmm   'THIS SEEMS LIKE a BAD WAY which isn't OPTIMIZED'.... Thats what it proly takes for supporting literally everything Untill now, we only had swapchain, WI, Pipeline to worry.... and those were like 'okay we are writting commands' to use dedicated GPU parts only for that
    - [These people did a Comparison SOMEHOW between CUDA & VULKAN](https://ir.lib.uth.gr/xmlui/bitstream/handle/11615/50906/19425.pdf?sequence=1&isAllowed=y)
    - [SO U can do calcs with cuda but then present using VULKAN](https://github.com/NVIDIA/cuda-samples/blob/master/Samples/vulkanImageCUDA/vulkanImageCUDA.cu)
    - BCZ 'CUDA GUI' is not a concept that actually exists, so.
    - Google Search: vkMapMemory vs cudamemcpy
    - Fun FACT: I didn't read any of these;   and most probably I will start at the Bottom TWO LINKs one DAY
</br>

- So yeah after doing all this BULL-SHIT for more than an HOUR, I am fucked, You're fucked if you solely keep using VULKAN and think [You_got_the_fastest_thing_(not_alive)]
    - Flash, the fastest Man Alive....