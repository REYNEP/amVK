#include "amVK_Pipeline.hh"
#include "amVK_Device.hh"

/** //GIT_DIFF_FIX
 *   █▀ █░█ ▄▀█ █▀▄ █▀▀ █▀█   █▀▄▀█ █▀█ █▀▄ █░█ █░░ █▀▀
 *   ▄█ █▀█ █▀█ █▄▀ ██▄ █▀▄   █░▀░█ █▄█ █▄▀ █▄█ █▄▄ ██▄
 */ //GIT_DIFF_FIX
#include <fstream>
VkShaderModule amVK_PipeStoreMK2::load_ShaderModule(std::string &spvPath) {
    //spvCode
    uint64_t size; char *spvCode;
 //GIT_DIFF_FIX
    //READ
    if (spvPath.size() > 0) {
        /** open the file. With cursor at the end     TODO: Erorr Checking in DEBUG */
        std::ifstream spvFile(spvPath, std::ios::ate | std::ios::binary);
 //GIT_DIFF_FIX
        if (!spvFile.is_open()) {
            LOG("FAILED to OPEN .spv FILE: " << spvPath);
        }
 //GIT_DIFF_FIX
        size = static_cast<uint64_t> (spvFile.tellg());    //Works cz of std::ios::ate
        spvCode = static_cast<char *> (calloc(size, 1));
 //GIT_DIFF_FIX
        spvFile.seekg(0);
        spvFile.read(spvCode, size);
        spvFile.close();
    } else {LOG("spvPath's Size isn't more than 0"); return nullptr;}
 //GIT_DIFF_FIX
    //Create Shader module
    VkShaderModuleCreateInfo CI = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, nullptr, 0,     /** .flags: Reserved for Future, [Who knows what they will unveil in TIME] */
        size, reinterpret_cast<uint32_t *>(spvCode)
    };
 //GIT_DIFF_FIX
    VkShaderModule A_module;
    VkResult res = vkCreateShaderModule(_amVK_D->_D, &CI, nullptr, &A_module);
 //GIT_DIFF_FIX
    if (res != VK_SUCCESS) {amVK_Utils::vulkan_result_msg(res); LOG_EX("vkCreateShaderModule() failed"); return nullptr;}
    return A_module;
}
 
VkPipeline amVK_GraphicsPipes::build_Pipeline(void) {
    /**  \│/  ╔═╗┬ ┬┌─┐┌┬┐┌─┐┬─┐╔═╗┌┬┐┌─┐┌─┐┌─┐┌─┐
     *   ─ ─  ╚═╗├─┤├─┤ ││├┤ ├┬┘╚═╗ │ ├─┤│ ┬├┤ └─┐
     *   /│\  ╚═╝┴ ┴┴ ┴─┴┘└─┘┴└─╚═╝ ┴ ┴ ┴└─┘└─┘└─┘
     *      ---------------------------------------
     * We build it from a shader module & SAY-ing that it's a VERTEX_/FRAGMENT_BIT
     * 1 CreateInfo per ShaderStage for the Pipeline    [Don't think Multiple same SHADER can be used]
     */
    if (vert == nullptr) {
        LOG("amVK_GraphicsPipe:- No Vertex Shader was Provided");
    } else if (frag == nullptr) {
        LOG("amVK_GraphicsPipe:- No Fragment Shader was Provided");
    }

                 amVK_Array<VkPipelineShaderStageCreateInfo> ShaderStages = {};  ShaderStages.n = 2;
    ShaderStages.data = new VkPipelineShaderStageCreateInfo[ ShaderStages.n ];
    amVK_ARRAY_PUSH_BACK(ShaderStages) = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0,
        VK_SHADER_STAGE_VERTEX_BIT, vert, "main",
        nullptr
    };
    amVK_ARRAY_PUSH_BACK(ShaderStages) = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0,
        VK_SHADER_STAGE_FRAGMENT_BIT, frag, "main",
        nullptr
    };


    the_info.stageCount = ShaderStages.n;
    the_info.pStages = ShaderStages.data;
    the_info.layout = shaderInputs->layout;
    the_info.renderPass = _amVK_RP->_RP;
    the_info.subpass = 0;
    the_info.basePipelineHandle = VK_NULL_HANDLE;
 //GIT_DIFF_FIX
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
 
