#ifdef amVK_AMD_VMA_SUP
    #define VMA_IMPLEMENTATION
#endif
#include "amVK_Device.hh"
#include "amVK_Types.hh"
#include "amVK_Logger.hh"

/**
 * |--------------------------------------------|
 * - SHADER_LOADING & PIPELINE in amVK_Pipeline -
 * |--------------------------------------------|
*/

/**
 * |-----------------------------------------|
 *             - RENDER PASSES -
 * |-----------------------------------------|
 * For Now we only impl. 1 Subpass as Desktop GPUs
 * 
 * [MOTTO: its more about the SubPasses and not about'RenderPass' hype that you get from hearing the name]
 * 
 * [vblanco20-1] [from: https://vkguide.dev/docs/chapter-1/vulkan_renderpass/]
 *     In Vulkan, all of the rendering happens inside a VkRenderPass. 
 *     It is not possible to do rendering commands outside of a renderpass, 
 *     but it is possible to do   Compute commands without them.
 * 
 * MUST-READ: https://gpuopen.com/learn/vulkan-renderpasses/      [They explain whats actually goin on, Explains SubPasses]
 * 
 *     The renderpass is a concept that only exists in Vulkan. 
 *     It’s there because it allows the driver to know more about the state of the images you render.  
 *     [HOW? Bcz of SubPasses, (thats only if... you do use them)]
 * 
 * \see create_framebuffers in amVK_WI
*/
VkRenderPass amVK_Device::init_renderPass(void) {
    /** the renderpass will use this color attachment.   [Actually not the Renderpass exactly in a sense, rather the ONE and ONLY Subpass we gonna Create] **/
	VkAttachmentDescription color_attachment = {};
    /** the attachment will have the format needed by the swapchain */
        color_attachment.format             = amVK_SWAPCHAIN_IMAGE_FORMAT;
    /** 1 sample, we won't be doing MSAA */
        color_attachment.samples            = VK_SAMPLE_COUNT_1_BIT;
    /** we Clear when this attachment is loaded */
        color_attachment.loadOp             = VK_ATTACHMENT_LOAD_OP_CLEAR;
    /** we keep the attachment stored when the renderpass ends */
        color_attachment.storeOp            = VK_ATTACHMENT_STORE_OP_STORE;
    /** we don't care about stencil */
        color_attachment.stencilLoadOp      = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp     = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    /** we don't know or care about the starting layout of the attachment */
        color_attachment.initialLayout      = VK_IMAGE_LAYOUT_UNDEFINED;
    /** after the renderpass ends, the image has to be on a layout ready for display */
        color_attachment.finalLayout        = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref = {};
	/** attachment number will index into the pAttachments array in the parent renderpass itself */
	    color_attachment_ref.attachment = 0;
	    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        /** 
         * Seems like you could not use any other format.... cz 
         * 1. swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT   that matches with the Layout above
         * 2. VUID-VkAttachmentReference-layout-00857
         * 3. Other than that if you keep searching for where to use & NOT-TO the leftover layouts, You'll find it can't be used here
         * 4. VkImageLayout docs says VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL should be used for images created with   [point 1]
         */




    //Chapter 4 - Depth Attachment
    VkAttachmentDescription depth_attachment = {};
        // Depth attachment
        depth_attachment.flags = 0;
        depth_attachment.format = VK_FORMAT_D32_SFLOAT;
        depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_attachment_ref = {};
        depth_attachment_ref.attachment = 1;
        depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;





    /**
     * Now that our main image target is defined, we need to add a subpass that will render into it.
	 * we are going to create 1 subpass, which is the minimum you can do
     */
	VkSubpassDescription subpass = {};
    /** Only Ray Tracing and Compute Bind points is Left */
	    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	    subpass.colorAttachmentCount = 1;
	    subpass.pColorAttachments = &color_attachment_ref;
        //depth attachment
	    subpass.pDepthStencilAttachment = &depth_attachment_ref;

    /**
     * The image life will go something like this:
     *    UNDEFINED -> RenderPass Begins -> Subpass 0 begins (Transition to Attachment Optimal) 
     * -> Subpass 0 renders -> Subpass 0 ends -> Renderpass Ends (Transitions to Present Source)
    */

    /**
     * NOTE NOTE NOTE: IMPORTANT:
     * The Vulkan driver will perform the layout transitions for us when using the renderpass. 
     * If we weren’t using a renderpass (drawing from compute shaders) we would need to do the same transitions explicitly.
    */

    VkAttachmentDescription attachments[2] = { color_attachment,depth_attachment };
    /** Finally Create the RenderPass - TIME TRAVEL ! */
    VkRenderPassCreateInfo render_pass_info = {};
	    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

	/** connect the color attachment to the info */
	    render_pass_info.attachmentCount = 2;
	    render_pass_info.pAttachments = attachments;
	/** connect the subpass to the info */
	    render_pass_info.subpassCount = 1;
	    render_pass_info.pSubpasses = &subpass;

    VkRenderPass renderPass;
    VkResult res = vkCreateRenderPass(_D, &render_pass_info, nullptr, &renderPass);

    VK_CHECK(res, nullptr);
    return renderPass;
}




















