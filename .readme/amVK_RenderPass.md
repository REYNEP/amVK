<h1 align=center> amVK_RenderPass </h1>

---

> <h3 align=center> "its more about the SubPasses and not about 'RenderPass' hype that you get from hearing the name" </h3>

So, let's start with the story & the motivation to actually have something like `RenderPass`. unlike *PC GPUs,* mobile GPUs has the concept of tiled rendering.... so simply, its rendered 1 tile after 1 tile.

📝Why? bcz, accessing memory on mobile devices can be constly... so we instead could work on smaller sized areas at once. [NOTE: many gpus also has a small amount of shading cores]


So tiles... 🤔 how'd that affect the pipeline and shaders?

> For every different kinda object, if we have different shareds, we'll have different pipelines. Every pipeline must have ***Vertex Shader & Fragment Shader....*** we know that, right? 

SubPass isn't about ***Vertex Shader*** or ***Tessellation*** or ***Geometry Shaders.*** Its about ***Fragment Shader***  & ***Post/Pre Fragment Operation*** stuffs. How? its mainly about the Fragment Shader *(big)* inputs, such as images & other attachments, 

Pipeline is created based on a RenderPass, & the FrameBuffer (from Swapchain) has to be created for that same RenderPass.

- RenderPass is about Subpasses, e.g. A renderpass may have multiple subpasses
- Subpass 









https://www.youtube.com/watch?v=ch6161wvME8&t=920s

 * WHY Ties to Pipeline: https://stackoverflow.com/a/55992644   \todo Go Furthur, Invertigator....
 * Tiled-architecture GPUs need to "load" image\buffer from general-purpose RAM to "on-chip memory". When they are done they "store" their results back to RAM.



\note Attachments are what Ties RENDERPASS & Framebuffers Together
 * 
 * attachment_refs are How subpass knows about the Attachments    & ties with the_info.pAttachments index