void amVK_GraphicsPipes::konfigurieren(void) {
    /**  \│/  ╦  ╦┌─┐┬─┐┌┬┐┌─┐─┐ ┬╦┌┐┌┌─┐┬ ┬┌┬┐╔═╗┌┬┐┌─┐┌┬┐┌─┐
     *   ─ ─  ╚╗╔╝├┤ ├┬┘ │ ├┤ ┌┴┬┘║│││├─┘│ │ │ ╚═╗ │ ├─┤ │ ├┤ 
     *   /│\   ╚╝ └─┘┴└─ ┴ └─┘┴ └─╩┘└┘┴  └─┘ ┴ ╚═╝ ┴ ┴ ┴ ┴ └─┘
     *      ---------------------------------------------------
     */ 
    OG.VAO = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO, nullptr, 0,
        VIBindings.n, VIBindings.data,
        VIAttributes.n, VIAttributes.data
    };
 
    /**  \│/  ╦  ╦┌─┐┬─┐┌┬┐┌─┐─┐ ┬╦┌┐┌┌─┐┬ ┬┌┬┐╔═╗┌─┐┌─┐┌─┐┌┬┐┌┐ ┬ ┬ ┬
     *   ─ ─  ╚╗╔╝├┤ ├┬┘ │ ├┤ ┌┴┬┘║│││├─┘│ │ │ ╠═╣└─┐└─┐├┤ │││├┴┐│ └┬┘
     *   /│\   ╚╝ └─┘┴└─ ┴ └─┘┴ └─╩┘└┘┴  └─┘ ┴ ╩ ╩└─┘└─┘└─┘┴ ┴└─┘┴─┘┴ 
     *      ------------------------------------------------------------
     * Triangle/Line/Point/Patch_List/Strip/Fan     [e.g. TRIANGLE_LIST, LINE_STRIP]
     */
    OG.VertexInputAssembly = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO, nullptr, 0,
        vertTopo, /** primitiveRestartEnable = */ VK_FALSE
    };





    /**  \│/  ╔╦╗┬ ┬┌┐┌┌─┐┌┬┐┬┌─┐╦  ╦┬┌─┐┬ ┬┌─┐┌─┐┬─┐┌┬┐    ┬    ╔═╗┌─┐┬┌─┐┌─┐┌─┐┬─┐
     *   ─ ─   ║║└┬┘│││├─┤│││││  ╚╗╔╝│├┤ │││├─┘│ │├┬┘ │    ┌┼─   ╚═╗│  │└─┐└─┐│ │├┬┘
     *   /│\  ═╩╝ ┴ ┘└┘┴ ┴┴ ┴┴└─┘ ╚╝ ┴└─┘└┴┘┴  └─┘┴└─ ┴ ───└┘─── ╚═╝└─┘┴└─┘└─┘└─┘┴└─
     *      --------------------------------------------------------------------------
     */
    OG.DynamicViewportNScissor = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO, nullptr, 0,
        viewportCount, nullptr, viewportCount, nullptr   /** viewportCount [def: 1], pViewports, scissorCount [def: 1], pScissors */
    };

    /**  \│/  ╔╦╗┬ ┬┬ ┌┬┐┬╔═╗┌─┐┌┬┐┌─┐┬  ┬┌┐┌┌─┐  ┌─╔╦╗╔═╗╔═╗╔═╗─┐
     *   ─ ─  ║║║│ ││  │ │╚═╗├─┤│││├─┘│  │││││ ┬  │ ║║║╚═╗╠═╣╠═╣ │
     *   /│\  ╩ ╩└─┘┴─┘┴ ┴╚═╝┴ ┴┴ ┴┴  ┴─┘┴┘└┘└─┘  └─╩ ╩╚═╝╩ ╩╩ ╩─┘
     *      -------------------------------------------------------
     */
    bool sampleShadingEnabled = false; if (samples > 1) {sampleShadingEnabled = true;}
    OG.MSAA = {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, nullptr, 0,
        (VkSampleCountFlagBits)samples, sampleShadingEnabled, minSampleShading, nullptr, VK_FALSE, VK_FALSE
    };



    /**  \│/  ╦═╗┌─┐┌─┐┌┬┐┌─┐┬─┐┬┌─┐┌─┐┌┬┐┬┌─┐┌┐┌
     *   ─ ─  ╠╦╝├─┤└─┐ │ ├┤ ├┬┘│┌─┘├─┤ │ ││ ││││
     *   /│\  ╩╚═┴ ┴└─┘ ┴ └─┘┴└─┴└─┘┴ ┴ ┴ ┴└─┘┘└┘
     *      --------------------------------------
     */
    OG.Raster = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, nullptr, 0,
        /** depthClampEnable */ VK_FALSE, rasterizerDiscardEnable,
        VK_POLYGON_MODE_FILL, 
        VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE,  /** no backface cull */
        VK_FALSE, 0.0f, 0.0f, 0.0f,     /** depthBiasEnable, ConstantFactor, depthBiasClamp, SlopeFactor */
        1.0f,     /** lineWidth: connected with POLYGON_MODE */
    };

    /**  \│/  ╔╦╗┌─┐┌─┐┌┬┐┬ ┬ ╔═╗┌┬┐┌─┐┌┐┌┌─┐┬┬  
     *   ─ ─   ║║├┤ ├─┘ │ ├─┤ ╚═╗ │ ├┤ ││││  ││  
     *   /│\  ═╩╝└─┘┴   ┴ ┴ ┴ ╚═╝ ┴ └─┘┘└┘└─┘┴┴─┘
     */
    OG.DepthStencil = {VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO, nullptr, 0,
        depth, depth, /** depthTestEnable, depthWriteEnable */
        VK_COMPARE_OP_LESS_OR_EQUAL,
        VK_FALSE,           /** depthBoundsTestEnable */
        VK_FALSE, {}, {},   /** STENCIL \todo WIP */
        0.0f, 0.0f    /** minDepthBounds, maxDepthBounds: connected with 2 lines before */
    };

    /**  \│/  ╔═╗┌─┐┬  ┌─┐┬─┐╔╗ ┬  ┌─┐┌┐┌┌┬┐
     *   ─ ─  ║  │ ││  │ │├┬┘╠╩╗│  ├┤ │││ ││
     *   /│\  ╚═╝└─┘┴─┘└─┘┴└─╚═╝┴─┘└─┘┘└┘─┴┘
     */
    VkPipelineColorBlendAttachmentState NoBlendAttach = {};
        NoBlendAttach.colorWriteMask = colorWriteMask;
        NoBlendAttach.blendEnable = blendEnable;
 
    OG.ColorBlendStates = {NoBlendAttach};
    OG.ColorBlend = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO, nullptr, 0,
        VK_FALSE, VK_LOGIC_OP_COPY,
        OG.ColorBlendStates.n, OG.ColorBlendStates.data,
        {0.0f, 0.0f, 0.0f, 0.0f}    /** .blendConstants */
    };


    /**  \│/  ╔╦╗┬ ┬┌┐┌┌─┐┌┬┐┬┌─┐╔═╗┌┬┐┌─┐┌┬┐┌─┐
     *   ─ ─   ║║└┬┘│││├─┤│││││  ╚═╗ │ ├─┤ │ ├┤ 
     *   /│\  ═╩╝ ┴ ┘└┘┴ ┴┴ ┴┴└─┘╚═╝ ┴ ┴ ┴ ┴ └─┘
     */
    OG.DynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    OG.DynamicWhat = {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO, nullptr, 0,
        OG.DynamicStates.n, OG.DynamicStates.data
    };





    the_info = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO, nullptr, 0,
        0, nullptr, /** stageCount, pStages.... \see build_pipeline(); */
        &OG.VAO,
        &OG.VertexInputAssembly,
        nullptr,    /** \todo Tessellation */
        &OG.DynamicViewportNScissor,
        &OG.Raster,
        &OG.MSAA,
        &OG.DepthStencil,
        &OG.ColorBlend,
        &OG.DynamicWhat,
        shaderInputs->layout,
        _amVK_RP->_RP,
        0,          /** .subpass */
        nullptr, 0  /** .basePipelineHandle, .basePipelineIndex */
    };
}