/**
 * |-----------------------------------------|
 *            - COMMAND BUFFER -
 * |-----------------------------------------|
 * a Good point to start: https://vkguide.dev/docs/chapter-1/vulkan_command_flow/
 * SHORT: Unlike OpenGL or DirectX pre-11, in Vulkan, all GPU commands have to go through a command buffer. 
 *            Command buffers are allocated from a Command Pool, and executed on Queues.
 *            The general flow to execute commands is:
 *              - You allocate a VkCommandBuffer from a VkCommandPool
 *              - You record commands into the command buffer, using VkCmdXXXXX functions.
 *              - You submit the command buffer into a VkQueue, which starts executing the commands.
 *       [- vblanco20-1]
*/

VkCommandPool amVK_Device::init_commandpool(uint32_t qFamily) {
    VkCommandPoolCreateInfo cmdPool_info = {};
        cmdPool_info.sType =  VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmdPool_info.pNext = nullptr;
	    cmdPool_info.queueFamilyIndex = qFamily;
	/** we also want the pool to allow for resetting of individual command buffers 
     *  Lets us use   vkResetCommandBuffer() later On     [ VUID-vkResetCommandBuffer-commandBuffer-00046 ]
     *  */
	    cmdPool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkResult res = vkCreateCommandPool(_D, &cmdPool_info, nullptr, &_CmdPool);
    VK_CHECK(res, nullptr);
    return _CmdPool;
}

/**
 * TODO: You can Allocate multiple cmdBuffer at once
 * \param n: Number of CommandBuffers to alloc
 * \return pointer to the Array of VkCommandBuffer
 */
VkCommandBuffer *amVK_Device::alloc_cmdBuf(uint32_t n) {
    VkCommandBufferAllocateInfo cmdAllocInfo = {};
        cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdAllocInfo.pNext = nullptr;

        /** commands will be made from our _commandPool */
        cmdAllocInfo.commandPool = _CmdPool;
        /** we will allocate 1 command buffer */
        cmdAllocInfo.commandBufferCount = n;
        /** command level is Primary [We not gonna use SECONDARY, which are used in Extreme MultiThreading Optimized scenarios  \see VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT] */
        cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    VkCommandBuffer *cmdBuffer = static_cast<VkCommandBuffer *> (calloc(n, sizeof(VkCommandBuffer)));
    VkResult res = vkAllocateCommandBuffers(_D, &cmdAllocInfo, cmdBuffer);
    VK_CHECK(res, nullptr);
    return cmdBuffer;
}

/**
 * Start Recording into a Particular COMMAND-BUFFER [For 1 Time usage or Multiple-Time usage]
 */
void amVK_Device::begin_cmdBuf(VkCommandBuffer cmdBuf, bool oneTime) {
    /** begin the command buffer recording. */
    VkCommandBufferBeginInfo cmdBeginInfo = {};
        cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmdBeginInfo.pNext = nullptr;
    /** Inheritance info on a command buffer is used for secondary command buffers */
        cmdBeginInfo.pInheritanceInfo = nullptr;
    /** 
     * As we are going to be recording the command buffer every frame, 
     * it’s best if Vulkan knows that this command will only execute once, as it can allow for great optimization by the driver.
     */
    if (oneTime) {
        cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    } else {
        cmdBeginInfo.flags = 0;     //Otherwise, no Need for Optimization ig....
    }

    VkResult res = vkBeginCommandBuffer(cmdBuf, &cmdBeginInfo);
    VK_CHECK(res, );
}

















/**
 * |-----------------------------------------|
 *            - SYNCRONIZATION -
 * |-----------------------------------------|
*/
VkFence amVK_Device::create_fence(void) {
	VkFenceCreateInfo fenceCreateInfo = {};
	    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	    fenceCreateInfo.pNext = nullptr;

	/** so we can wait on it before using it on a GPU command (for the first frame) */
	    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;


    VkFence fence;
	VkResult res = vkCreateFence(_D, &fenceCreateInfo, nullptr, &fence);
    VK_CHECK(res, nullptr);
    return fence;
}

VkSemaphore amVK_Device::create_semaphore(void) {
    /** for the semaphores we don't need any flags */
	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	    semaphoreCreateInfo.pNext = nullptr;
	    semaphoreCreateInfo.flags = 0;

    VkSemaphore sema;
	VkResult res = vkCreateSemaphore(_D, &semaphoreCreateInfo, nullptr, &sema);
    VK_CHECK(res, nullptr);
    return sema;
}








/**
 * |-----------------------------------------|
 *    - AMD VMA (Vulkan Memory Allocator) -
 * |-----------------------------------------|
*/
#include "amVK.hh"

VmaAllocator amVK_Device::init_VMA(void) {
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = _PD;
    allocatorInfo.device = _D;
    allocatorInfo.instance = amVK_CX::instance;
    vmaCreateAllocator(&allocatorInfo, &_allocator);

    return _allocator;
}