* [vblanco20-1] [from: https://vkguide.dev/docs/chapter-1/vulkan_renderpass/]
 *     In Vulkan, all of the rendering happens inside a VkRenderPass. 
 *     It is not possible to do rendering commands outside of a renderpass, 
 *     but it is possible to do   Compute commands without them.
 * 
 * MUST-READ: https://gpuopen.com/learn/vulkan-renderpasses/      [They explain whats actually goin on, Explains SubPasses]
 * 
 *     The renderpass is a concept that only exists in Vulkan. 
 *     It’s there because it allows the driver to know more about the state of the images you render.  
 *     [HOW? Bcz of SubPasses, (thats only if... you do use them)]
 *        Now whaat can someone mean by 'state of the images'?   
 *        well if you have ever deffered rendering.... or Multiple Passes in Blender.... like Mist, Diffuse, Specular, Volume
 *        its like, in OpenGL People used to implement multiple FRAMEBUFFERS or IMAGEs for each of these Passes.... but this theory of PASSES is slightly by a bit different
 *          cz this one usually has like DEPTH or STENCIL as the other Passes.... then only if the Game Engine author wants the other Passes like from blender to be like that....
 *        
 *        Now each of these passes can depend on each other.... so comes again the idea of States.... if a pass is Done or is in Rendering process.... thats it, yeah....




























</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>









Here we goooooo!!!!!
The new idea of Vulkan.... which was originally introduced by Mobile [Tiled_GPU_Rendering] hardware ppl! But can also be used for MultiPass/Deferred-Rendering
I'm gonna start at 'Why RenderPass is not a bad idea | Why OpenGL/DirectX way of things weren't solid/cool enough' first!
Then gonna explain how the whole situation of `renderpass-framebuffers-attachments-swapchain(+theDefaultImagesCreatedByvkCreateSwapchainKHR)-ImageLessFrameBuffers-vkCmdBlitImg-dynamicViewportScissor(ForPipeline)-` Ties together....

Extras:
- https://www.reddit.com/r/vulkan/comments/n3269q/does_vk_khr_imageless_framebuffer_mean_that_we/     [But_vkCmdBlitImg_actuallySucks] bcz, personally I think that the OS should let the Graphics API just render directly to the FRAMEBUFFER, it should have support for the formats MONITOR have support for. It should then convert to electronic signals VIA the GPU and not do any CPU stuffs in the MIDDLE. its like, we as the VULKAN programmers are supposed to pass our Final FrameBuffer to the OS's RAM/VRAM.... But after that the only operation remaining is mixing that with the other windows, (Which I presume is also GREAT on the GPU) and sending the ENTIRE image to the MONITOR.... and in FULLSCREEN mode, you dont even need to do mixing with the other WINDOW-s
Somone from OS guys should come up with an extension to do it ALL on the GPU (+have FallBacks if errors occur, and bypass the DATA to CPU so that the SCREEN still works/Shows)


Thoughts:
- Its to bizzare that nobody explains the RELATIONS with freakin' any kind of EXAMPLES!!!!! its like the whole internet got its very own peices of Ideas.... like a puzzle and you need to piece it together to see the bigger picture, what the DEVS thought..... in reality nobody knows it all. cz even the AMD-NVIDIA-APPLE-Mobile guys can go like `Hey Brothas, whats up.... lets team up`    so different drivers implement differently..... 😊 you can never know what the hell is actually wrong with this one or the OTHER one....

- How the Swapchain-FrameBuffer relation holds is like.... swapchain exists there, so that it can handle the OS related HANDLES & stuffs, Data, gets tied to the actual BUFFER (whether its on the GPU or CPU), smone gotta do that Right? Now it fits right for the swapchain to make VkImages.... cz it opens up 'MAYBE's for optimizing those images     [PS_IamGonnaBenchMark_Depth/Stencil/Multipass/ImageCreationTiming_VS_Swapchain-Recreation-with-oldSwapchain]
Like, if I was the one making the GREAT FOKING OS and the GREAT FOKING DRIVER, I would definitely tie this together, have a VkImage that I Only created once, its only that the Image is recreated at LEVELs, if the Window MAXES out at (or becomes smaller that)   480p/720p/1080p/1440p/2160p/   only then the vkImage is created....    [AndIBet_OnlyimageReCreationDoesnt_/ShouldntHurt_atMYDriverLevel]
[OkayIneedtoShowyousomeExmaples] about hwo the other Drivers do it, [atLeastTheOpenSourceOnes....]

Fuck all that CRAP!!!!.... forget it forget it.... just know summerize it to a mental MODEL like `Swapchain Ties the OS Surface to VULKAN and the only gurantee that you get is YOU will be GIVEN the Image/s that you wanted by setting the CI`.... again its all based on what alll the VENDORS can all AGREE & GURANTEE on.... ^^cking D^cking ppls with T^^PPING ideas all around....

But yes, you get images.... [andMyGuessis_ItsmoreOptimized_ImageReCreations_than_RegularImageCreation]
and you can use the Images to Create the FRAMEBUFFER

- Now that was SWAPCHAIN part of it.... (and it wasn't even 20% of the IDEA of whole SWAPCHAIN.... haha) Now we finally come to the FRAMEBUFFERS part of it.... Framebuffers are the REPRESENTATION of 'Renderable Images'      DOes it specifically mean the FInal Thing the GPU Renders to?
Naada, in different GPUs its different, cz there is that IDEA of Multipass....    some will do like TILED RENDERING where the  MENTAL IMAGE is

FRAMEBUFFER is just a smaller _boxed tile_ from the BIGGER-PICTURE. but it contains all the AttachmentImages of the MULTI-PASS [if_We_have_that]

or [NonTiledGPUs] FRAMEBUFFER is the Entire MULTIPASS stuffs.... all the Attachments if the VRAM is BIG Enough for all those to fit in and GPU doesnt have to GO Fetch from the LAST CACHE of VRAM or maybe even RAM 

or [NotUsingDeferredRendering] FRAMEBUFFER does have any Attachments other than the FINAL Color attachment, Not even any DEPTH-Attachment.... in that case, yeah it can be said to be 'JUST the GPU-VISUAL representation of where we will be Rendering to'

now to understand the 2nd one, ENTIRE MULTI-PASS    what I mean is.... for MULTIPass/Deferred Rendering in OPENGL you had to create MultiPle FrameBuffers and Render From one to ANOTHER....   [Watch what happens on QUALCOMM mobile GPUS 😉: https://www.youtube.com/watch?v=mpL4lVdhoVo&t=4m30s] now iGPUs does that.... but not the BIG NVIDIA ones with really High amount of VRAM?     Well there's still the idea of L1-L2-L3 cache in the GPU Memory too....


- RenderPass-Framebuffers?   Does it really tie together....? Does it Need to?    in most cases (AMDGpuOpenDrivers & NVIDIA) it doesnt'.... ref: [https://community.khronos.org/t/why-does-vkframebuffer-need-vkrenderpass-when-created/104461]
    i Mean this guy just questioned out of curiosity, that there is a `vkCmdBeginRenderPass` already where we use rpInfo.frameBuffer.... and till the `vkCmdEndRenderPass` that framebuffer is used....

- Here comes the IDEA of IMAGELESS-NESS.....

- 1 MORE Thing First: It is undefined. Undefined means that you, as the programmer, should not have expectations about the contents. Vulkan is a specification, making the results undefined here is meant to give freedom to the implementation to do whatever it deems best REF: [https://www.reddit.com/r/vulkan/comments/a24l2o/comment/eav4ccn/?utm_source=share&utm_medium=web2x&context=3]