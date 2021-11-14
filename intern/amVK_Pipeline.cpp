#include "amVK_Pipeline.hh"
#include "glslang/SPIRV/GlslangToSpv.h"
#include "glslang/Public/ShaderLang.h"
#include <StandAlone/ResourceLimits.h>

/**
 *   █▀ █░█ ▄▀█ █▀▄ █▀▀ █▀█   █▀▄▀█ █▀█ █▀▄ █░█ █░░ █▀▀
 *   ▄█ █▀█ █▀█ █▄▀ ██▄ █▀▄   █░▀░█ █▄█ █▄▀ █▄█ █▄▄ ██▄
 */
#include <fstream>
VkShaderModule amVK_PipeStoreMK2::load_ShaderModule(std::string &spvPath) {
    //spvCode
    uint64_t size; char *spvCode;

    //READ
    if (spvPath.size() > 0) {
        /** open the file. With cursor at the end     TODO: Erorr Checking in DEBUG */
        std::ifstream spvFile(spvPath, std::ios::ate | std::ios::binary);

        if (!spvFile.is_open()) {
            LOG("FAILED to OPEN .spv FILE: " << spvPath);
        }

        size = static_cast<uint64_t> (spvFile.tellg());    //Works cz of std::ios::ate
        spvCode = static_cast<char *> (malloc(size));

        spvFile.seekg(0);
        spvFile.read(spvCode, size);
        spvFile.close();
    } else {LOG("spvPath's Size isn't more than 0"); return nullptr;}

    //Create Shader module
    VkShaderModuleCreateInfo CI = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, nullptr, 0,     /** .flags: Reserved for Future, [Who knows what they will unveil in TIME] */
        size, reinterpret_cast<uint32_t *>(spvCode)
    };

    VkShaderModule A_module;
    VkResult res = vkCreateShaderModule(_amVK_D->_D, &CI, nullptr, &A_module);

    if (res != VK_SUCCESS) {amVK_Utils::vulkan_result_msg(res); LOG_EX("vkCreateShaderModule() failed"); return nullptr;}
    return A_module;
}

#include <cstring>
VkShaderModule amVK_PipeStoreMK2::glslc_Shader(std::string &glslPath, amVK_ShaderStage stage, std::string cacheSPVPath, bool cache) {
    //glslCode
    uint64_t size; char *glslCode;

    //READ
    if (glslPath.size() > 0) {
        /** open the file. With cursor at the end     TODO: Erorr Checking in DEBUG */
        std::ifstream glslFile(glslPath, std::ios::ate | std::ios::binary);

        if (!glslFile.is_open()) {
            LOG("FAILED to OPEN FILE: " << glslPath);
        }

        size = static_cast<uint64_t> (glslFile.tellg());    //Works cz of std::ios::ate
        glslCode = static_cast<char *> (malloc(size));

        glslFile.seekg(0);
        glslFile.read(glslCode, size);
        glslFile.close();
    } else {LOG("glslPath's Size isn't more than 0"); return nullptr;}


    // .spv Output [WRITE....]
    if (cacheSPVPath[0] == '\0') {
        cacheSPVPath = glslPath + ".spv";
    }


    //Shader Stage
    if (stage == Shader_Unknown){
        uint32_t size = glslPath.size();
        const char *ptr_last_4_char = &glslPath[size-1 - 4];
             if(strcmp(ptr_last_4_char, "vert"))     stage = Shader_Vertex;
        else if(strcmp(ptr_last_4_char, "frag"))     stage = Shader_Fragment;
        else if(strcmp(ptr_last_4_char, "comp"))     stage = Shader_Compute;

        else if(strcmp(ptr_last_4_char, "geom"))     stage = Shader_Geometry;
        else if(strcmp(ptr_last_4_char, "tesc"))     stage = Shader_TesC;
        else if(strcmp(ptr_last_4_char, "tese"))     stage = Shader_TesE;
    }


    glslc_Shader(glslCode, stage, cacheSPVPath, cache);
}

