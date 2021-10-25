#ifndef amVK_DEVICE_H
#define amVK_DEVICE_H
#include "vulkan/vulkan.h"

/** We use VMA for now    [VMA_IMPLIMENTATION used in this Translation Unit] */
#include "vk_mem_alloc.h"   //includes vulkan/vulkan.h   [//Vulkan Includes "windows.h" on WIN     [? DEFINE  WIN32_LEAN_AND_MEAN ]]
#define amVK_DO_INIT_VMA true
#define amVK_DONT_INIT_VMA false

#ifndef amVK_CMDPOOL_H
  class amVK_CmdPool;   //Cyclic_Dep  [amVK_CmdPool.hh included in cpp]
#endif

class amVK_DeviceMods;  //Cyclic_Dep [amVK.hh,   so we include in amVK_Device.cpp]




/**
     █████╗ ███╗   ███╗██╗   ██╗██╗  ██╗        ██████╗ ███████╗██╗   ██╗██╗ ██████╗███████╗
    ██╔══██╗████╗ ████║██║   ██║██║ ██╔╝        ██╔══██╗██╔════╝██║   ██║██║██╔════╝██╔════╝
    ███████║██╔████╔██║██║   ██║█████╔╝         ██║  ██║█████╗  ██║   ██║██║██║     █████╗  
    ██╔══██║██║╚██╔╝██║╚██╗ ██╔╝██╔═██╗         ██║  ██║██╔══╝  ╚██╗ ██╔╝██║██║     ██╔══╝  
    ██║  ██║██║ ╚═╝ ██║ ╚████╔╝ ██║  ██╗███████╗██████╔╝███████╗ ╚████╔╝ ██║╚██████╗███████╗
    ╚═╝  ╚═╝╚═╝     ╚═╝  ╚═══╝  ╚═╝  ╚═╝╚══════╝╚═════╝ ╚══════╝  ╚═══╝  ╚═╝ ╚═════╝╚══════╝

 * \brief
 * Initiate CommandPool, [Create & Begin] CommandBuffer, RenderPass, Fence & Semaphore, Initiate VMA [VulkanMemoryAllocator by AMD]
 * BASE: All functions that require Device as Parameter should be Here
 * 
 * since we dont have a amVK_PhysicalDevice class, Physical Device related stuffa are mostly in amVK_CX, But you can find sm stuffs here too like the USED Internally section
 * \todo doc this in the MAIN DOC
 */
class amVK_Device {
 public:
  VkDevice _D;
  VkPhysicalDevice _PD;
  amVK_DeviceMods *_MODS; /** qFamily, Extensions, Features */
  VkCommandPool _CmdPool; /** TODO: But you could create Multiple CmdBufs on different Threads [NOTE: vkQueueSubmit is not thread-safe, only one thread can push the commands at a time.] */
  VmaAllocator _allocator;    /** as per VkDevice    [ \see init_VMA()]     TODO: Make this option & init_VMA optional or at least a way for other VMA alt APIs*/
    
  /** \param MODS: can be nullptr, */
  amVK_Device(VkDevice D, VkPhysicalDevice PD, amVK_DeviceMods *MODS) : _D(D), _PD(PD), _MODS(MODS) {this->init_VMA();}
  ~amVK_Device() {if (_MODS != nullptr) delete _MODS; /** \see amVK_CX::CreateDevice() */}

  /**
    ╻ ╻   ╻ ╻┏━┓┏━╸╺┳┓   ╻┏┓╻╺┳╸┏━╸┏━┓┏┓╻┏━┓╻  ╻  ╻ ╻   
     ╋╸   ┃ ┃┗━┓┣╸  ┃┃   ┃┃┗┫ ┃ ┣╸ ┣┳┛┃┗┫┣━┫┃  ┃  ┗┳┛   
    ╹ ╹   ┗━┛┗━┛┗━╸╺┻┛   ╹╹ ╹ ╹ ┗━╸╹┗╸╹ ╹╹ ╹┗━╸┗━╸ ╹ 
   */
    uint32_t present_qFamily = 0xFFFFFFFF;         //See present_qFam
    bool found_present_qFamily = false;
    uint32_t _present_qFam(VkSurfaceKHR S);       /** \see amVK_WI::is_present_sup() & amVK_WI::amVK_WI() [Constructor] */

