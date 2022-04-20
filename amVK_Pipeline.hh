#pragma once

#include <string>
#include "amVK_INK.hh"
#include "amVK_Device.hh"
#include "amVK_RenderPass.hh"

/**
 * \page
 * ShaderInputsMK2 [a.k.a PIPELINE LAYOUT]
 * amVK_PipeStoreMk2
 *  amVK_GraphicsPipes
 *  amVK_ComputePires [W.I.P]
 * 
 * some SPOTIFY links....
 * amVK_Pipeline ['Basically this is why, we came all this way....']
 */


/** 
 *   █▀█ █ █▀█ █▀▀ █░░ █ █▄░█ █▀▀  █░░ ▄▀█ █▄█ █▀█ █░█ ▀█▀
 *   █▀▀ █ █▀▀ ██▄ █▄▄ █ █░▀█ ██▄  █▄▄ █▀█ ░█░ █▄█ █▄█ ░█░
 * 
 * \todo SPIRV-Reflect: Parse DescriptorSet & PushConstant input information from (Compiled Shader) .spv binary 
 * How do you use this class? Well, its just a helper class around 'Create VkPipelineLayout'
 *  see example in RTC_2D_Quad
 * 
 * You won't need to have CI anymore after you vkCreatePipelineLayout, so thats STATIC
 * atleast, not unless you need to create the same Layout & Pipeline on different Devices at RANDOM times....   [idontthink set_Info can hurt in this case, so]
 */
class ShaderInputsMK2 {
 public:
  static inline VkPipelineLayout       S_layout = nullptr;
  static inline VkPipelineLayoutCreateInfo s_CI = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO, nullptr, 0};

  ShaderInputsMK2(void) {}
  ~ShaderInputsMK2(void) {}

  static inline void set_Info(uint32_t pushCount, const VkPushConstantRange *pPushConstantRanges, uint32_t setLayoutCount, const VkDescriptorSetLayout *pSetLayouts) {
    s_CI.pushConstantRangeCount = pushCount;
    s_CI.pPushConstantRanges = pPushConstantRanges;
    s_CI.setLayoutCount = setLayoutCount;
    s_CI.pSetLayouts = pSetLayouts;
  }

  /**
   * Destroy using vkDestroyPipelineLayout()
   */
  static inline VkPipelineLayout create_PipelineLayout(VkDevice D) {
    VkResult res = vkCreatePipelineLayout(D, &s_CI, nullptr, &S_layout);
    if (res != VK_SUCCESS) { 
      amVK_LOG_EX(amVK_Utils::vulkan_result_msg(res)); amVK_LOG_EX("vkCreatePipelineLayout() failed"); 
    }
    return S_layout;
  }
  static inline VkPipelineLayout create_PipelineLayout(amVK_DeviceMK2 *amVK_D) { return create_PipelineLayout(amVK_D->D);}
};


