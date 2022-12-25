<h3 align=center> 
https://www.youtube.com/watch?v=JWeJHN5P-E8
</h3>
<p align=center>Tune in to some music... cz you are relly gonna need it....</p>

```md
  Vulkan loves structs. I mean, really loves structs.
  Most of Vulkan code is simply filling out structs
  and submitting that to an API call with the necessary
  object handles attached
        - FasterThanLife, Part III

  \see amVK_InstanceMK2::set_VkApplicationInfo();

  https://www.youtube.com/watch?v=4HGpMAHMZ1w    .... Not English ;)
```

</br>
</br>
<p align=center>
    <img src="HardwareDiscovery.png" />
</p>

```md
  We got a amVK_InstanceMK2::load_PD_info(), which loads/enumerates
  - All Physical Devices
  - their Features, Properties, MemoryProperties
  - All PD QueueFamilies
  - \see    struct loaded_PD_info_internal   & amVK_InstanceMK2::SPD


  We also got a benchmarking system of our own, haha.... which is pretty much garbaj
      benchMark_PD()
  - The only thing that we actually did right... was that load_PD_info() part...
  - Anyway, from there anyone can just make their own benchmarking system... pretty easily
  - just make sure that you Create_VkInstance before....

  https://www.youtube.com/watch?v=K0A9f_cL_wU   .... Not English, again ;)
```






</br>
</br>
</br>
<p align=center>
    <img src="Queues.png" />
</p>

```md
  Whats the CONERN for you with QueueFamilies & Queues? really?
  Its kinda like, Queue is actually a queue of GPU Commands to be executed....
  - there can be Compute/Graphics/Transfer/SparseBinding/VideoEncode/Decode
  - Its just.... for some underlying architechtural differences....
  - e.g. a GPU might have CUDA/RayTracing/Tensor all kinds of cores.....
  - sooooo, the hardware people just decided to implement smth such as QueueFamilies
  - which is interesting in a way....
  - QueueFamilies have info about which Queue Types it supports... & some other info
```



</br>
</br>
</br>
<p align=center>
    <img src="WorkWorkWork.png" />
</p>

```md
  Now a Physical Device is just a Physical Device....
  Its there, so that you can query for the HARDWARE info....

  Now you create an actual VkDevice.... Which is the one that matters.... really
  
  Also note that there's also option for DeviceGroup, Which always felt to me like
  SLI/CrossFire such Bridge supports.... creating VkDevice combining multiple hardware pieces

  
  all the VkDevice related stuffs are separated, in amVK_Device.hh & amVK_Device.cpp files
  \see amVK_DeviceMK2
```


</br>
</br>
</br>

### Now you finally know everything about amVK.... there's 1 More last bits to it.... amVK's Conventions

```md
  - all the `the_info` variabes are public... check them out, you can modify on your own
  - every class like amVK_Surface, amVK_WI, amVK_Device, everything has modifiable public vars
  - through these public stuffs is how you can modify Vulkan....

  - I tried to expose all the available tweaks inside the class declaration.... elements

  - https://www.youtube.com/watch?v=Jqs5EaAaueA
```

























</br>
</br>
</br>
<p align=center>
    <img src="5DataTypes.png" />
</p>

### Now you may wanna stick with this Readme, for some last Bits of Desserts

</br>
</br>
<h2 align=center>
    <u>MEMORY</u>
</h2>
<p align=center>
    <img src="Memory.png" />
</p>

```md
  All your data starts off CPU side.  
  In a typical hardware setup both the CPU and GPU maintain a region of memory which can be mapped by the other device.  
  This makes sharing data between the two much easier.  
  Still it's your job ( using Vulkan ) to make it visible to the GPU. 

  - Its kinda like. First you copy from CPU_LOCAL (RAM) to GPU_LOCAL_BUT_CPU_VISIBLE (VRAM)

  --- GPU_LOCAL_BUT_CPU_VISIBLE    a.k.a   mixture of (HOST_VISIBLE_BIT | DEVICE_LOCAL_BIT)
  --- usually this one comes in a small stack     e.g.     around 200MB on GTX 1080

  - Then you use Transfer Commands to transfer from GPU_LOCAL_BUT_CPU_VISIBLE to GPU_LOCAL (actual VRAM)
 
  https://www.youtube.com/watch?v=Oj18EikZMuU - an Usual EDM one.... FUN when you are in a fast mood
```

</br>
<h2 align=center>
    <u>MEMORY PT II</u>
</h2>
<p align=center>
    <img src="MemoryTypes.png" />
</p>

```md
  Note that reading memory across the bus is obviously going to be slower so unless warranted it's best to avoid that whenever ( as long as ) possible.

  - This is how Vulkan organizes memory.
  - When you call vkGetPhysicalDeviceMemoryProperties you get back what amounts to a list of heaps and memory types.
  - When allocating memory, you'll look through this and find a best match for your purposes.
  - Note: 
    - There must be at least one memory type with HOST_VISIBLE_BIT and HOST_COHERENT_BIT set.
    & there must be at least one memory type with DEVICE_LOCAL_BIT set only.  

  - \see  amVK_InstanceMK2::SPD.mem_props
  - \todo add a better way to access

  https://www.youtube.com/watch?v=hh1WeQxfCX0
```

### Note: If you'd like to know more on this subject, head over to [gpuopen](https://gpuopen.com/learn/vulkan-device-memory/) for a nice write up by [Timothy Lottes](https://twitter.com/TimothyLottes) from AMD. [if it seems a bit hard, Soon I am trynna write down a Explanation on this]

<p align=center>
    <img src="MemoryHandling.png" />
</p>

```md
A Side Note

  Let me just interject real quick that recently Adam Sawicki over at AMD came out with a Vulkan Allocator that you can drag and drop into your game.
  As memory management is something "hairy" for a lot of people, just know there are options open to you.
  I have not had a chance to evaluate it yet, but it's great to have the contribution!

  [AMD Vulkan Allocator](http://gpuopen.com/gaming-product/vulkan-memory-allocator/)

  - Faster Than Life....

  Tho me, REYNEP... dont really like it.... that thing should have been easier.... with lots of docs.... on Deeper Hardware bits n infos
  https://www.youtube.com/watch?v=JWeJHN5P-E8
```

<p align=center>
    <img src="MemoryLimit.png" />
</p>

### Now just go ahead and check the other DOC files... But those are actually not from V2... so they suck... I am working on it - REYNEP

[ref: 'Working on it - fitz' - Agents of S.H.I.E.L.D]
