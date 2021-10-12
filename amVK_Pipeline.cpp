#include <fstream> //CreateShaderModule() function
#include "amVK_Pipeline.hh"
#include "amVK_Device.hh"

VkPipeline amVK_GraphicsPipe::build_Pipeline(void) {


/**
 *
 *        ██████╗░░█████╗░██████╗░████████╗  ░░███╗░░
 *        ██╔══██╗██╔══██╗██╔══██╗╚══██╔══╝  ░████║░░
 *        ██████╔╝███████║██████╔╝░░░██║░░░  ██╔██║░░
 *        ██╔═══╝░██╔══██║██╔══██╗░░░██║░░░  ╚═╝██║░░
 *        ██║░░░░░██║░░██║██║░░██║░░░██║░░░  ███████╗
 *        ╚═╝░░░░░╚═╝░░╚═╝╚═╝░░╚═╝░░░╚═╝░░░  ╚══════╝
 *
*/
/**
 * |-----------------------------------------|
 *              - ShaderStages -
 * |-----------------------------------------|
 * We build it from a shader module & SAY-ing that it's a VERTEX_/FRAGMENT_BIT
 * 1 CreateInfo per ShaderStage for the Pipeline    [Don't think Multiple same SHADER can be used]
 */
    if (_vert == nullptr) {
        LOG("amVK_GraphicsPipe:- No Vertex Shader was Provided");
    } else if (_frag == nullptr) {
        LOG("amVK_GraphicsPipe:- No Fragment Shader was Provided");
    }

    uint32_t ShaderStagesN = 2;
    VkPipelineShaderStageCreateInfo ShaderStages[2] = {};
        ShaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        //ShaderStages[0].pNext = nullptr;
        //ShaderStages[0].flags = 0;
    /** STAGE_BIT */
        ShaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    /** VkShaderModule containing the code for this shader stage */
        ShaderStages[0].module = _vert;
    /** the entry point of the shader  [TODO: Add support for ShaderProgrammer's choice] */
        ShaderStages[0].pName = "main";
        //ShaderStages[0].pSpecializationInfo = nullptr;


        ShaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ShaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        ShaderStages[1].module = _frag;
        ShaderStages[1].pName = "main";



/**
 * |-----------------------------------------|
 *            - VertexInputState -
 * |-----------------------------------------|
 * \name VERTEX INPUT LAYOUT
 * contains the information for vertex buffers and vertex formats. 
 * This is equivalent to the VAO configuration on opengl
 */
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.pNext = nullptr;
        vertexInputInfo.flags = 0;

        vertexInputInfo.vertexBindingDescriptionCount      = _VIBindings.n;
        vertexInputInfo.pVertexBindingDescriptions         = _VIBindings.data;
        vertexInputInfo.vertexAttributeDescriptionCount    = _VIAttributes.n;
        vertexInputInfo.pVertexAttributeDescriptions       = _VIAttributes.data;


/** 
 * |-----------------------------------------|
 *          - VertexInputAssembly -
 * |-----------------------------------------|
 * Contains the configuration for what kind of topology will be drawn. 
 * This is where you set it to draw triangles, lines, points, or others like triangle-list.
 * On the info we just have to set boilerplate plus what kind of topology we want. Example topologies:
 *       VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST : normal triangle drawing
 *          VK_PRIMITIVE_TOPOLOGY_POINT_LIST : points
 *           VK_PRIMITIVE_TOPOLOGY_LINE_LIST : line-list
 */
    VkPipelineInputAssemblyStateCreateInfo vertexInputAssembly = {};
        vertexInputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        vertexInputAssembly.pNext = nullptr;
        vertexInputAssembly.topology = _vertTopo;
    /** we are not going to use primitive restart on the entire tutorial so leave it on false */
        vertexInputAssembly.primitiveRestartEnable = VK_FALSE;



/**
 * |-----------------------------------------|
 *            - [TBA TESSELATION] -
 * |-----------------------------------------|
 */















/**
 *
 *        ██████╗░░█████╗░██████╗░████████╗  ██████╗░
 *        ██╔══██╗██╔══██╗██╔══██╗╚══██╔══╝  ╚════██╗
 *        ██████╔╝███████║██████╔╝░░░██║░░░  ░░███╔═╝
 *        ██╔═══╝░██╔══██║██╔══██╗░░░██║░░░  ██╔══╝░░
 *        ██║░░░░░██║░░██║██║░░██║░░░██║░░░  ███████╗
 *        ╚═╝░░░░░╚═╝░░╚═╝╚═╝░░╚═╝░░░╚═╝░░░  ╚══════╝
 *
*/
/**
 * |-----------------------------------------|
 *            - ViewPortState -
 * |-----------------------------------------|
 */
    VkViewport _viewport;
    VkRect2D _scissor;
    /** From Top-Left Corner IG */
        _viewport.x = 0.0f;
        _viewport.y = 0.0f;
        _viewport.width = (float)_viewportExtent.width;
        _viewport.height = (float)_viewportExtent.height;
        _viewport.minDepth = 0.0f;
        _viewport.maxDepth = 1.0f;

        _scissor.offset = { 0, 0 };
        _scissor.extent = _viewportExtent;

    /** at the moment we won't support multiple viewports or scissors*/
    VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.pNext = nullptr;
        viewportState.flags = 0;

    /** Should be 1, if PhysicalDeviceFeatures.multiViewport not avail */
        viewportState.viewportCount = 1;
        viewportState.pViewports = &_viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &_scissor;


/**
 * |-----------------------------------------|
 *            - Rasterization -
 * |-----------------------------------------|
 */
    /** In here is where we enable or disable backface culling, and set line width or wireframe drawing. */
    VkPipelineRasterizationStateCreateInfo Rasterizer = {};
        Rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        Rasterizer.pNext = nullptr;
        Rasterizer.flags = 0;
        Rasterizer.depthClampEnable = VK_FALSE;
    /** 
     * if enabled, discards all primitives (triangles in our case) before the rasterization stage.... which we don't want
     * means the triangles would never get drawn to the screen. You might enable this, e.g.
     * if you’re only interested in the side effects of the vertex processing stages, such as writing to a buffer which you later read from. 
     * But in our case we’re interested in drawing the triangle, so we leave it disabled. 
     */
        Rasterizer.rasterizerDiscardEnable = VK_FALSE;

    /** toggle between wireframe and solid drawing */
        Rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        Rasterizer.lineWidth = 1.0f;
    /** no backface cull */
        Rasterizer.cullMode = VK_CULL_MODE_NONE;
        Rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

    /** no depth bias */
        Rasterizer.depthBiasEnable = VK_FALSE;
        Rasterizer.depthBiasConstantFactor = 0.0f;
        Rasterizer.depthBiasClamp = 0.0f;
        Rasterizer.depthBiasSlopeFactor = 0.0f;

    
/** 
 * |-----------------------------------------|
 *     - MultiSampling [MSAA] [TBA WIP] -
 * |-----------------------------------------|
 * allows us to configure MSAA for this pipeline. 
 * We are not going to use MSAA on the entire tutorial, so we are going to default it to 1 sample and MSAA disabled. 
 * If you wanted to enable MSAA, you would need to set rasterizationSamples to more than 1, and enable sampleShading. 
 * Keep in mind that for MSAA to work, your renderpass also has to support it, which complicates things significantly.
 */
    VkPipelineMultisampleStateCreateInfo Multisampling = {};
        Multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        Multisampling.pNext = nullptr;

        Multisampling.sampleShadingEnable = VK_FALSE;
    /** multisampling defaulted to no multisampling (1 sample per pixel) */
        Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        Multisampling.minSampleShading = 1.0f;
        Multisampling.pSampleMask = nullptr;
        Multisampling.alphaToCoverageEnable = VK_FALSE;
        Multisampling.alphaToOneEnable = VK_FALSE;















/**
 *
 *        ██████╗░░█████╗░██████╗░████████╗  ██████╗░
 *        ██╔══██╗██╔══██╗██╔══██╗╚══██╔══╝  ╚════██╗
 *        ██████╔╝███████║██████╔╝░░░██║░░░  ░█████╔╝
 *        ██╔═══╝░██╔══██║██╔══██╗░░░██║░░░  ░╚═══██╗
 *        ██║░░░░░██║░░██║██║░░██║░░░██║░░░  ███████╗
 *        ╚═╝░░░░░╚═╝░░╚═╝╚═╝░░╚═╝░░░╚═╝░░░  ╚══════╝
 *
*/
/**
 * |-----------------------------------------|
 *          - Depth-Stencil [TBA] -
 * |-----------------------------------------|
 */

/**
 * |-----------------------------------------|
 *              - ColorBlend -
 * |-----------------------------------------|
 * You can like make Transparent Stuffs here
 */
    VkPipelineColorBlendAttachmentState _colorBlendAttachment = {};
        _colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        _colorBlendAttachment.blendEnable = VK_FALSE;
    /** 
     * setup dummy color blending. We aren't using transparent objects yet
     * the blending is just "no blend", but we do write to the color attachment 
     */
    VkPipelineColorBlendStateCreateInfo colorBlending = {};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.pNext = nullptr;

        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &_colorBlendAttachment;

/**
 * |-----------------------------------------|
 *            - DynamicState [TBA] -
 * |-----------------------------------------|
 */











/**
 *
 *        ██████╗░░█████╗░██████╗░████████╗  ░░██╗██╗
 *        ██╔══██╗██╔══██╗██╔══██╗╚══██╔══╝  ░██╔╝██║
 *        ██████╔╝███████║██████╔╝░░░██║░░░  ██╔╝░██║
 *        ██╔═══╝░██╔══██║██╔══██╗░░░██║░░░  ███████║
 *        ██║░░░░░██║░░██║██║░░██║░░░██║░░░  ╚════██║
 *        ╚═╝░░░░░╚═╝░░╚═╝╚═╝░░╚═╝░░░╚═╝░░░  ░░░░░╚═╝
 *
*/   
/**
 * |-----------------------------------------|
 *            - Depth Stencil -
 * |-----------------------------------------|
 */
VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.pNext = nullptr;

    /** depthTestEnable holds if we should do any z-culling at all. Set to VK_FALSE to draw on top of everything, and VK_TRUE to not draw on top of other objects. 
     * depthWriteEnable allows the depth to be written. While DepthTest and DepthWrite will both be true most of the time, 
     * there are cases where you might want to do depth write, but without doing depthtesting; it’s sometimes used for some special effects. */
    depthStencil.depthTestEnable = true; /** bDepthTest ? VK_TRUE : VK_FALSE; */
    depthStencil.depthWriteEnable = true; /** bDepthWrite ? VK_TRUE : VK_FALSE; */

    /** The depthCompareOp holds the depth-testing function. Set to VK_COMPARE_OP_ALWAYS to not do any depthtest at all. Other common depth compare OPs are:-
     *  VK_COMPARE_OP_LESS (Only draw if Z < whatever is on the depth buffer), or VK_COMPARE_OP_EQUAL (only draw if the depth z matches) */
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL; /** bDepthTest ? compareOp : VK_COMPARE_OP_ALWAYS; */
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    /** STENCIL */
    depthStencil.stencilTestEnable = VK_FALSE;







/** 
 * |-----------------------------------------|
 *     - BUILD THE ACTUAL FKING PIPELINE -
 * |-----------------------------------------|
 * we now use all of the info structs we have been writing into into this one to create the pipeline
 * Not using TESSELATION, Depth-Stencil, & DynamicState 
 */
    if (_renderPass == nullptr) {LOG("RenderPass wasn't Given"); return nullptr;}
    VkGraphicsPipelineCreateInfo the_info = {};
        the_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        the_info.pNext = nullptr;
        the_info.flags = 0;

        the_info.stageCount = ShaderStagesN;
        the_info.pStages = ShaderStages;
        the_info.pVertexInputState = &vertexInputInfo;
        the_info.pInputAssemblyState = &vertexInputAssembly;
        the_info.pViewportState = &viewportState;
        the_info.pRasterizationState = &Rasterizer;
        the_info.pMultisampleState = &Multisampling;
        the_info.pColorBlendState = &colorBlending;
        the_info.pDepthStencilState = &depthStencil;
        the_info.layout = _layout;
        the_info.renderPass = _renderPass;
        the_info.subpass = 0;
        the_info.basePipelineHandle = VK_NULL_HANDLE;
        

    /** it's easy to error out on create graphics pipeline, so we handle it a bit better than the common VK_CHECK case */
	VkPipeline newPipeline;
	if (vkCreateGraphicsPipelines(_amVK_D->_D, VK_NULL_HANDLE, 1, &the_info, nullptr, &newPipeline) != VK_SUCCESS) {
		LOG("failed to create pipeline\n");
		return nullptr; /** TODO: Better Error Handling */
	}
	else
	{
		return newPipeline;
	}
}