/**
 *              █████╗ ███╗   ███╗██╗   ██╗██╗  ██╗        ██████╗ ██╗██████╗ ███████╗██╗     ██╗███╗   ██╗███████╗        ███╗   ███╗██╗  ██╗██████╗ 
 *   ▄ ██╗▄    ██╔══██╗████╗ ████║██║   ██║██║ ██╔╝        ██╔══██╗██║██╔══██╗██╔════╝██║     ██║████╗  ██║██╔════╝        ████╗ ████║██║ ██╔╝╚════██╗
 *    ████╗    ███████║██╔████╔██║██║   ██║█████╔╝         ██████╔╝██║██████╔╝█████╗  ██║     ██║██╔██╗ ██║█████╗          ██╔████╔██║█████╔╝  █████╔╝
 *   ▀╚██╔▀    ██╔══██║██║╚██╔╝██║╚██╗ ██╔╝██╔═██╗         ██╔═══╝ ██║██╔═══╝ ██╔══╝  ██║     ██║██║╚██╗██║██╔══╝          ██║╚██╔╝██║██╔═██╗ ██╔═══╝ 
 *     ╚═╝     ██║  ██║██║ ╚═╝ ██║ ╚████╔╝ ██║  ██╗███████╗██║     ██║██║     ███████╗███████╗██║██║ ╚████║███████╗███████╗██║ ╚═╝ ██║██║  ██╗███████╗
 *             ╚═╝  ╚═╝╚═╝     ╚═╝  ╚═══╝  ╚═╝  ╚═╝╚══════╝╚═╝     ╚═╝╚═╝     ╚══════╝╚══════╝╚═╝╚═╝  ╚═══╝╚══════╝╚══════╝╚═╝     ╚═╝╚═╝  ╚═╝╚══════╝
 * ════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════
 *                                            - Pipeline [varied by SHADER/INPUTS/LAYOUT/&FixedFunctions] -
 * ════════════════════════════════════════════════════════════════════ HIGH LIGHTS ═══════════════════════════════════════════════════════════════════
 * \brief
 * Pipeline [varied by SHADER/INPUTS/LAYOUT or fixed-function stuffs like MSAA, Rasterization, ColorBlending, Viewport & Scissors
 *          INPUTS: VertexInputLayout \todo extended \page on these
 *
 * METAPHORE:
 * you have a GraphicePipe or ComputePipe.... which is like a store of pipes and then you build a PIPELINE using the pipes    
 * Only create 1 Object of these, and store those VARYING Options (like _vert, _frag) locally in your MESH-CLASS or whatever
 *    we did this, cz not everything needs to be saved in memory
 *
 *
 * \todo debug and see if amVK_GraphicsPipes ShaderOnly_pipeStore; declaration causes a call to constructor      cz we later do ShaderOnly_pipeStore = amVK_GraphicsPipes(renderPass, device); which means calling the construcotr   we dont wanna waste time
 * \todo see if ShaderOnly_pipeStore = amVK_GraphicsPipes(renderPass, device);   actually called the constructor & copy-constructor..... calling both would waste CPU time
 * TODO: Make a PARTED Docs on PIPELINE [ 😃 Use Good Fonts]
 * 
 * USE ONE OF THESE:
 * \see amVK_GraphicsPipes
 * \see amVK_ComputePipes  [WIP]
 */
class amVK_PipeStoreMK2 {
 public:
  amVK_DeviceMK2 *amVK_D;
  VkPipelineLayout shaderInputsLayout; /** MUST */
  VkPipelineCreateFlags flags;  /** :WIP: */

  amVK_PipeStoreMK2(amVK_DeviceMK2 *D = nullptr);
  ~amVK_PipeStoreMK2() {}
  inline void destroy() {}

  /** 
   *   █▀ █░█ ▄▀█ █▀▄ █▀▀ █▀█  █▀▄▀█ █▀█ █▀▄ █░█ █░░ █▀▀ 
   *   ▄█ █▀█ █▀█ █▄▀ ██▄ █▀▄  █░▀░█ █▄█ █▄▀ █▄█ █▄▄ ██▄ 
   * 
   * \param spvPath: You should compile any .vert or .frag and specify Full/Rel-path to EXE (.spv)
   */
  VkShaderModule load_ShaderModule(std::string &spvPath);
  /**
   * \todo add Multi-Shader support
   * \todo add support for automatic VulkanVersion choosing for GLSLANG
   * \todo fix/add preamble stuffs
   * 
   * \param glslPath: Full Path of glsl file
   * \param cacheSPVPath: if '\0', glslPath's folder & name is used.... with '.spv' prefix for the SPV output
   *                      otherwise should be a full/path/fileName.spv,      '.spv' not added implicitly then
   */
  VkShaderModule glslc_Shader(std::string &glslPath, amVK_ShaderStage stage = Shader_Unknown, std::string cacheSPVPath = "\0", bool cache = false);
  VkShaderModule glslc_Shader(const char  *glslCode, amVK_ShaderStage stage,                  std::string cacheSPVPath,        bool cache);

