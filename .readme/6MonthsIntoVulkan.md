Concepts in Vulkan [Frustration]:
- Instance
- Device
- Swapchain
- RenderPass
- Framebuffer [Connects_RenderPass_&_SwapchainImgs]
</br>

Concepts in Vulkan [quid_pro_quo]
- Pipeline
- Shader
- Buffers
- Image
- CommandBuffers & Queues [+CommandPool]
- Synchronization [Semaphores_&_Fences]
</br>

STD Stuffs to Ignore
- Unordered_Map [https://www.youtube.com/watch?v=M2fKMP47slQ]
- std::sort & std::merge [https://www.youtube.com/watch?v=zqs87a_7zxw]
- I also try not to use VECTOR whereever I can
</br>

Fun Stuffs:
- https://www.fasterthan.life/blog/2017/7/11/i-am-graphics-and-so-can-you-part-1
- [https://liamhinzman.com/blog/vulkan-fundamentals]
- https://alain.xyz/blog/raw-vulkan
</br>

LEVEL-II [INTERMEDIATE]:
- [https://web.engr.oregonstate.edu/~mjb/whirlwind]
- https://www.youtube.com/watch?v=0R23npUCCnw
- [https://fgiesen.wordpress.com/2011/07/09/a-trip-through-the-graphics-pipeline-2011-index/]
- [https://zeux.io/2020/02/27/writing-an-efficient-vulkan-renderer]
- Synchronization, RayTracing, VideoEncDec soon....
- [https://www.vulkan.org/blog]
</br>
- Introduction to the Vulkan Computer Graphics API by Mike Bailey (SIGGRAPH 2020) (Video ain't that cool, [SLIDES](https://web.engr.oregonstate.edu/~mjb/vulkan/Handouts/FULL.1pp.pdf) are really fast
- [WIP](https://developer.nvidia.com/sites/default/files/akamai/gameworks/VulkanDevDaypdaniel.pdf) - I am gonna add the Slide 37 from there smtime soom on this README page
- [WIP](https://software.intel.com/content/www/us/en/develop/articles/api-without-secrets-introduction-to-vulkan-part-2.html) Will have to cover this too
- [WIP](https://www.jeremyong.com/c++/vulkan/graphics/rendering/2018/03/26/how-to-learn-vulkan/) Bold Lines from here.... and how he directly tells to start with SHADERS
</br>
- A Colorfull 2^2^2 page [List](https://www.khronos.org/files/vulkan11-reference-guide.pdf) of VK1.1 Functions
- ['Resources' Resource](https://inexor-vulkan-renderer.readthedocs.io/en/latest/links/main.html)
- another ['Resources' Resource](https://wiki.nikitavoloboev.xyz/computer-graphics/vulkan)

</br>

## Git Commands:
```shell
    git add .                   #'Stage' changes
    git restore --staged .      #revert Staged changes

    git commit                  #'Commit' Locally
    git reset HEAD~1 --soft     #reverse of commit,  still changed will be in Staged state

    git commit --amend          #add the newly Staged changed to this commit that wasn't pushed

    git push                    #github.com/REYNEP/amVK.git will be updated/downgraded by commits
    git pull                    #your repo will be updated/downgraded by commits

    git log
    git status
    git diff
    git show

    #smth about git https://github.blog/2020-12-17-commits-are-snapshots-not-diffs/
    #This is why you cant have manual diffs.... cz commits are snalshots not diffs
    #when you try to see a diff, it is calculated on your Processor locally, ig....
    #hell, i even tried this and knowledge from w3modding times https://git-scm.com/book/en/v2/Git-Tools-Interactive-Staging
```


## VSCODE
- If U R using VSCode /w C/C++ Ext (for VSCode), & you use CMake 
    - U'ld also install that CMakeTools Extension 
    - or U'll need to set includePath & defines in your [c_cpp_proprties.json](https://code.visualstudio.com/docs/cpp/c-cpp-properties-schema-reference) 
    (note: if you open a folder in vscode, that folder a.k.a. the Project can have a `.vscode` folder which can have `settings.json`, but `c_cpp_properties.json` is a C/C++ Extension specific file....)

- C/C++ Extension comes with its own EnhancedColorization option. But those are not Overridable using the VSCode API as of 2021 November. So I opened an [issue135599](https://github.com/microsoft/vscode/issues/135599)
    - Which was a total failure, By The Way....

> I came across a [Less-Contrasty] theme for VSCODE: [EvaTheme by fisheva](https://github.com/fisheva/Eva-Theme)
> OR, if you want Cool Contrasty themes, ask [vrecusko](https://github.com/vrecusko) 😉

- If you are searching on GITHUB, it Has a problem, Sometimes it won't show stuffs from BIG files when you search like vkCreateInstnace in trampoline.c

- ```"files.exclude": {
        "**/build": true
    },
    ```
    this disabled 'find Definition to search for stuffs in specific folders.... **/ means workspacefolder i guess'