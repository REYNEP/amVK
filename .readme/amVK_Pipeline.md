### We came up with a concept of 'PipeStore'.... its like a Store, where you can 'BUILD' different pipelines from.... so it stores the settings for you....

amVK_PipeStoreMK2
  |- amVK_GraphicsPipes
  |- amVK_ComputePipes [W.I.P.]

if its your first day/week in vulkan and you have reached VkPipeline.... then please don't be like I GOTTA KNOW EVERYTHING.... every bits from everywhere, by default amVK_GraphicsPipes is set to values that you may create VkPipeline, that can be used to render any kinda TRIANGLE based object.... options already exposed in amVK_GraphicsPipes should be really easy to understand [*over google searches maybe sometimes*], if smth is confusing let me know....

Thus, I decided to make this file, a Doc [Short + Details on how each Pipeline settings actually works.... and are meant for]

1. OG.VAO ['VertexArrayObject' in OpenGL] [VertexInputState Vulkan] ['Input Layout' DX11/DX12]
    - How, What, Where, When...? https://vkguide.dev/docs/chapter-3/triangle_mesh_code/
      - load a mesh/.obj into RAM [ex.TinyOBJLoader]
      - now you gotta alloc & copy/transfer from RAM to VRAM
      for some reason, VkGuide choose to use VMA_CPU_TO_GPU, [means writeable by CPU, readable by GPU]
      but that kinda memory can be really short.... like 256MB on modern RDNA AMD cards or RTX Nvidia Cards
      see amVK_Memory.md
      - each vertex may have color & normal & other stuffs attached to it....
      now these are whats called 'Attribute' just like in Blender's GeometryNodes Attributes
      - VIBindings & VIAttributes
      - not sure if attributes can be vec2 or bool or vec4.... how would it connect to VIAttributes....
      - without IndexBuffer, there will be duplicated vertices [along with all attributes] 
      </br>

    - (First 2 Minutes) [IndexBuffer - Brendan Galea [P16]](https://www.youtube.com/watch?v=qxuvQVtehII)
    - What I call this: *VertexInputInfo*
    - Tools: (1) VkCmdBindVertexBuffers.... (2) You gotta create that VertexBuffer on VRAM once, (3) [ShaderInput] VIAttributes location.
    (4) VkCmdBindIndexBuffers
    - alias: 
        - Vertex Array Object
        - Vertex Input State
        - Vertex Buffer Object? [VBO from OpenGL?] (well, 'VBO' is not a thing from specs. But the concept sure does exists. I mean, the VRAM Buffer of Vertices IS 'VertexBuffer', right....)
        - Note: VBO is related to this, but this is just the INFO structures.... not the VBO itself....
        - TAGS: VertexInputBinding, VertexInputAttribute, VertexBinding, VertexAttribute, VertexInputBuffer, Vertex Buffer, VertexBuffer, Vertex Input Fromat, VertexInputFormat
      </br>

    - **IN A GLANCE:** [*VkPipelineVertexInputStateCreateInfo*]
    - VIBindings: VertexInput 'Binding'
      - .binding   - ties to VkCmdBindVertexBuffers()
      - .stride    - literally size of a stride, xD.... [each vertex]
      - .inputRate - VK_VERTEX_INPUT_RATE_VERTEX/VK_VERTEX_INPUT_RATE_INSTANCE
    - VIAttributes: VertexInput 'Attribute'
      - .binding   - ties to VkCmdBindVertexBuffers() & VertexInput 'Binding'
      - .location  - 'layout (location = x) in ...' in Shader
      - .format    - e.g. Position (32bit float) x, y, z = 'VK_FORMAT_R32G32B32_SFLOAT'
      - .offset    - shouldn't exceed '.stride'
</br></br></br>


2. OG.VertexInputAssembly
    - [amVK_GraphicsPipes]var vertTopo: VK_PRIMITIVE_TOPOLOGY_[TRIANGLE/LINE/POINT/PATCH]_[LIST/STRIP]  +/-[_WITH_ADJACENCY]
    - primitiveRestartEnable: [W.I.P.]
      - I dont quite understand this
      - specs says it doesn't work on 'LIST' topologies ([unless] U use primitiveTopologyListRestart [VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT])
      - It only works on INDEXED draws [vkCmdDrawIndexed] sooooo, IndexBuffers
      - a exmaple would be 'INDEXED Triangle Strip', google has enough info on 'TriangleStrip'....
      - 🤔
      - either its like, each element in index buffer means where that element points into the vertexbuffer + the 2 vertices before, are what froms the triangle....
      e.g. indexBuf[10] = 4; means vertexBuf[4], vertexBuf[3], vertexBuf[2] will form a triangle....
      or its like a regular 'INDEXED Triangle List'
      - 😁
      - Note: I found some threads on the internet saying, 'even if its TRIANGLE_STRIP mixed with DrawIndexed'
      if the indexBuffer has say N elements, N-2 triangles can be drawn from it,

      - now here comes the part about 'primitiveRestartEnable'
      if you see here [P12]: https://web.engr.oregonstate.edu/~mjb/vulkan/Handouts/GraphicsPipeline.1pp.pdf
      'a special “index” indicates that the primitive should start over.' if **indexType** of *vkCmdBindIndexBuffer* was
      VK_INDEX_TYPE_UINT32, that special “index” = 0xFFFFFFFF,
      VK_INDEX_TYPE_UINT16, that special “index” = 0xFFFF,
      VK_INDEX_TYPE_UINT8,  that special “index” = 0xFF
      - this could mean 2 things tooo, what I was thinking at first....
      like after UINT8/16/32 gets reached the GPU would automatically start from the beginning
      or you can manually set the value of a IndexBuffer element e.g.  indexBuf[100] = 0xFF[/FF][/FFFF], and when the GPU reached that one, it loops from beginning

      - i still think the second option is whats happenning here....

    - **IN A GLANCE:** [*VkPipelineInputAssemblyStateCreateInfo*]
      - vertTopo [VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST]
      - primitiveRestartEnable [Loop through the same IndexBuffer], kinda like [maybe_faster] alt. to INSTANCES....
</br></br></br>




3. OG.Tessellation
    - I haven't yet tried this.... will do soon....
</br></br></br>




4. OG.Raster
    - .depthClampEnable        : normally, enabling depthClamping would disable 'Primitive Clipping', hmm, 🤔, i think that might be a good thing actually
    but do read the specs on this one, if you are gonna enable
    - .rasterizerDiscardenable : TURNS of Rasterizer... completely??? otherwise what would be the purpose of even this....
      - seems like it: [StackOF - Why turn off rasterization step?](https://stackoverflow.com/questions/42470669/when-does-it-make-sense-to-turn-off-the-rasterization-step)
      - *"means the triangles would never get drawn to the screen. You might enable this, e.g. if you’re only interested in the side effects of the vertex processing stages, such as writing to a buffer which you later read from."*
      - by ***vblanco20-1*** [vkGuideDev]
      </br>

    - .depthBias   : [DX11](https://docs.microsoft.com/en-us/windows/win32/direct3d11/d3d10-graphics-programming-guide-output-merger-stage-depth-bias), [really old nVidia page](https://developer.download.nvidia.com/cg/DepthBias.html), [where that nVidia page pointed](https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glPolygonOffset.xhtml), 
    - .cullMode    : VK_CULL_MODE_NONE/FRONT/BACK_BIT   also FRONT_AND BACK
    - .frontFace   : VK_FRONT_FACE_CLOCKWISE  [or]  COUNTER_CLOCKWISE
    spec also tells how its calculated
    - .polygonMode : VK_POLYGON_MODE_POINT/LINE/FILL/
    - .lineWidth   : smth to look at if you do VK_PRIMITIVE_TOPOLOGY_LINE
</br>

5. OG.DepthStencil
    - .depthTestEnable    : XD, 😉
    - .depthWriteEnable   : you can manipulate depth buffer from within a [shader?](https://stackoverflow.com/questions/42341879/how-can-i-write-a-different-value-to-the-depth-buffer-than-the-value-used-for-de)
      - *"depthWriteEnable allows the depth to be written."*
      - *"there are cases where you might want to do depth write, but without doing depthtesting; it’s sometimes used for some special effects."*
      - by ***vblanco20-1*** [vkGuideDev] 
    - .depthCompareOp     : VK_COMPARE_OP_NEVER/LESS/EQUAL/LESS_OR_EQUAL/GREATER/NOT_EQUAL/GREATER_OR_EQUAL/ALWAYS
    - .depthBoundsTestEnable : next 2 options  ['Depth Bounds Test']
    - .minDepthBounds     : ...
    - .maxDepthBounds     : ...
    - .stencilTestEnable  : XD, 😉 (2)
    - .front , .back      : VkStencilOpState
</br>

6. OG.ColorBlend
    - You can like make Transparent Stuffs here  [like a GlassWindow. Like the Vibrancy extension for  VSCODE 😉]
      - *even if the blending is just "no blend", we still do have to write to the color attachment....*
      - Logical Operation locks BLENDING.... [https://www.khronos.org/opengl/wiki/Logical_Operation]
      - print & read: https://www.glprogramming.com/red/chapter10.html
      - idk, i searched for 'logic operation Color Blending' & a lot of stuffs popped up
      - https://docs.unity3d.com/430/Documentation/Components/SL-Blend.html
      - https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkLogicOp.html
      - https://vulkan.lunarg.com/doc/view/1.2.189.2/windows/chunked_spec/chap29.html#framebuffer-logicop

    - .flags         : theres a option for ARM architecture
    - .logicOpEnable : 😄, I'm sure you get this one
    - .logicOp       : e.g. VK_LOGIC_OP_CLEAR

</br></br></br>




7. OG.MSAA [Renderpass also has to support it]
    - .rasterizationSamples    : VK_SAMPLE_COUNT_1/2/4/8/16/32/64_BIT
    - .sampleShadingEnable     : Enable MultiSampling ???
    there is another way [enable?](https://www.khronos.org/registry/vulkan/specs/1.3-extensions/html/vkspec.html#primsrast-sampleshading)
    - .minSampleShading        : minimum fraction of the sample shading
    SampleRateShading: https://docs.gl/gl4/glMinSampleShading  [got this website form TheCherno]
    - .pSamleMask              : for 'Sample Mask Test', IDK what that is yet....
    - .alphaToCoverageEnable   : other stuffs to find out what these do
    - .alphaToOneEnable        : same....
</br>

8. OG.DynamicViewportNScissor [amVK_GraphicsPipes] default is 'DYNAMIC' but you can set your own too, 😉 [there is a way]
    - MultiView
      - [amVK_GraphicsPipes] has viewportCount & scissorCount as variables...
      - but [VK_KHR_multiview] isn't about those, i think.... e.g. [StereoScopic Example](https://www.saschawillems.de/blog/2018/06/08/multiview-rendering-in-vulkan-using-vk_khr_multiview/)
      see the [nvidia post](https://developer.nvidia.com/blog/turing-multi-view-rendering-vrworks/), I found on [VK_EXT_multiview](https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/VK_KHR_multiview.html) page 
      - ["A geometry shader is used to output geometry to multiple viewports in one single pass" – Jherico](https://gamedev.stackexchange.com/questions/161793/vulkan-unable-to-render-using-multiple-view-ports#comment286083_161793)
      - read this first: a really good post on VK_KHR_multiview vs multiple viewports [viewportCount] a.k.a called 'Viewport Array'
      https://stackoverflow.com/a/57312456
      - two multiview problems [still an issue as of 2022] in imGUI: [issue3669](https://github.com/ocornut/imgui/issues/3669), [issue1542](https://github.com/ocornut/imgui/issues/1542)
      - it feels like with multiViewport [or multiview EXT], you can render OBJECTS tied to a Pipeline in multiple viewport before switching the pipeline...

    - DYNAMIC is really good, no Impaaaaactttt.... but [trust a random guy on the internet](https://www.reddit.com/r/vulkan/comments/g6c4iz/comment/foshogz)

    - **IN A GLANCE:** [*VkPipelineViewportStateCreateInfo*]
    - being Dynamic you can just do 
    - [**vkCmdSetViewport**] & [**vkCmdSetScissor**]
</br>

9. OG.DynamicStates
    - VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR is default
</br></br></br>


10. Additional Stuffs:
    - INSTANCING: vkCmdDraw or vkCmdDrawIndexed has options for instancing....
    - Transparency: See OG.ColorBlend
    - wireframe drawing: see OG.Raster.lineWidth & polygonMode
    - Point drawing: use gl_PointSize in shader  & polygonMode  [change vertTopo]