  static inline bool is_glslang_init = false; /** Godot says its safe to call it on Multiple threads, but glslang:InitializeProcess calls `ShInitialize()` function only, which GLSLang tells to only call per process, *not per thread */
                                              /** https://github.com/godotengine/godot/blob/8f6c16e4a459b54d6b37bc64e0bd21a361078a01/modules/glslang/register_types.cpp#L193-L195 */
  const std::string shader_preamble = "#define maybe_has_VK_KHR_multiview 1\n"; /** https://github.com/godotengine/godot/blob/master/modules/glslang/register_types.cpp#L120   [2021 NOV 14] */
  
  inline void destroy_ShaderModule(VkShaderModule xd) {vkDestroyShaderModule(amVK_D->D, xd, nullptr);}

  /** \see ShaderInputsMK2::create_pipelineLayout for layout */
};












/**
 *    █▀▀ █▀█ ▄▀█ █▀█ █░█ █ █▀▀ █▀  █▀█ █ █▀█ █▀▀ █░░ █ █▄░█ █▀▀ 
 *    █▄█ █▀▄ █▀█ █▀▀ █▀█ █ █▄▄ ▄█  █▀▀ █ █▀▀ ██▄ █▄▄ █ █░▀█ ██▄
 *
 * VARIABLES: You can modify thse before calling build_pipeline,    pipeline will use these like 'CONFIGURATIONS'
 *      MUST: vars that marked Must must be assigned a VALUE, DUH
 *     USAGE: Pipeline is Needed as per DIFFERENT _vert/_frag shader   or    as per VertexInputLayout (a.k.a Binding/Attribute Desc.)
 *            You can use the sample pipeline for Different MODELS if they share the Same Shaders
 * 
 *     OPT-I: Create a base amVK_GraphicsPipe.... have _vert,_frag (stuffs like that)   as members of your MESH,
 *            Think of this CLASS as a NAMESPACE with bunch of MODIFY-able GLOBAL OPTIONS....
 * METAPHORE: think of it in this way....   this is amVK_GraphicsPipe.... you take PIPE/s from here & do \fn build_Pipeline().... this is the STORE for all your PIPEs
 */
class amVK_GraphicsPipes : public amVK_PipeStoreMK2 {
 public:
  amVK_RenderPassMK2 *amVK_RP;       /** [IN, MUST] */
  amVK_GraphicsPipes(amVK_RenderPassMK2 *amVK_RP, amVK_DeviceMK2 *D) : amVK_RP(amVK_RP), amVK_PipeStoreMK2(D) {if (amVK_RP == nullptr) {amVK_LOG_EX("Param 'RP': nullptr ....  build_pipeline() will fail ");}}
  /** 
   * variables below should be set before calling this function. 
   * Only 'vert', 'frag' is exception, those are set to the_info in build_Pipeline 
   * if you wish to change A SINGLE or VERY FEW settings, manipulate OG.xxx explicitly
   */
  void konfigurieren(void);
  VkPipeline build_Pipeline(void);


  /** \todo Add support for Tesselation & Geometry which can be optional */
  VkShaderModule vert = nullptr;     /** [IN, MUST] */
  VkShaderModule frag = nullptr;     /** [IN, MUST] */


  amVK_Array<VkVertexInputBindingDescription>     VIBindings{};  /** VertexInput   'Binding' */
  amVK_Array<VkVertexInputAttributeDescription> VIAttributes{};  /** VertexInput 'Attribute'  */

  /** [OG.VertexInputAssembly] */
  VkPrimitiveTopology vertTopo = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

  /** [OG.Raster] */
  VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;         /** should be changed for rendering LINE or POINT */
  float lineWidth = 1.0f;                                   /** theres a .wideLines PhysicalDeviceFeature [https://stackoverflow.com/a/44575227] */

  bool rasterizerDiscardEnable = false;                     /** rasterizationStage ON/OFF */
  VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE;          /** [or] COUNTER_CLOCKWISE */
  VkCullModeFlagBits faceCulling = VK_CULL_MODE_NONE;       /** Disabled by default */
  /** \todo do smth about the DepthBias Options */