/** 
 * 
 *   █▀ █░█ ▄▀█ █▀▄ █▀▀ █▀█   █▀▄▀█ █▀█ █▀▄ █░█ █░░ █▀▀
 *   ▄█ █▀█ █▀█ █▄▀ ██▄ █▀▄   █░▀░█ █▄█ █▄▀ █▄█ █▄▄ ██▄
 * 
 * You should compile any .vert or .frag and specify Full/Rel-path to EXE 
 */
VkShaderModule amVK_Pipeline::load_ShaderModule(std::string &spvPath) {
    //spvCode
    uint64_t size; char *buffer;

    //READ
    if (spvPath.size() > 0) {
        /** open the file. With cursor at the end     TODO: Erorr Checking in DEBUG */
        std::ifstream spvFile(spvPath, std::ios::ate | std::ios::binary);

        if (!spvFile.is_open()) {
            LOG("FAILED to OPEN .spv FILE: " << spvPath);
        }

        size = static_cast<uint64_t> (spvFile.tellg());    //Works cz of std::ios::ate
        buffer = static_cast<char *> (calloc(size, 1));

        spvFile.seekg(0);
        spvFile.read(buffer, size);
        spvFile.close();
    } else {LOG("spvPath's Size isn't more than 0"); return nullptr;}

    //Create Shader module
    VkShaderModuleCreateInfo the_info = {};
    the_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    the_info.pNext = nullptr;
    the_info.flags = 0; //Reserved for Futurue, [Who knows what they will unveil in TIME]
    the_info.codeSize = size;
    the_info.pCode = reinterpret_cast<uint32_t *>(buffer);

    VkShaderModule A_module;
    VkResult res = vkCreateShaderModule(_amVK_D->_D, &the_info, nullptr, &A_module);

    if (res != VK_SUCCESS) {amVK_Utils::vulkan_result_msg(res); LOG("vkCreateShaderModule() failed"); return nullptr;}
    return A_module;
}