VkShaderModule amVK_PipeStoreMK2::glslc_Shader(const char *glslCode, amVK_ShaderStage stage, std::string cacheSPVPath, bool cache) {
    //Shader Stage
    EShLanguage stage_glslang;
    switch (stage)
    {
        case Shader_Unknown:
        {
            LOG_EX("param 'stage' is Shader_Unknown.... it won't compile")
        }
        case Shader_Vertex:     stage_glslang = EShLangVertex;          break;
        case Shader_Fragment:   stage_glslang = EShLangFragment;        break;
        case Shader_Compute:    stage_glslang = EShLangCompute;         break;

        case Shader_Geometry:   stage_glslang = EShLangGeometry;        break;
        case Shader_TesC:       stage_glslang = EShLangTessControl;     break;
        case Shader_TesE:       stage_glslang = EShLangTessEvaluation;  break;

        case Shader_RT:         stage_glslang = EShLangRayGenNV;        break;
    }

    //The Shader
    glslang::TShader   shader_glslang(stage_glslang);
    shader_glslang.setStrings(&glslCode, 1);
    shader_glslang.setPreamble(this->shader_preamble.c_str());

    extras:
    {
        shader_glslang.setEnvInput( glslang::EShSourceGlsl, stage_glslang, glslang::EShClientVulkan, 120);  
        /** 120: "#define VULKAN 120" [https://github.com/godotengine/godot/blob/8f6c16e4a459b54d6b37bc64e0bd21a361078a01/modules/glslang/register_types.cpp#L53] */
        shader_glslang.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_2);
        shader_glslang.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_5);
    }

    





    // Compiled SPIR-V Binary
    std::vector<uint32_t> SpirV;
    compileShader:
    {
        EShMessages msg = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);
        const int DefaultVersion = 100;     //should this be like the '120' above? 🤔
        
        // wtf_is_this ? Preprocess? Why?
        preprocess:
        {
            std::string pre_processed_code;
            glslang::TShader::ForbidIncluder includer;  //why need this?

            //preprocess
            if (!shader_glslang.preprocess(&glslang::DefaultTBuiltInResource, DefaultVersion, ENoProfile, false, false, msg, &pre_processed_code, includer)) {
                LOG_EX("glslang::TShader->preprocess() failed.... "
                    << "\n    " << shader_glslang.getInfoLog()
                    << "\n    " << shader_glslang.getInfoDebugLog()
                );
            }
            //set back..
            const char *wtf_is_this = pre_processed_code.c_str();
            shader_glslang.setStrings(&wtf_is_this, 1);
        }

        //parse [for validating setEnv*** function inputs from earlier, ig.]
        if (!shader_glslang.parse(&glslang::DefaultTBuiltInResource, DefaultVersion, false, msg)) {
            LOG_EX("glslang::TShader->parse() failed.... "
                << "\n    " << shader_glslang.getInfoLog()
                << "\n    " << shader_glslang.getInfoDebugLog()
            );
        }




        glslang::TProgram program_glslang;
        program_glslang.addShader(&shader_glslang);

        /** https://github.com/KhronosGroup/glslang/blob/4302d51868daa94e81d3002073e9265397b2e444/glslang/MachineIndependent/ShaderLang.cpp#L1988 */
        if (!program_glslang.link(msg)) {
            LOG_EX("glslang::TProgram->Link() failed to link shaders.... "
                << "\n    " << program_glslang.getInfoLog()
                << "\n    " << program_glslang.getInfoDebugLog()
            );
        }
        
        finally_compile:
        {
            glslang::SpvOptions spvOptions;
            glslang::GlslangToSpv(*(program_glslang.getIntermediate(stage_glslang)), SpirV, &spvOptions);
        }
    }






    // .spv Output
    cache:
    {
        if (cache) {
            std::ofstream spvFile(cacheSPVPath, std::ios::trunc | std::ios::binary);

            if (!spvFile.is_open()) {
                LOG("FAILED to OPEN .spv FILE: " << cacheSPVPath);
            }

            spvFile.write(reinterpret_cast<char *>(SpirV.data()), SpirV.size() * 4);
        }
    }
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
        LOG("amVK_GraphicsPipe:- No Vertex Shader.");
    } else if (frag == nullptr) {
        LOG("amVK_GraphicsPipe:- No Fragment Shader.");
    } else if (shaderInputs == nullptr) {
        LOG_EX("amVK_GraphicsPipe::shaderInputs == nullptr;  No shaderInputs [a.k.a PipelineLayout]. \n" << 
               "create & assign 'new' ShaderInputsMK2 to it & create_PipelineLayout [before build_pipeline]");
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
        nullptr,    /** .layout, set in build_pipeline(); */
        _amVK_RP->_RP,
        0,          /** .subpass */
        nullptr, 0  /** .basePipelineHandle, .basePipelineIndex */
    };
}
















/** 
 *   █▀█ █ █▀█ █▀▀ █░░ █ █▄░█ █▀▀   █░░ ▄▀█ █▄█ █▀█ █░█ ▀█▀
 *   █▀▀ █ █▀▀ ██▄ █▄▄ █ █░▀█ ██▄   █▄▄ █▀█ ░█░ █▄█ █▄█ ░█░
 */ 
void ShaderInputsMK2::create_PipelineLayout(VkDevice D) {
    /** TrianglePipeLineLayout, May be needed in So many other Vulkan Functions */
    VkPipelineLayoutCreateInfo CI{};
      CI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      CI.pNext = nullptr;
    /** empty defaults */
      CI.flags = 0;

    /** ref_PushConsts() might return {valid_address, 0},    but bcz n is 0, valid_address will be ignored */
    amVK_Array<VkPushConstantRange> pushConsts = ref_PushConsts();
    amVK_Array<VkDescriptorSetLayout> descSets = ref_DescSets();
      CI.setLayoutCount = descSets.n;
    /** bcz our shader has no inputs, but we will soon add something to here. */
      CI.pSetLayouts = descSets.data;
      CI.pushConstantRangeCount = pushConsts.n;
      CI.pPushConstantRanges = pushConsts.data;

    VkResult res = vkCreatePipelineLayout(D, &CI, nullptr, &layout);
    if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res)); LOG_EX("vkCreatePipelineLayout() failed");}
}