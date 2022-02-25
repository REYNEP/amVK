#include "amVK_IN.hh"

/**
   ╻ ╻   ┏━┓┏┓ ┏━┓╻ ╻╺┳╸   ╺┳┓┏━╸┏━┓┏━╸┏━┓╻┏━┓╺┳╸┏━┓┏━┓┏━┓
   ╺╋╸   ┣━┫┣┻┓┃ ┃┃ ┃ ┃     ┃┃┣╸ ┗━┓┃  ┣┳┛┃┣━┛ ┃ ┃ ┃┣┳┛┗━┓
   ╹ ╹   ╹ ╹┗━┛┗━┛┗━┛ ╹    ╺┻┛┗━╸┗━┛┗━╸╹┗╸╹╹   ╹ ┗━┛╹┗╸┗━┛
 ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Simply a Pipeline or moresoever a 'Shader' pipeline can have Inputs, they come in Two forms PushConstants & DescriptorSets
 *    Now PushConstants is a Pretty Cool and Easy Idea... just see [2D_Quad.hh CONSTRUCTOR]
 *    But Descriptor Sets can get pretty confusing pretty fast....
 *  
 * The easy way out, that I got introduced to, at first was.... [whats inside CONSTRUCTOR -> whats inside Texture() -> vkCmdBindDescriptorSets() before you gonna Draw]
 *    The first station is:- 'You gotta Allocate SETs, as many as you want.... and DescriptorPool is how you help the GPU manage all those SETs'
 *    its kinda like this
 *      - DescriptorPool -> can have multiple DescriptorSets -> Can have multiple Descriptors
 *      -     now these 'Descriptors' [not SETs] has to be Grouped into BINDINGs....
 *      -
 *      - VkDescriptorPollSize is where you say how many 'Descriptors' [again (not the SETs)] you will have
 *      -     you also gotta specify 'maxSets' [DescriptorSets, (not the one above)] when you create Pool
 *      - now,
 *      - 'vkAllocateDescriptorSets' can allocate multiple Sets, but it also wants to know about how many 'Descriptors' and what Kind each 'SET' will have
 *      -     thats what the 'DescriptorSetLAYOUT' is.... 
 *      -     so you create a LAYOUT for each set, then give the LAYOUTs to the SetAllocateInfo
 *      -     now you need to give bunch of 'Bindings' to the LAYOUT, & Bindings are just groups of 'Descriptors' 
 *      -     why Bindings? 
 *      -       cz there are different kinds of DESCRIPTORs (Texture2D, Sampler, Sampler2D, Texture2DArray)
 *      -       a BINDING groups  same kind  of DESCRIPTORs and lets you access like an Array....
 * 
 *      - POOL -> 1++ SETs -> 1++ Bindings -> 1++ Descriptors (of same Type/Kind)
 *      - \todo a Video on this....
 * 
 *      - Finally inside our Texture() function, we Write all the DESCRIPTORs into the SET, maintaining BindingNumber & dstArrayElement
 * 
 * If you found my Explanation COMPLEX:- then read this:- https://stackoverflow.com/a/51716660
 * 
 * an Descriptor Array Example:- http://kylehalladay.com/blog/tutorial/vulkan/2018/01/28/Textue-Arrays-Vulkan.html
 */




/** 
 * We have separated this.... 
 *  + Pipeline varies by Devices, so does Inputs (PushConstants & DescriptorSets) 
 *  + to create the DSetLayout, we DON'T NEED DeadPool/DescriptorPool
 *  + It's more like an spec.... that just has to be passed to VkPipelineLayoutCreateInfo.... 
 */
class amVK_DSetLayout {
  public:
    VkDescriptorSetLayout _SetLayout = nullptr;
    inline VkDescriptorSetLayout& get(void) {return _SetLayout;}

    static inline VkDescriptorSetLayoutCreateInfo setInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, nullptr, 0,
        0, nullptr  /** bindingCount, pBindings */
    };

    amVK_DSetLayout(void) {}
    amVK_DSetLayout(VkDescriptorSetLayout SetLayout) : _SetLayout(SetLayout) {}
    ~amVK_DSetLayout(void) {}

    /**
     * \param amVK_D: has to match with the RTC_DeadPool one, when you gonna create RTC_DSET
     */
    bool create(amVK_DeviceMK2 *amVK_D, uint32_t n, VkDescriptorSetLayoutBinding *pBindings) {
        setInfo.bindingCount = n;
        setInfo.pBindings = pBindings;
        VkResult res = vkCreateDescriptorSetLayout(amVK_D->_D, &setInfo, nullptr, &_SetLayout);

        if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res)); return false;}
        else {return true;}
    }

    /** \todo since this part requires DEVICE, move into RTC_DSET.... */
    inline void destroy(amVK_DeviceMK2 *amVK_D) {
        vkDestroyDescriptorSetLayout(amVK_D->_D, _SetLayout, nullptr);
    }
};














