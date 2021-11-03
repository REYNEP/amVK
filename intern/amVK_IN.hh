#ifndef amVK_LIB
#define amVK_LIB

#include "amVK_Defines.hh"
#include "vulkan/vulkan.h"  // Vulkan Includes "windows.h" on WIN  cz amVK_Defines.hh defines VK_USE_PLATFORM_WIN32_KHR on WIN

#include <cstdint>
#include <cstddef>


//Common Stuffs
#define amVK_LOGGER_BLI_ASSERT  /** \todo there are other modules inside amVK_Logger.hh.... */
#include "amVK_Logger.hh"
#include "amVK_Utils.hh"
#include "amVK_Types.hh"


//Even if this file does this.... all other amVK Headers include amVK_Device.hh if they have a `amVK_DeviceMK2` var
#ifndef amVK_DEVICE_HH
  class amVK_DeviceMK2;
#endif

#include <vector>
/**
 * I just wanted to access amVK_IN::D_list with D_list[(VkDevice)D]
 * IMPL: \see amVK_DeviceMK2.cpp
 */
class vec_amVK_Device : public std::vector<amVK_DeviceMK2 *> {
  public:
    vec_amVK_Device(uint32_t n) : std::vector<amVK_DeviceMK2 *> (n) {}
    vec_amVK_Device(void) : std::vector<amVK_DeviceMK2 *>() {}
    ~vec_amVK_Device() {}

    amVK_DeviceMK2 *operator[](VkDevice D);
    bool doesExist(amVK_DeviceMK2 *amVK_D);
    uint32_t index(amVK_DeviceMK2 *D);
};



#define HEART amVK_IN::heart

/** 
 * Since amVK_CX has a lot of things, decided to separate these, cz are used in many amVK files.... 
 * only used as amVK_CX base/parent class 
 * 
 * \note Don't COnvert/TypeCast amVK_CX to this.... you should refer to these vars below from an amVK_CX object
 */
class amVK_IN {
  public:
    static inline                        amVK_IN *heart = nullptr;  /** C++17 */
    static inline amVK_DeviceMK2               *activeD = nullptr;
    static inline VkInstance                   instance = nullptr;
    static inline VkApplicationInfo          vk_appInfo = {};
    static inline vec_amVK_Device                D_list = {};
    loaded_PD_info_plus_plus                         PD = {};   /** \ref amVK_Types.hh, call \fn load_PD_info() before */


    virtual VkInstance CreateInstance(void) = 0;
    virtual bool add_InstanceExt(char *extName) = 0;
    virtual bool add_ValLayer(char *vLayerName) = 0;
    virtual bool DestroyInstance(void) = 0;

    virtual bool check_VkSupport(void) = 0; /** \todo */
    virtual void set_VkApplicationInfo(VkApplicationInfo *appInfo = nullptr) = 0;

    /**
     * sets into PD (member var)
     * \return true if already/successfully loaded.... false if enum_PhysicalDevs \returns false
     * \param force_load: pretty much sure you get this one ;)
     * \param auto_choose: does benchmark too, if not already done
     * 
     * \todo our benchmark just sucks....
     */
    virtual bool load_PD_info(bool force_load, bool auto_choose) = 0;


    /** Only way to set activeD, function not internally called.... */
    void activate_device(amVK_DeviceMK2 *D) {
      if (D_list.doesExist(D)) activeD = D;
    }

    /** called by amVK_CX CONSTRUCTOR */
    amVK_IN(void) {heart = this; LOG("amVK_IN::heart set! \n");}
    ~amVK_IN() {}
};

/** these doesn't need amVK_Device.hh to be included */
#define amVK_CHECK_DEVICE(PARAM_D, VAR_D) \
  if (PARAM_D && !HEART->D_list.doesExist(PARAM_D)) { \
    LOG_EX("\u0027param amVK_DeviceMK2 *" << #PARAM_D << "\u0027 doesn't exist in 'amVK_CX::heart->D_list'"); \
  } else { VAR_D = PARAM_D; }

#define amVK_SET_activeD(VAR_D) \
  if           (!HEART->activeD){ LOG_EX("Either pass a valid amVK_DeviceMK2 as param" << \
                                         "\n  ...or see amVK_CX::activeD & amVK_CX::activate_device()  "); } \
  else { VAR_D = HEART->activeD; }

#endif //amVK_LIB