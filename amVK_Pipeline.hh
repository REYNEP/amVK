#ifndef amVK_PIPELINE_H
#define amVK_PIPELINE_H

#include <string>
#include "amVK_Common.hh"
#include "amVK_RenderPass.hh"
class amVK_Device; // Cyclic Dependency, [Header included in] 😃

class ShaderInputsMK2 {
 public:
  VkPipelineLayout     layout;
  bool       usingSolo = true;  /** SOLO is the deafult */
  bool       didMalloc = false; /** if you call NotSolo::alloc(); free(); called in Destructor*/

  virtual amVK_Array<VkPushConstantRange> ref_PushConsts(void) = 0;
  virtual amVK_Array<VkDescriptorSetLayout> ref_DescSets(void) = 0;

  /** //GIT_DIFF_FIX
   *   █▀█ █ █▀█ █▀▀ █░░ █ █▄░█ █▀▀  █░░ ▄▀█ █▄█ █▀█ █░█ ▀█▀
   *   █▀▀ █ █▀▀ ██▄ █▄▄ █ █░▀█ ██▄  █▄▄ █▀█ ░█░ █▄█ █▄█ ░█░
   * //GIT_DIFF_FIX
   * Uses   \fn ref_pushConsts()   \fn ref_descSets()    and [out]puts into layout */
  void create_PipelineLayout(amVK_Device *amVK_D);
};


/** //GIT_DIFF_FIX
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
 * //GIT_DIFF_FIX
 * METAPHORE:
 * you have a GraphicePipe or ComputePipe.... which is like a store of pipes and then you build a PIPELINE using the pipes    
 * Only create 1 Object of these, and store those VARYING Options (like _vert, _frag) locally in your MESH-CLASS or whatever
 *    we did this, cz not everything needs to be saved in memory
 * //GIT_DIFF_FIX
 * //GIT_DIFF_FIX
 * \todo change LOG_EX to BackTrace
 * \todo fix amVK_Array logics, currently ppl can like set .data to malloced space,   or initialize with a list; which causes 'new' alloc
 * \todo debug and see if amVK_GraphicsPipes ShaderOnly_pipeStore; declaration causes a call to constructor      cz we later do ShaderOnly_pipeStore = amVK_GraphicsPipes(renderPass, device); which means calling the construcotr   we dont wanna waste time
 * \todo see if ShaderOnly_pipeStore = amVK_GraphicsPipes(renderPass, device);   actually called the constructor & copy-constructor..... calling both would waste CPU time
 * TODO: Make a PARTED Docs on PIPELINE [ 😃 Use Good Fonts] //GIT_DIFF_FIX
 * USE ONE OF THESE:
 * \see amVK_GraphicsPipes
 * \see amVK_ComputePipes  [WIP]
 */ //GIT_DIFF_FIX
class amVK_PipeStoreMK2 {
 public:
  amVK_Device *_amVK_D;
  ShaderInputsMK2 *shaderInputs;/** MUST, you do it EXPLICIT */
  VkPipelineCreateFlags flags;  /** :WIP: */

  amVK_PipeStoreMK2(amVK_Device *D = nullptr) : _amVK_D(D) { 
    if (D == nullptr) {amVK_SET_activeD(_amVK_D);}
    else {amVK_CHECK_DEVICE(D, _amVK_D);}
  }
  ~amVK_PipeStoreMK2() {}

  /** 
   *   █▀ █░█ ▄▀█ █▀▄ █▀▀ █▀█  █▀▄▀█ █▀█ █▀▄ █░█ █░░ █▀▀ 
   *   ▄█ █▀█ █▀█ █▄▀ ██▄ █▀▄  █░▀░█ █▄█ █▄▀ █▄█ █▄▄ ██▄ 
   * 
   * \param spvPath: You should compile any .vert or .frag and specify Full/Rel-path to EXE (.spv)
   */
  VkShaderModule load_ShaderModule(std::string &spvPath);

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
 * METAPHORE: think of it in this way....   this is amVK_GraphicsPipe.... you take PIPE/s from here & build_Pipeline.... this is the STORE for all your PIPEs
 */
class amVK_GraphicsPipes : public amVK_PipeStoreMK2 {
 public:
  amVK_RenderPassMK2 *_amVK_RP;       /** [IN, MUST] */
  amVK_GraphicsPipes(amVK_RenderPassMK2 *RP, amVK_Device *D) : _amVK_RP(RP), amVK_PipeStoreMK2(D) {if (RP == nullptr) {LOG_EX("Param 'RP': nullptr ....  build_pipeline() will fail ");}}
  amVK_GraphicsPipes(void) {}

  /** \todo Add support for Tesselation & Geometry which can be optional */
  VkShaderModule vert = nullptr;     /** [IN, MUST] */
  VkShaderModule frag = nullptr;     /** [IN, MUST] */

  /** VAO in OpenGL,  [alias-names: VertexInputBuffer, vertex buffers, VertexBuffer, vertex formats, VERTEX INPUT LAYOUT, VertexInputLayout, VertexInputState] */
  amVK_Array<VkVertexInputBindingDescription>     VIBindings{};  /** VertexInput   'Binding' */
  amVK_Array<VkVertexInputAttributeDescription> VIAttributes{};  /** VertexInput 'Attribute'  */

  /** VertexInputAssembly */
  VkPrimitiveTopology vertTopo = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