/**
 * amVK_DSetGroup, returned by amVK_DeadPool::allocate()
 */
class amVK_DSetGroup {
  public:
    VkDescriptorPool _DeadPool;
    amVK_Array<VkDescriptorSet> Sets;
};


/**
 *              █████╗ ███╗   ███╗██╗   ██╗██╗  ██╗        ██████╗ ███████╗ █████╗ ██████╗ ██████╗  ██████╗  ██████╗ ██╗     
 *   ▄ ██╗▄    ██╔══██╗████╗ ████║██║   ██║██║ ██╔╝        ██╔══██╗██╔════╝██╔══██╗██╔══██╗██╔══██╗██╔═══██╗██╔═══██╗██║     
 *    ████╗    ███████║██╔████╔██║██║   ██║█████╔╝         ██║  ██║█████╗  ███████║██║  ██║██████╔╝██║   ██║██║   ██║██║     
 *   ▀╚██╔▀    ██╔══██║██║╚██╔╝██║╚██╗ ██╔╝██╔═██╗         ██║  ██║██╔══╝  ██╔══██║██║  ██║██╔═══╝ ██║   ██║██║   ██║██║     
 *     ╚═╝     ██║  ██║██║ ╚═╝ ██║ ╚████╔╝ ██║  ██╗███████╗██████╔╝███████╗██║  ██║██████╔╝██║     ╚██████╔╝╚██████╔╝███████╗
 *             ╚═╝  ╚═╝╚═╝     ╚═╝  ╚═══╝  ╚═╝  ╚═╝╚══════╝╚═════╝ ╚══════╝╚═╝  ╚═╝╚═════╝ ╚═╝      ╚═════╝  ╚═════╝ ╚══════╝
 */
class amVK_DeadPool {
  public:
    VkDescriptorPool _DPool = nullptr;  /** Different from DeadPool 🙃 */
    amVK_DeviceMK2 *_amVK_D = nullptr;

    static inline VkDescriptorPoolSize sizes[1] = { 
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10},    /** A SET can have multiple Bindings, which can have multiple 'DESCRIPTOR's of this type \todo How many is safe & okay? \see maxDescriptorSetSampledImages */
     // {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         10},    /** But at a Total, all DescriptorSET's all BINDINGs combined will not cross more than 10 Image Samplers */
     // {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         10},    /** REF: https://stackoverflow.com/a/64540804  */
    };
    static inline VkDescriptorPoolCreateInfo CI = {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO, nullptr, 0,
        10, 1, sizes   /** maxSets, poolSizeCount, pPoolSizes */
    };


    inline bool create(void) {
        if (_DPool != nullptr) { LOG_EX("Descriptor Pool [_DPool] has already been created"); return false;}
        
        VkResult res = vkCreateDescriptorPool(_amVK_D->_D, &CI, nullptr, &_DPool);
        if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res)); return false;}
        else {return true;}
    }


    /** Functions */
    amVK_DeadPool(amVK_DeviceMK2 *amVK_D) {_amVK_D = amVK_D;}
    ~amVK_DeadPool(void) {}
    inline void destroy(void) {vkDestroyDescriptorPool(_amVK_D->_D, _DPool, nullptr);}
    inline void reset(VkDescriptorPoolResetFlags flags) {vkResetDescriptorPool(_amVK_D->_D, _DPool, flags);}
    /** \see free() in RTC_DSET */


    // ---------- Alloc Set ----------
    static inline VkDescriptorSetAllocateInfo allocInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, nullptr, 
        nullptr, 0, nullptr    /** descriptorPool, descriptorSetCount, pSetLayouts */
    };


    /** 
     * This is Separate, cz you might wanna set once, and then keep allocating multiple times. 
     * + INLINE optimizes it a lot 
     */
    inline bool Settings(uint32_t nLayouts, VkDescriptorSetLayout *pLayouts) {
        allocInfo.descriptorPool = _DPool;
        allocInfo.descriptorSetCount = nLayouts;
        allocInfo.pSetLayouts = pLayouts;
    }
    inline amVK_DSetGroup Allocate(void) {
        VkDescriptorSet *Sets = new VkDescriptorSet[allocInfo.descriptorSetCount];

        vkAllocateDescriptorSets(this->_amVK_D->_D, &allocInfo, Sets);
        return {_DPool, amVK_Array<VkDescriptorSet>(Sets, allocInfo.descriptorSetCount)};
    }


    inline VkDescriptorSet Allocate_Single(VkDescriptorSetLayout SetLayout) {
        allocInfo.descriptorPool = _DPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &SetLayout;

        VkDescriptorSet Set = nullptr;
        VkResult res = vkAllocateDescriptorSets(this->_amVK_D->_D, &allocInfo, &Set);
        if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res)); return nullptr;}
        
        return Set;
    }
};