#ifndef amVK_PIPELINE_H
#define amVK_PIPELINE_H

/** Think of this CLASS/es as    a NAMESPACE with bunch of MODIFY-able GLOBAL OPTIONS....      SO Only create 1 Object of these, and store those VARYING Options (like _vert, _frag) locally in your MESH-CLASS */

#include <string>
#include "vulkan/vulkan.h"
#include "amVK_Types.hh"    //amVK_Array

class amVK_Device; // Cyclic Dependency, [Header included in] 😃

/**
 * TODO: Make a PARTED Docs on PIPELINE [ 😃 Use Good Fonts]
 * USE ONE OF THESE:  FUCK
 * \see amVK_GraphicsPipe
 * \see amVK_ComputePipe  [WIP]
 * 
 * if while if for
 * 
 * https://vkguide.dev/docs/chapter-2/vulkan_render_pipeline/
 * https://vkguide.dev/docs/chapter-2/triangle_walkthrough/
 * This one and Brendan Galea GraphicsPipeLine video is the BEst, Fuck Vulkan-Tutorial Confusing since the beginning of time 
 * https://vkguide.dev/docs/chapter-2/pipeline_walkthrough/ [Best Info so far]
 */
class amVK_Pipeline {
 public:
  amVK_Device *_amVK_D;
  //VkPipeline _P;      Read the First Comment line 4
  VkPipelineLayout _layout;   //Common between any kind (Graphics/Compute/RT)
  //VkPipelineCreateFlags _flags;  WIP

  amVK_Pipeline(amVK_Device *D) : _amVK_D(D) {}
  ~amVK_Pipeline() {}

  /** 
   *   █▀ █░█ ▄▀█ █▀▄ █▀▀ █▀█  █▀▄▀█ █▀█ █▀▄ █░█ █░░ █▀▀ 
   *   ▄█ █▀█ █▀█ █▄▀ ██▄ █▀▄  █░▀░█ █▄█ █▄▀ █▄█ █▄▄ ██▄ 
   * 
   * You should compile any .vert or .frag and specify Full/Rel-path to EXE 
   * COULD HAVE BEEN STATIC, BUT i dont actually need it now to be that
   */
  VkShaderModule load_ShaderModule(std::string &spvPath);

  /** 
   *   █▀█ █ █▀█ █▀▀ █░░ █ █▄░█ █▀▀  █░░ ▄▀█ █▄█ █▀█ █░█ ▀█▀
   *   █▀▀ █ █▀▀ ██▄ █▄▄ █ █░▀█ ██▄  █▄▄ █▀█ ░█░ █▄█ █▄█ ░█░
   * 
   * It’s here where you would configure your 
   *      push-constants &
   *      descriptor sets
   */
  VkPipelineLayout new_PipelineLayout(uint32_t pushConstant_n, VkPushConstantRange *pushConstant_ranges, uint32_t descriptorSet_n, VkDescriptorSetLayout * descriptorSet_layouts);
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
class amVK_GraphicsPipe : public amVK_Pipeline {
 public:
  VkRenderPass _renderPass = nullptr; /** [MUST] */

  VkShaderModule _vert = nullptr;     /** [MUST] */
  VkShaderModule _frag = nullptr;     /** [MUST] */

  VkExtent2D _viewportExtent{};       /** [MUST] */

  amVK_Array<VkVertexInputBindingDescription>     _VIBindings{};  /** VertexInput   'Binding' Description [Array] */
  amVK_Array<VkVertexInputAttributeDescription> _VIAttributes{};  /** VertexInput 'Attribute' Description [Array] */

  VkPrimitiveTopology _vertTopo = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;



  /** 
   * ALL YOU WILL PROBABLY NEED TO CARE ABOUT
   * USE: amVK_Pipeline::load_ShaderModule()     for _vert & _frag
   * TODO: SubPass Support
   */
  VkPipeline build_Pipeline(void);

  /** CONSTRUCTOR */
  amVK_GraphicsPipe(amVK_Device *D, VkRenderPass RP = nullptr) : amVK_Pipeline(D), _renderPass(RP) {}
  ~amVK_GraphicsPipe() {}
};

#endif //#ifndef amVK_PIPELINE_H