  /** Dynamic Viewport & Scissors.... you can't change it,  trust a random guy on the internet: [https://www.reddit.com/r/vulkan/comments/g6c4iz/comment/foshogz] 
   * so, whats different?   viewportCount = 1, pViewports = nullptr  [same for scissors]      - linked with the_info.pViewportState
   *  later in CommandBuffer you need to call vkCmdSetViewport/Scissor(); */
  uint8_t viewportCount = 1;  // also ScissorCount      [increase for multiViewport]

  /** [MSAA], \note renderpass also has to support it */
  uint8_t samples = 1;  float minSampleShading = 1.0f; /** SampleRateShading: https://docs.gl/gl4/glMinSampleShading  [got this website form TheCherno] */

  /** [Rasterization] : In here is where we do backface culling, set line width [a.k.a wireframe drawing], or DepthBias   \see amVK.md for now \todo DOCS */ 
  bool rasterizerDiscardEnable = false;   // [Interesting feature 🤔  \see other options details in amVK.md now ]

  /** [DepthStencil] :  VK_COMPARE_OP_LESS_OR_EQUAL used  [yes, seems that, its in REVERSE, DRAW if z-index is less] 0.0f is on top, 1.0f is at bottom */
  bool depth = true;  //by default do Depth Test   \also \see Viewport.maxDepth & minDepth   + \see DepthStencil.depthBias
  bool stencil = false; //WIP \todo SOON
  
  /** [ColorBlend] : You can like make Transparent Stuffs here  [like a GlassWindow. Like the Vibrancy extension for  VSCODE 😉] */
  VkColorComponentFlags    colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  bool blendEnable = false; //WIP \todo SOON
  // OG.ColorBlendStates

  /** [Dynamic] :  VK_DYNAMIC_STATE_VIEWPORT , VK_DYNAMIC_STATE_SCISSOR is default */
  // OG.DynamicStates


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

  typedef void (*konfigurieren_callback)(the_info_finals *);

  /**
   *   \│/  ╔╗ ┬ ┬┬┬  ┌┬┐    ╔═╗┬┌─┐┌─┐┬  ┬┌┐┌┌─┐  
   *   ─ ─  ╠╩╗│ │││   ││    ╠═╝│├─┘├┤ │  ││││├┤   
   *   /│\  ╚═╝└─┘┴┴─┘─┴┘─── ╩  ┴┴  └─┘┴─┘┴┘└┘└─┘  
   */
  void konfigurieren(void);   //[Re]Write OG
  VkPipeline build_Pipeline(void);
};

class amVK_ComputePipes : public amVK_PipeStoreMK2 {
 public:
  bool WIP = true;
};






/** 
   ╻ ╻   ┏━┓╻ ╻┏━┓╺┳┓┏━╸┏━┓   ╻┏┓╻┏━┓╻ ╻╺┳╸┏━┓   ┏┳┓╻┏ ┏━┓
   ╺╋╸   ┗━┓┣━┫┣━┫ ┃┃┣╸ ┣┳┛   ┃┃┗┫┣━┛┃ ┃ ┃ ┗━┓   ┃┃┃┣┻┓┏━┛
   ╹ ╹   ┗━┛╹ ╹╹ ╹╺┻┛┗━╸╹┗╸   ╹╹ ╹╹  ┗━┛ ╹ ┗━┛   ╹ ╹╹ ╹┗━╸
 */
class ShaderInputsMK2_Solo : public ShaderInputsMK2 {
 public:
  /** Not checked if set or not.... */
  amVK_Array<VkPushConstantRange> ref_PushConsts(void) {uint32_t x = 1; if (_pushConst.size == 0) x=0; return amVK_Array<VkPushConstantRange>(&_pushConst, x);}
  amVK_Array<VkDescriptorSetLayout> ref_DescSets(void) {uint32_t x = 1; if  (_descSet == nullptr) x=0; return amVK_Array<VkDescriptorSetLayout>(&_descSet, x);}
  ShaderInputsMK2_Solo(void) {usingSolo = true;}

 private:
  VkPushConstantRange    _pushConst = {};
  VkDescriptorSetLayout    _descSet = nullptr;
};

class ShaderInputsMK2_NotSolo : public ShaderInputsMK2 {
 public:
  /** Not checked if set or not.... */
  amVK_Array<VkPushConstantRange> ref_PushConsts(void) {return _pushConsts;}
  amVK_Array<VkDescriptorSetLayout> ref_DescSets(void) {return _descSets;}
  
 private:
  amVK_Array<VkPushConstantRange> _pushConsts;
  amVK_Array<VkDescriptorSetLayout> _descSets;

  ShaderInputsMK2_NotSolo(void) {usingSolo = false;}
  void alloc(uint32_t pushConst_n, uint32_t descSet_n) {
    void *xd = malloc((pushConst_n * sizeof(VkPushConstantRange))
                     + descSet_n * sizeof(VkDescriptorSetLayout));
    _pushConsts.data = (VkPushConstantRange *) xd;
    _descSets.data = (VkDescriptorSetLayout *) _pushConsts.data + pushConst_n;
    _pushConsts.n = pushConst_n;
    _descSets.n = descSet_n;
    ShaderInputsMK2::didMalloc = true;
  }
  ~ShaderInputsMK2_NotSolo() {if (didMalloc) {free(_pushConsts.data); didMalloc = false;}}
};

#endif  //amVK_PIPELINE_H