  /** [OG.DepthStencil] */
  bool depth = true;                                        /** \see Viewport.maxDepth & minDepth */
  VkCompareOp depthOp = VK_COMPARE_OP_LESS_OR_EQUAL;        /**  [yes, seems that, its in REVERSE, DRAW if z-index is less] 0.0f is on top, 1.0f is at bottom */
  bool stencil = false;                                     /** WIP \todo SOON */
  
  /** [OG.ColorBlend] + OG.ColorBlendStates */
  bool transparencyEnable = false;                          /** WIP \todo soon */
  bool blendEnable = false;                                 /** WIP \todo soon */
  VkColorComponentFlags    colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;



  /** [OG.DynamicViewportNScissor] use vkCmdSetViewport / Scissor later */
  /** [OG.DynamicStates] : VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR is default */
  uint8_t viewportCount = 1;
  uint8_t scissorCount = 0;

  /** [OG.MSAA] MultiSampling, \note renderpass also has to support it */
  uint8_t MSAAsamples = 1;  float minSampleShading = 1.0f;  
  /** \todo add .pSamleMask & .alphaToCoverageEnable support */




  /** 
   *   ╻ ╻   ╺┳╸╻ ╻┏━╸   ╻┏┓╻┏━╸┏━┓
   *   ╺╋╸    ┃ ┣━┫┣╸    ┃┃┗┫┣╸ ┃ ┃
   *   ╹ ╹    ╹ ╹ ╹┗━╸╺━╸╹╹ ╹╹  ┗━┛
   * [word of advice, dont look at the right side, it looks messy] SCI = StateCreateInfo 
   */
  #define SCI_VertexInput       VkPipelineVertexInputStateCreateInfo
  #define SCI_InputAssembly     VkPipelineInputAssemblyStateCreateInfo
  #define SCI_Tessellation      VkPipelineTessellationStateCreateInfo
  #define SCI_Viewport          VkPipelineViewportStateCreateInfo
  #define SCI_Rasterization     VkPipelineRasterizationStateCreateInfo
  #define SCI_Multisample       VkPipelineMultisampleStateCreateInfo
  #define SCI_DepthStencil      VkPipelineDepthStencilStateCreateInfo
  #define SCI_ColorBlend        VkPipelineColorBlendStateCreateInfo
  #define SCI_Dynamic           VkPipelineDynamicStateCreateInfo
  struct the_info_finals {
    SCI_VertexInput      VAO;
    SCI_InputAssembly    VertexInputAssembly;
    SCI_Tessellation     Tessellation;
    SCI_Viewport         DynamicViewportNScissor;
    SCI_Rasterization    Raster;
    SCI_Multisample      MSAA;
    SCI_DepthStencil     DepthStencil;
    SCI_ColorBlend       ColorBlend;
    SCI_Dynamic          DynamicWhat;
    amVK_Array<VkDynamicState> DynamicStates;     // 'new' alloc
    amVK_Array<VkPipelineColorBlendAttachmentState> ColorBlendStates; // 'new' alloc
  } OG;
  VkGraphicsPipelineCreateInfo the_info;
};

class amVK_ComputePipes : public amVK_PipeStoreMK2 {
 public:
  bool WIP = true;
};





/**
    https://open.spotify.com/playlist/142cbkQ47RALYjSZ1SDfkj?si=0a3c910e6a214e3c
    https://open.spotify.com/playlist/6OlaKLLkqZMbeiYVlnYS3O?si=c9d61255910b4723
    https://open.spotify.com/playlist/6yIn9i7Z8WutrZaCB7ixVw?si=e3a1c53adddc4eec
    https://open.spotify.com/user/31i7ysye54yvmkkteb757x3z6zo4?si=c538a3a94a084030
    https://open.spotify.com/playlist/4rmpEaJO1C4lODSDVTST24?si=bcdf324e080141bb
    https://open.spotify.com/playlist/7sva0cdxDoes7IULKHdQZK?si=859584c635c249fc
    https://open.spotify.com/playlist/5O6qx7wpZ8kcC2VuhziNSK?si=36819cd6102f4580
 */