  /** only \if you used  amVK_DevicePreset_Flags */
    uint32_t qIndex_MK2 = 0;  //in amVK_DeviceMods.... we only asked for 1 queue
    uint32_t get_graphics_qFamily(void);          //_MODS->_graphics_qFAM;
    VkQueue  get_graphics_queue(void);            //vkGetDeviceQueue(_D, _MODS->_graphics_qFAM, 0, &Q);
  
  /**
   * TODO: Add support for Multi-Threaded CommandPool    [Start Here: https://stackoverflow.com/questions/53438692/creating-multiple-command-pools-per-thread-in-vulkan]
   * \param qFamily: you can Create CommandPool for Computer/Graphics/Transfer anything really....
   * \return The commandPool that was Created
   * \see CommandBuffer related stuffs in amVK_CmdPool.hh
   */
  VkCommandPool init_commandpool(uint32_t qFamily);
  /**
   *   ╻ ╻   ┏━╸┏━┓┏┳┓┏┳┓┏━┓┏┓╻╺┳┓   ┏┓ ╻ ╻┏━╸┏━╸┏━╸┏━┓
   *   ╺╋╸   ┃  ┃ ┃┃┃┃┃┃┃┣━┫┃┗┫ ┃┃   ┣┻┓┃ ┃┣╸ ┣╸ ┣╸ ┣┳┛
   *   ╹ ╹   ┗━╸┗━┛╹ ╹╹ ╹╹ ╹╹ ╹╺┻┛   ┗━┛┗━┛╹  ╹  ┗━╸╹┗╸
   * 
   * a Good point to start: https://vkguide.dev/docs/chapter-1/vulkan_command_flow/
   * SHORT: Unlike OpenGL or DirectX pre-11, in Vulkan, all GPU commands have to go through a command buffer. 
   *            Command buffers are allocated from a Command Pool, and executed on Queues.
   *            The general flow to execute commands is:
   *              - You allocate a VkCommandBuffer from a VkCommandPool
   *              - You record commands into the command buffer, using VkCmdXXXXX functions.
   *              - You submit the command buffer into a VkQueue, which starts executing the commands.
   *       [- vblanco20-1]
  */
  /**
   * TODO: You can Allocate multiple cmdBuffer at once
   * \param n: Number of CommandBuffers to alloc
   * \return pointer to the Array of VkCommandBuffer
   */
  VkCommandBuffer *alloc_cmdBuf(uint32_t n);

  /**
   * Start Recording into a Particular COMMAND-BUFFER [For 1 Time usage or Multiple-Time usage]
   */
  void begin_cmdBuf(VkCommandBuffer cmdBuf, bool oneTime = true);

    
  /** Every fench only waits for SINGLE vkCmd** command [Reset the fench before using again ref: https://vkguide.dev/docs/chapter-1/vulkan_mainloop_code/] */
  VkFence create_fence(void);
  VkSemaphore create_semaphore(void);


 private:
  /**
   *   ╻ ╻   ┏━┓┏┳┓╺┳┓   ╻ ╻┏┳┓┏━┓
   *   ╺╋╸   ┣━┫┃┃┃ ┃┃   ┃┏┛┃┃┃┣━┫
   *   ╹ ╹   ╹ ╹╹ ╹╺┻┛   ┗┛ ╹ ╹╹ ╹
   *  needs INSTANCE, DEVICE & PD    [Instance is the deafault GLOBAL amVK_CX::instance]
   * \return VmaAllocator: _allocator member variable
   * CALLED-IN: CONSTRUCTOR
   * You shouldn't care about init_VMA() if you haven't reached BUFFER related stuffs yet 
   */
  VmaAllocator init_VMA(void);
};
#endif //#ifndef amVK_DEVICE_H