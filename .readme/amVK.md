<!--
  [Cut out on SEPT22 from amVK.cpp]



 * WORKFLOW in a GLANCE [if you want pictures (for now) https://www.khronos.org/assets/uploads/developers/library/2016-vulkan-devu-seoul/1-Vulkan-Tutorial_English.pdf]
 * MY ADVICE: FEEL FREE TO COLLAPSE FUNCTIONS, I Tried my best make private-functions independant.... each function is like a station & they just sets values to Variables listed below 
 * For now, call CreateInstance and CreateDevice. thats as far as you can get with this Shitty ass shyte amVK Library [6/26/2021]
 * 
 * ZERO: Instance Creation ------ You are just copy pasting & learning the Philosophy
 * D: RenderDOc Vulkan in 30 Minutes
 * D: VkStructures, their similarities
 * D: Why Vulkan
 * D: Vulkan Layers
 * D: Anything not hidden from the DEVS
 * D: CreateInfos
 * D: Enum Functions
 * D: [VK_KHR_win32_surface gives 'vkCreateWin32SurfaceKHR']
 * D: Learn to Read the VK_Specs
 * 
 * ONE: Device Creation --------- a Step furthur into prologue madness
 * D: Enum PhysicalDeivices, Exts, Features and it's qFamilies [cur. vk has 3 qfamilies, \see ref in amVK::createDevice]
 * D: everyInfo_PD PD{}; member var
 * D: Choose which queueFamily and How many queues to create on a LogicalDevice [To use >1 qFamilies i think the solution is to Create Multiple LogicalDevices, Ask a ques on official forum/Reddit]
 *    see 5.3.2 Queue Creation chapter. vkCreateDevice is when you need to specify qFamily and queues you are gonna use
 * D: VkDeviceCreateInfo.pEnabledFeatures for enabling Extra Features from a GPU
 * D: VkDeviceCreateInfo.ppEnabledExtensionNames for all the Exts that you are gonna use [eg: VK_KHR_SWAPCHAIN, RTX]
 * D: Finally Create the Device, its a Logical One, so you could create multiple in same Physical one, But i don't think anyone needs that
 * D: Fun fact, VkPhysicalDevice itself is an Pointer
 * 
 *     ONE-ONE: the VULKAN CI [Create Info]
 *              .sType ; [khr-reg]/vulkan/specs/1.2-extensions/man/html/VkStructureType.html]
 *              .pNext ; [TODO]
 *              .flags ; [mostly 'reserved for future'] - - - 
 * 
 * TWO: The PROLOGUE Starts! - (Single-Thread Still yet)
 * D: Remember the SurfaceExtension that you queried for vkCreateInstance? You'll need to create Surface for every window
 *    But thats not, something amVK does for you. You need to Manage your own Windows and Surface, Check out amGHOST or GLFW
 *    for Creating surface only thing you will need is amVK_CX::instance;
 * D: PRESENTATION, Don;t get bothered by the word, I was really bothered by it and lost months of Time
 *    it simply, Means that, when GPU PRESENTs [or simply rather SEND PixelValues] to a Fking SURFACE
 *    Annnnnd, thats just it, PRESENTATION SUPPORT [like HULK says, YEEEES! Time-Travel.... What, I see this as an Absolute WIN]
 *    Have Some Hope
 * D: SWAP_CHAIN
 *    [WATCH Brendan Galea's Video on Swapchain, Read TODO: Update this Swapchain DOCS]
 *    Not all graphics cards are capable of presenting images directly to a screen for various reasons, e.g cz they're designed for servers & don't have any display output.
 *    2ndly, since image presentation is heavily tied into the window system and the surfaces associated with windows, it is not actually part of the Vulkan core. 
 *    You have to enable the VK_KHR_swapchain device extension after querying for its support.
 *      - From Vulkan-Tutorial by Alexander Overvoorde
 *    
 *    Now before you Actually Get Into creating the SwapChain. You need to hear something out, [Chappy] (GO Lawrance GO [cobra Kai-ep1]):-
 *      When creating such CreateInfos like the Swapchain, you might get really confused.... Like I Got confused by the imageFormat and imageColorSpace
 *        - Now U'll most probably learn your lesson after you've gotten yourself into some case like that....
 *      Don't do that to yourself, try to Know what you're Actually doin'....
 *      + Don't Try to breakDown every single Stuff that the DRIVER does.... Sometimes its just not Worth it.... + You don't need to anyway
 *      + Sometimes you might 
 * 
 *    Now go on and Check out https://renderdoc.org/vkspec_chunked/chap34.html#_surface_queries, Get yourself an IDEA of what SURFACE_SUP looks like 
 *    Most of these is not about your choices, rather WHAT your DRIVER sup and gives you the options....      
 *      
 *    Now you can finally go and Check the functions inside amVK_WI.hh & amVK_WI.cpp
 *    Create a Swapchain and Check when I recreated the Swapchain in rtr.cpp (if you are confused about amGHOST usage)
 *      + e.g. When window is Resized or minimized swapchain ReCreation is required
 * 
 *  SIDE-QUEST: If you get really Exhausted, go Read this, use your Phone, Get some Sleep, Have some snacks, GIve time to your loved ones [better if you have a GirlFriend, someone that I never had the luck for, yet], maybe go to movies with that someone, 
 *             Then come back fresh minded after some time, Bcz what you have done was great, and If you did it in a Week, go buy yourself a Friggin' big 20' Pizza or smth, You'ld deserve it
 *             THIS: jeremyong.com/c++/vulkan/graphics/rendering/2018/03/26/how-to-learn-vulkan/
 * 
 *  SIDE-QUEST: vblanco20-1 also has a small vkEngine which happens to do more than amVK does rn
 *             Go Check it out if you got a day to spare or understand what you did till this far
 *             THIS: https://github.com/vblanco20-1/VkEngine/blob/master/src/vulkan_init.cpp
 *             This Guy also Worked on THIS: https://github.com/godotengine/godot/issues/23998#issuecomment-497951825
 * 
 * 
 * 
 * 
 * 
 *   .oldSwapchain: Setting oldSwapChain to the saved handle of the previous swapchain aids in resource reuse and makes sure that we can still present already acquired images   
 *                      - [- Sascha Williems] 

 *  TBA: APIs Without Secrets update coming soon to Swapchain
 *       This guy seems really afraid of NO-SYNC or rathermore NO-FReAKING InPUT LaG....    so only tests  BuFFErING....
 *           https://github.com/KhronosGroup/Vulkan-Samples/blob/master/samples/performance/swapchain_images/swapchain_images_tutorial.md

 *  D: RenderPass    [MOTTO: its more about the SubPasses and not about'RenderPass' hype that you get from hearing the name]
 *        The renderpass is a concept that only exists in Vulkan. It’s there because it allows the driver to know more about the state of the images you render.
 *         A Renderpass will render into a Framebuffer. The framebuffer links to the images you will render to, and it’s used when starting a renderpass to set the target images for rendering.
 *         So a framebuffer and RENDERPASS is tied together throught the ATTACHMENTs  [VkRenderPassCreateInfo.pAttachments] & [VkFramebufferCreateInfo.pAttachments]
You can think of attachments as VkImages.... while the RenderPassCI will hold the AttachmentDescription Information only
and the actual Framebuffer will have VkImageViews as Attachment....    [Now VkImageView vs VkImage is a Different story, I personally think of this like GPU views the image through the ImageView..... like ImageView is a Portal & sm Description]

Its like what FasterThanLife said,
" The VkRenderPass is definitely something that takes some getting used to.  Essentially the renderpass is an orchestration of image data.  It helps the GPU better understand when you'll be drawing, what you'll be drawing to, and what it should do between render passes. I promise after hearing that, and looking at the code, things will start to make sense. "
[https://www.fasterthan.life/blog/2017/7/12/i-am-graphics-and-so-can-you-part-3-breaking-ground]




 *
 *         The general use of a renderpass when encoding commands is like this:

            vkBeginCommandBuffer(cmd, ...);

            vkCmdBeginRenderPass(cmd, ...);

            //rendering commands go here

            vkCmdEndRenderPass(cmd);

            vkEndCommandBuffer(cmd)
            When beginning a renderpass, you set the target framebuffer, and the clear color (if available). In this first chapter, we will change the clear color dynamically over time.







            https://docs.unity3d.com/2018.1/Documentation/ScriptReference/Experimental.Rendering.RenderPass.html



 * A Intro to RENDERPASS BY GPUOPEN: 
 " Among the features proposed by our mobile members was the renderpass — an object designed to allow an application to communicate the high-level structure of a frame to the driver. Tiling GPU drivers can use this information to determine when to bring data on and off chip, whether or not to flush data out to memory or discard the content of tile buffers and even to do things like size memory allocations used for binning and other internal operations. This is a feature that Mantle did not have, and is not part of Direct3D® 12 either. "


 There is also smth called DEFFERRED REndering for which people uses multiple Subpasses.... ever since Vulkan Came out
 But originally it was an Idea of Mobile Graphics Devs. For Tiles GPUs. Prolly cz smtimes too many attachments cant just fit into the L2 cache and there you have a huge Perf Penalty
 But Subpasses can be Interesting on Non-Tiled or rather    NEWER [after 2016 i think] Nvidia AND AMD GCN cards....

 But smtimes for Shadow Mapping or sm Post COMP Effects you will Need Multiple RENDER PASS [not MultiPass, that one is 'Multiple Subpasses']
 SRC: https://www.reddit.com/r/vulkan/comments/ime12i/multiple_renderpass/
 In OpenGL: https://www.reddit.com/r/vulkan/comments/81d126/comment/dv2d1t0/?utm_source=share&utm_medium=web2x&context=3


 *
 *

   [OCT2 - a Good Presentation: https://on-demand.gputechconf.com/gtc/2016/events/vulkanday/Vulkan_Overview.pdf]






   MSAA [If you are confused about how the MSAA ties to RENDERPASS]
   https://www.youtube.com/watch?v=pFKalA-fd34    [OpenGL MSAA ++ \brief of other AA like FXAA, SMAA]
   A Cool Intro https://on-demand.gputechconf.com/gtc/2016/events/vulkanday/Vulkan_Overview.pdf    And Other 2016 GTC VulkanDAY [Vulkan Training Day SLIDES]
   https://developer.apple.com/videos/play/tech-talks/606/    [Mantle on A11 chip]
   
   So, vkCreateSwapchain creates the Images.... and we dont plug in our RenderPass to that, But we need 4X more Buffer/Memory for MSAA x4 [ref: ]
   How do we plug in this whole MSAA stuff? Do we need to create More framebuffer?   No
   Then we Need to Create Images?   But how?   and even where do we Plug it in?
   Well, We can create with vkCreateImage & vkCreateImageView     [yes imageView, cz we gonna plug that into FrameBuffer attachments]
   here goes a Thread;    [https://www.reddit.com/r/vulkan/comments/5yrzac/comment/destacc/]       This guy was also Confused like me....
   At Vulkan Everyone mostly is Confused, cz Smtimes under the hood implementation can be smth entirely Different..... say for smone just invents a Holy fuking new GPU
   That Can do MSAA 8x like Calculations without the need for Multiple Buffers....   [yes, MSAA is part of Rasterization that Happens before Fragment SHading stage]
   Then the Under the hood implementation would just Ignore your Newly created    imageViews & Images that you have created..... 
   I mean yes, it would just ignore it and not make use of it..... as a VULKAN Dev you yourself would have to KNow about it..... or smone from the DRIVER dev can come up with a cool new Extension Providing Information about their card and Support  ;-)    But thats on them and how much they care about the Open Source Community

   Anyway... Other than Plugging in that image into Framebuffer, as you are Plugging it in as an Attachment you also have to let the RenderPass know that you are Plugging in MSAA Image Attachment Stuffs....
   so there is VkSubpassDescription.pResolveAttachments     [as you can Also see in that Reddit Thread....]

   Whatever So much of a Big talk. Its completely Fine and OK if you dont agree.... I mean i am just a Kid who is just trying to find his way into the Big League.... so....

   ARM Recommended Settings on MSAA: https://developer.arm.com/documentation/101897/0200/fragment-shading/multisampling-for-vulkan



   And as Always Peace.... And Vulkan Tutorial is really much much much CONFUSING.... (Just check VkGuide on Multisampling or MultiPass.... that is Truely better)


  also you will need this for MultiSampling: https://github.com/SaschaWillems/Vulkan/blob/master/examples/multisampling/multisampling.cpp#L257



 *     In Vulkan, all of the rendering happens inside a VkRenderPass. 
 *     It is not possible to do rendering commands outside of a renderpass, 
 *     but it is possible to do   Compute commands without them.
 * 
 *     MUST-READ: https://vkguide.dev/docs/chapter-1/vulkan_renderpass/    [Don't Forget to check the IMAGE LAYOUT section]
 *     You can also Read this if you Want to: https://renderdoc.org/vkspec_chunked/chap9.html#renderpass
 *     also this https://stackoverflow.com/questions/39551676/confused-about-render-pass-in-vulkan-api
 *     Also This https://software.intel.com/content/www/us/en/develop/articles/api-without-secrets-introduction-to-vulkan-part-3.html
 * 
 *  SIDE-QUEST: A More Broad Overview https://gpuopen.com/learn/vulkan-renderpasses/    [still yet introductory in a sense]
 *     Well, this goddamn fucking RenderPass Thingy feels exactly like what I imagined it would be
 *     Tiling the hell outta the GPU, but still keeping order between them without SYNC-wait-wrk
 *     AMD & NVIDIA Doesn't make TILING GPUs?
 * 
 *   A Renderpass will render into a Framebuffer. The framebuffer links to the images you will render to, 
 *   and it’s used when starting a renderpass to set the target images for rendering, with this:
        vkBeginCommandBuffer(cmd, ...);
        vkCmdBeginRenderPass(cmd, ...);
    
 *   [A.K.A: Later you will need to let the FrameBuffer Know about RenderPass [Just check out FrameBuffer Creation Code first, what are you doing here just reading and reading and reading]
 * 
 *     [From the GPUOPEN:]
 *     In Vulkan, a renderpass object contains the structure of the frame. 
 *     In its simplest form, a renderpass encapsulates the set of framebuffer attachments, 
 *                                                     basic information about pipeline state and not much more. 
 *     However, a renderpass can contain one or more subpasses and information about how those subpasses relate to one another. 
 *     This is where things get interesting.
 * 
 * 
 *   Something from VkGuide to mention NOW:
 *      The renderpass is a concept that only exists in Vulkan. 
 *      It’s there because it allows the driver to know more about the state of the images you render.  
 *      [HOW? it's Bcz of SubPasses, (thats only if... you do use them)]s
 * 
 *  Back to Where vkGuide were:-
 *     First thing we start writing is the color attachment. 
 *     This is the description of the image we will be writing into with rendering commands.
 *     It's Really nothing more than that. But the vkSpec Glossary [https://renderdoc.org/vkspec_chunked/chap54.html#glossary] says somewhat different
 *     \see Attachment (Render Pass) & \see Color Attachments
 * 
 *     There can be multiple Color Attachments, We will need to link them with RenderPass's Attachments
 *     Along with the subpasses, using AttachMentReference
 * 
 *     But I don't think we will use Multiple cz.... it's more like description of img we'l be writing into with rendering commands.
 * 
 *  Now finally [Read this after you see the entire code to Create RenderPass]
 *     ABOUT IMAGE-LAYOUTS:
 *          Images in the GPU aren’t necessarily in the format you would expect. 
 *          For optimization purposes, the GPUs perform a lot of transformation and reshuffling of them into internal opaque formats. 
 *          For example, some GPUs will compress textures whenever they can, and will reorder the way the pixels are arranged so that they mipmap better. 
 * 
 *       In Vulkan, you don’t have control of that, 
 *          but there is control over the layout for the image, which lets the driver transform the image to those optimized internal formats.
 * 
 *      If you get overwhelmed by so many IMAGE LAYOUTS and start wondering what to use.... Then
 *      Just don't be Overwhelmed....  [& listen to some ROCK bands like Linkin Park]
 *      Its Only about choosing the Most OPTIMAL one.... like [VK_IMAGE_LAYOUT_GENERAL could be used anywhere.... But it's like HELLISHLY OPPOSITE: of OPTIMAL]
 * 
 * 
 *      [P.S. I like Chester's Pop songs too.... Heavy, I'm just talking to myself]
 * 
 * 
 *    MORE DETAILS on RENDER PASS Coming Soon
 *    CAN WE USE MUltiple of these? How to sync? When to Use even Multiple? WTF does exactly RenderPass Represent?
 *    https://renderdoc.org/vkspec_chunked/chap8.html#synchronization-semaphores-waiting has got something usefull about RENDERPASS, Seems like what I thought Exactly
 * 
 * 
 *  D: FRAMEBUFFERS
 *     This actually will just hold the Information about actual vkImages from the Swapchain and the RENDER-PASS together
 *     Letting the RenderPass access the vkImages ig....
 *     As the vkImages will change along with SWAPCHAIN-Upadtes.... You will need to Update these Framebuffers
 *     I mean you will need to ReCreate FrameBuffers too....
 * 
 * 
 * 
 *  D: Queues
 *     Sm stuffs about QUEUES: stackoverflow.com/a/55273688
 *          QUOTE: For many apps, a single "universal" queue is all they need. More advanced ones might have one graphics+compute queue, a separate compute-only queue for asynchronous compute work, 
 *                 and a transfer queue for async DMA. Then map what you'd like onto what's available;
 *     if you are confused about queues and qFamilies: [https://stackoverflow.com/a/65086765]
 *     and yes if you think more queues = (idk, what you are thinking, just check this out): [https://www.reddit.com/r/vulkan/comments/kjb388/reason_for_only_creating_1_queue/ggw81mc]
 *     
 * 
 *  D: CommandPool & CommandBuffers
 *     a Good point to start: https://vkguide.dev/docs/chapter-1/vulkan_command_flow/
 *     SHORT: Unlike OpenGL or DirectX pre-11, in Vulkan, all GPU commands have to go through a command buffer. 
 *            Command buffers are allocated from a Command Pool, and executed on Queues.
 *            The general flow to execute commands is:
 *              - You allocate a VkCommandBuffer from a VkCommandPool
 *              - You record commands into the command buffer, using VkCmdXXXXX functions.
 *              - You submit the command buffer into a VkQueue, which starts executing the commands.
 *       [- vblanco20-1]
 * 
 *     It is possible to submit the same command buffer multiple times.
 * 
 * 
 *     CommandBuffer is the only way to MAKE the GPU work for you.... XD
 *     READ: https://vkguide.dev/docs/chapter-1/vulkan_command_flow/#vkcommandbuffer again
 *     AFTER: You have read my Pipeline State doc page that actually explains the IDEA of STATE-MACHINE
 *     TODO: Turn STATE DOC  into a Video on VULKAN PIPELINE
 * 
 * 
 * THREE: - INTO THE PROLOGUE:-    [Well You can watch the PROLOGUE now or after 2nd TIMEline your WISH!!!!]
 *   D:  SYNC & renderLoop Intro:
 *       https://vkguide.dev/docs/chapter-1/vulkan_mainloop/
 *       NVIDIA GDC Talk time
 *       Fench - CPU-GPU Sync         [Easiest one SO FAR]
 *       Semaphore - GPU-GPU sync     [can be UNSIGNALED after Signaled, ref: https://www.reddit.com/r/vulkan/comments/6pwuzd]
 *       Barrier - TBA
 *       Event - TBA
 * 
 *       In your MAIN Loop use:
 *          vkWaitForFences(device->_D, 1, &fenceOne, true, 1000000000);
 *          vkResetFences(device->_D, 1, &fenceOne);
 * 
 *       QueueSubmission & SEMA: https://renderdoc.org/vkspec_chunked/chap6.html#devsandqueues-submission
 *                               https://renderdoc.org/vkspec_chunked/chap8.html#synchronization-semaphores-waiting
 * 
 * 
 *    D: PIPELINE - Intro1
 *       -VkPipelineLayout is a must [even created (vkCreatePipelineLayout)  with   null/0   is okay....]
 *
 *        Having MANY amVK_GraphicsPipe would consume hell lot of Memory that we wont actually ever need.... mostly 99% stuffs will be COMMON between MESHES/OBJECTS 
 *        So, we create a BasePipe.... Change stuffs that varies, outside that 99% zone (e.g. vert, frag).... Those 1% stuffs can be stored   locally in these MESHES/OBJECT
 *        METAPHORE: you can think of amVK_GraphicsPipe   like a STORE of PIPES.... you take PIPE/s from there to build_pipeline() 
 * 
 * 
 * 
 *  FOUR:  - THE 2nd TIMELINE - [Movie hasn't actually started YET]
 *    D: VERTEX_BUFFER
 *       In Vulkan, you can allocate memory visible to the GPU, and read it from the shaders. You can allocate memory for two purposes, 
 *            1. for images We have already been using images a bit as part of the render pass and the swapchain, 
 *               but using them in the shaders can be complicated so we leave it for a later chapter. 
 *               Images are used mostly for 2d or 3d data, like textures, where you want to access any part of it from the shaders.
 * 
 *            2. The other type is BUFFERS,  and are just a bunch of memory that the GPU can see and write/read to. 
 *               There are multiple types of buffers, and they can be used in different ways. 
 *               vertex buffer  will allow the GPU to read the data from the buffer and send it to our vertex shader.
 * 
 *       To read a vertex buffer from a shader, you need to set the vertex input state on the pipeline. 
 *       This will let Vulkan know how to interpret a given buffer as vertex data. Once this is set up, 
 *       we will be able to get vertex information into the vertex shader, like vertex colors or vertex positions, automatically.
 * 
 *      Summing it up, say we wanna RENDER an actual OBJECT from CPU MEM, we Create VERTEX_BUFFER.... just another chunk of memory [to PASS TO GPU]
 *                     Then we USE VMA to Allocate & Upload the Buffer to the GPU    [then we might & can delete the CPU Data]
 *                     Finally we gotta let the GPU Know the FUCKING layout of VERTEX_BUFFER via Binding & Attribute DESC.... passing them to VertexInputState at PIPELINE
 *                                  [e.g. There can be 1 Binding But 3 inputs to SHADER.... or Each Binding per Input to Shader]
 * 
 *      Also 1 Pipeline can be used for Different VERTEX_BUFFERS as long as those VERTEX_BUFFERS [a.k.a different OBJECTS] has the SAME MATERIAL
 *      [https://www.reddit.com/r/vulkan/comments/e7n5b6/drawing_multiple_objects/]
 * 
 *    D: PUSH-CONSTANTS:
 *      https://stackoverflow.com/a/50956855
 *      Brendan Galea 09 - Push Constants
 *
 *
 *    D: Descriptor Sets:
 *       TODO


 D: STORAGE BUFFERS:
 Uniform buffers are great for small, read only data. But what if you want data you don’t know the size of in the shader? Or data that can be writeable. You use Storage buffers for that. Storage buffers are usually slightly slower than uniform buffers, but they can be much, much bigger. If you want to stuff your entire scene into one buffer, you have to use them. Make sure to profile it to know the performance.

 With storage buffers, you can have an unsized array in a shader with whatever data you want. A common use for them is to store the data of all the objects in the scene.

 We are going to use them to remove the usage of push-constants for the object matrices, which will let us upload the matrices at the beginning of the frame in bulk, and then we no longer need to do individual push constant calls every draw. This also will mean that we will hold all the object matrices into one array, which can be used for interesting things in compute shaders.
 * ---------------------
 * UNDER-THE-HOOD in a GLANCE
 */








  ADDITIONAL RESOURCES: github.com/jcoder58/VulkanResources
-->



# !TOP_PRIORITY
- TODO: Once you finish these Stuffs. Go Live. and Try to explain whats actually going on....
- TODO: REALTIME Viewport Comp https://www.youtube.com/watch?v=Cy8dopYFTqY with EEVEE
- TODO: Cleanup (like vkDestroyInstance)
- TODO: TURN  `[isn't nullptr]`   error loggings into   Memory Errors....   Introducing Types of ERROR

# !2ND_PRIORITY
- Add support for Multiple DEVICE [PRIORITY, Always Better to make use of All those Hardware Just Sitting there] & Instances [Even if it's NEVER Needed anyway]
- Add a Basic Template for Vulkan Layer [Has to Be Fun] & User VulkanLayers
- Add Support for User-Specified Exts & amGHOST_Presets     [InstanceExts_&_DeviceExts....] (e.g. Vulkan3D Preset, RT_Preset, ENC/DEC_Preset)
- Add support for user Choosing Device Features and ETC

# COOL_FEATURES
- Add docs for VULKAN Structs and CREATEInfos In particular
- Add Support for MIXING AMD and NVIDIA CARDS. [PROBLEMS: Different InstancExts, ]
- Docs & Extended BenchMarks on VARIABLE-PRESENT-MODE? I mean sometimes you can be sure that the frame you are gonna render is gonna take upto this amount of time. [VKSPEC_33.8.Display_Timing_Queries]
- Showing ALl the info like iep, iExts, PDinfo etc in a good UI kinda WAY, Maybe a NODE SYSTEM based?
- Document the DEPENDENCY Graph of FIles
- A UI for what vulkan.gpuinfo.com gives you, check their json files, Very Interesting

- [Maybe a way (UI) for ppl/devs to actually see vkDataTypes [e.g.PhysicalDeviceProps&_otherData], then in a NODE-LIKE way set up Logic....[e.g._IF_RayTracing_Sup] which can be executed by amVK to select stuffs and settings at runtime [e.g._Use_smth_like_JSON_to_READ_and_Match_Required_RayTracingSettings]]

- [a TEST Database.... so many stuffs in VULKAN is just 'NOT DEFINED' [e.g._qFamilies] smtimes there are DEDICATED qFamilies that only support 1 TYPE of QUEUES.... even then there would be ALL-SUPPORTING qFAMs.... So for these HAZY and FADED stuffs. we could create a DATABASE storing all GPUs Perfomance Information.... That has to be Always live.... for RUNTIME support.... more like extended version of that https://vulkan.gpuinfo.org/]

# MODULE_STYLES
- Functions that are still in BETA/ALPHA stages will have CAMEL-CASE style
- Use m_var_name for Member Variables [static members doesn't HAVE TO but can]
- AS LESS FUNCTIONS AS THERE CAN BE. eg. see stuffs in createInstance [KHR_Vulkan impl also does this]

# INVESTIGATION
- CALLOC vs MALLOC [For now we Fucking Malloc everywhere https://stackoverflow.com/questions/282926/time-complexity-of-memory-allocation and Calloc if 0-init needed]
- [REALLY_BIG_TASK] Scan through how blender Speed Ups happened
-  As of the ARCHIVE on JULY 21, it's Like It takes around 50MB RAM (Windows task Manager says GPU SHared memory increases like ~50-100MB) to just run a single fucking 1000x720 Windowed traingle [remember 3 SWAPCHAIN_IMAGES were Created]
- https://stackoverflow.com/questions/40185665/performance-cost-of-passing-by-value-vs-by-reference-or-by-pointer    [TEST: https://stackoverflow.com/a/22842701  another: https://gist.github.com/justbuchanan/a1594aead5fda4e6e526]       [GCC_AUTO_REF2VAL:https://stackoverflow.com/a/49523201]

# MISCELLANEOUS
- LET PPL decide which Vulkan-Api Version they want to Run on, and UPDATE DOCS on this File
- TIME-TRAVEL SplashScreen [ ++TVA ]
- Introduce Fatal Erorr instant Close with Message pop up
- make \section based \docs

# SAFE_ND_SOUND
- Make sure that No Variable was Declared Local to Function and then set the Memory Address to Class (Pointer)Variable 
- USE c++ style casts for as much places as possible, I do think calloc and Malloc can and should still be C-style casts   {+see Investigation#1}

- We calloc most of the Stuffs here, Prolly all VulkanStruct Pointers, REMEMBER TO FREE.... just wanted something BETTER THAN FUCKING VECTOR
- DISCUSS WHICH FORBIDDEN STUFFS SHOULD BE EXPOSED to PUBLIC API
-- Decide Wheather we should Expose some of FORBIDDEN VARIABLES in the CLASS as public/private.... maybe make some of these STATIC?