/** 
 *   █▀█ █ █▀█ █▀▀ █░░ █ █▄░█ █▀▀   █░░ ▄▀█ █▄█ █▀█ █░█ ▀█▀
 *   █▀▀ █ █▀▀ ██▄ █▄▄ █ █░▀█ ██▄   █▄▄ █▀█ ░█░ █▄█ █▄█ ░█░
 */ 
void ShaderInputsMK2::create_PipelineLayout(amVK_Device *amVK_D) {
    /** TrianglePipeLineLayout, May be needed in So many other Vulkan Functions */
    VkPipelineLayoutCreateInfo CI{};
      CI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      CI.pNext = nullptr;
    /** empty defaults */
      CI.flags = 0;

    amVK_Array<VkPushConstantRange> pushConsts = ref_PushConsts();
    amVK_Array<VkDescriptorSetLayout> descSets = ref_DescSets();
      CI.setLayoutCount = descSets.n;
    /** bcz our shader has no inputs, but we will soon add something to here. */
      CI.pSetLayouts = descSets.data;
      CI.pushConstantRangeCount = pushConsts.n;
      CI.pPushConstantRanges = pushConsts.data;

    VkResult res = vkCreatePipelineLayout(amVK_D->_D, &CI, nullptr, &layout);
    if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res)); LOG_EX("vkCreatePipelineLayout() failed");}
  }