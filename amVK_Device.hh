#ifndef amVK_DEVICE_H
#define amVK_DEVICE_H
#include "vulkan/vulkan.h"
#include <vector>
#include <fstream> //CreateShaderModule() function
#include "amVK_Logger.hh"

/** We use VMA for now    [VMA_IMPLIMENTATION used in this Translation Unit] */
#include "vk_mem_alloc.h"   //includes vulkan/vulkan.h   [//Vulkan Includes "windows.h" on WIN     [? DEFINE  WIN32_LEAN_AND_MEAN ]]
#define amVK_DO_INIT_VMA true
#define amVK_DONT_INIT_VMA false

#ifndef amVK_CMDPOOL_H
  class amVK_CmdPool; //Cyclic_Dep  [amVK_CmdPool.hh included in cpp]
#endif

/**
 * LOGICAL DEVICE Implimentation, [That one thing that you get calling vkCreateDevice....]
 * All functions that require Device as Parameter should be Here
 * You can also Find all Logical Device related Info here, Query for them or Purge them from Memory [happy Empire-Giri on your GPU]
 * 
 * SHORT: has the VkDevice as a Member + (All)Most of Device related Functions [Only exceptions are like amVK_WSI.hh and such]
 * TODO: Queues & qFamily Support?
 */
class amVK_Device {
 public:
  VkDevice _D;
  VkPhysicalDevice _PD;
  VkCommandPool _CmdPool; /** TODO: But you could create Multiple CmdBufs on different Threads [NOTE: vkQueueSubmit is not thread-safe, only one thread can push the commands at a time.] */
  VmaAllocator _allocator;    /** as per VkDevice    [ \see init_VMA()]     TODO: Make this option & init_VMA optional or at least a way for other VMA alt APIs*/
    
  /** You shouldn't care about init_VMA() if you haven't reached BUFFER related stuffs yet */
  amVK_Device(VkDevice D, VkPhysicalDevice PD) : _D(D), _PD(PD) {this->init_VMA();}
  ~amVK_Device() {}

  
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


  /**
   *   ╻ ╻   ┏━┓┏━╸┏┓╻╺┳┓┏━╸┏━┓┏━┓┏━┓┏━┓┏━┓
   *   ╺╋╸   ┣┳┛┣╸ ┃┗┫ ┃┃┣╸ ┣┳┛┣━┛┣━┫┗━┓┗━┓
   *   ╹ ╹   ╹┗╸┗━╸╹ ╹╺┻┛┗━╸╹┗╸╹  ╹ ╹┗━┛┗━┛
   */
  VkRenderPass  init_renderPass(void);

    
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
   */
  VmaAllocator init_VMA(void);
};










/**
 *  ╻ ╻   ╻ ╻╺┳╸╻╻  
 *  ╺╋╸   ┃ ┃ ┃ ┃┃  
 *  ╹ ╹   ┗━┛ ╹ ╹┗━╸
 * I just wanted to access amVK_CX::_device_list with _device_list[(VkDevice)D]
 * NOTE: if it was std::vector<amVK_Device> then if sm1 changes a amVK_Device, that won't be change in the VECTOR ONE
 *       So we gotta store only the REference or ratherMore pointers
 */
class vec_amVK_device : public std::vector<amVK_Device *> {
 public:
  vec_amVK_device(uint32_t n) : std::vector<amVK_Device *> (n) {}
  vec_amVK_device(void) : std::vector<amVK_Device *>() {}
  ~vec_amVK_device() {}

  //amVK Stores a lot of different kinda data like this. Maybe enable a option to store these in HDD as cache
  amVK_Device *operator[](VkDevice D) {
    amVK_Device **data = this->data();
    for (int i = 0, lim = this->size(); i < lim; i++) {
      if (data[i]->_D == D) {
        return data[i];
      }
    }
    LOG_EX("LogicalDevice doesn't Exist"); //TODO: Better error Handle
    return nullptr;
  }
};
#endif //#ifndef amVK_DEVICE_H