/** 
 * 
 *   █▀█ █ █▀█ █▀▀ █░░ █ █▄░█ █▀▀   █░░ ▄▀█ █▄█ █▀█ █░█ ▀█▀
 *   █▀▀ █ █▀▀ ██▄ █▄▄ █ █░▀█ ██▄   █▄▄ █▀█ ░█░ █▄█ █▄█ ░█░
 * 
 * Alongside of all the State structs, we will need a VkPipelineLayout object for our pipeline. 
 * Unlike the other state structs, this one is an actual full Vulkan object, and needs to be created separately from the pipeline.
 * Pipeline layouts contain the information about shader inputs of a given pipeline. 
 * It’s here where you would configure your 
 *      push-constants &
 *      descriptor sets
 * but at the time we won’t need it, so we are going to create an empty pipeline layout for our Pipeline
 */
VkPipelineLayout amVK_Pipeline::new_PipelineLayout(uint32_t pushConstant_n, VkPushConstantRange *pushConstant_ranges, uint32_t descriptorSet_n, VkDescriptorSetLayout * descriptorSet_layouts) {
    /** TrianglePipeLineLayout, May be needed in So many other Vulkan Functions */
    VkPipelineLayoutCreateInfo pipeline_layout_info{};
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.pNext = nullptr;
    /** empty defaults */
        pipeline_layout_info.flags = 0;
        pipeline_layout_info.setLayoutCount = descriptorSet_n;
    /** bcz our shader has no inputs, but we will soon add something to here. */
        pipeline_layout_info.pSetLayouts = descriptorSet_layouts;
        pipeline_layout_info.pushConstantRangeCount = pushConstant_n;
        pipeline_layout_info.pPushConstantRanges = pushConstant_ranges;

    VkResult res = vkCreatePipelineLayout(_amVK_D->_D, &pipeline_layout_info, nullptr, &_layout);
    if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res)); LOG("vkCreatePipelineLayout() failed"); return nullptr;}
    else {return _layout;}
}