/** 
 * Basically this is why, we came all this way.... from the Beginning, when we chose VULKAN....
 * 
 * Its NOT OK to create amVK_Pipeline as per OBJECT....
 * instead you should create Classes like 2D_Quad, have a single STATIC Pipeline for all the objects....
 * But pipelines vary by devices.... \see RTC Quad2D.hh and other files for solution for this
 */
class amVK_Pipeline {
 public:
  /** \fn Destroy() makes is nullptr */
  VkPipeline             P = nullptr;
  VkPipelineLayout  layout = nullptr;
  VkShaderModule      vert = nullptr;
  VkShaderModule      frag = nullptr;

  /**
   * PURE VIRTUAL FUNCTION
   * has to call \fn ShaderInputsMK2::set_Info()  \see 2D/Quad2D.hh
   *   called in \fn   amVK_Pipeline::Build()
   * [Every Pipeline needs Inputs.... this function sets Inputs info so the VkPipelineLayout can be created]
   */
  virtual inline void set_ShaderInputs(void) = 0;

  amVK_Pipeline() {}
  ~amVK_Pipeline() {}

  /** Destroyed ShaderModule & Pipeline.... [Remember to separately Destroy ShaderInputs a.k.a PipelineLayouts]*/
  bool destroy(VkDevice D) {
    // _PS->destroy_ShaderModule(vert);
    // _PS->destroy_ShaderModule(frag);
    vkDestroyShaderModule(D, vert, nullptr);
    vkDestroyShaderModule(D, frag, nullptr);
    
    //seems this below implicitly destroyes ShaderModules
    if      (this->P) {vkDestroyPipeline(D, this->P, nullptr);}
    else {amVK_LOG_EX("Seems the pipeline [amVK_Pipeline::_P] is already destroyed");}

    if (layout) {vkDestroyPipelineLayout(D, layout, nullptr);}
    else {amVK_LOG_EX("Seems the pipelien layout [layout]   is already destroyed");}

    this->P = nullptr;
    return true;
  }
  inline bool destroy(amVK_DeviceMK2 *amVK_D) {return destroy(amVK_D->D);}

  /** 
   * \todo update to support any kinda pipestore like compute, currently param PS is amVK_GraphicsPipes
   * 
   * \param spvFileName_Common: without the '.vert'/'.frag'  e.g. 'shaders/2D_Quad'
   * \param inputs: ShaderInputsMK2_Solo/ShaderInputsMK2_NotSolo
   * \param C_PipelineLayout: true default, calles inputs->create_PipelineLayout(PS->_amVK_D) if true
   * 
   * \param PS: PS is the Pipestore, where we create Pipelines from.... cz so many stuffs are repetitive in different pipelines, so we made a store
   *            \see amVK_PipeStoreMK2
   */
  void Build(std::string spvFileName_Common, amVK_GraphicsPipes *PS) {
    //vert = _PS->load_ShaderModule(spvFileName_Common + ".vert.spv");
    //frag = _PS->load_ShaderModule(spvFileName_Common + ".frag.spv");
    vert = PS->glslc_Shader(spvFileName_Common + ".vert", Shader_Vertex);
    frag = PS->glslc_Shader(spvFileName_Common + ".frag", Shader_Fragment);
    
    set_ShaderInputs();
    layout = ShaderInputsMK2::create_PipelineLayout(PS->amVK_D);

    PS->vert = vert;
    PS->frag = frag;
    PS->shaderInputsLayout = layout;

    this->P = PS->build_Pipeline();
  }

 protected:
  amVK_Pipeline(const amVK_Pipeline&) = delete;             //Brendan's Solution
  amVK_Pipeline& operator=(const amVK_Pipeline&) = delete;  //Brendan's Solution
};
