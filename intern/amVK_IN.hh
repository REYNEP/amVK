#pragma once

/** INCLUDES */
#include "amVK_Defines.hh"    // You should check this file individually 😉
#include "vulkan/vulkan.h"    // Vulkan Includes "windows.h" on WIN  cz amVK_Defines.hh defines VK_USE_PLATFORM_WIN32_KHR on WIN

#include <stdint.h>           /** These two are included in amVK_Utils too */
#include <stddef.h>
#ifndef amVK_IN_NOSTDLIB
  #include <stdlib.h>         /** Currently these 4 can't be considered like... used at many places....*/
  #include <string.h>         /** memcpy: amVK_ArrayDYN */
#else
  #pragma message("amVK_IN_NOSTDLIIB    defined")
#endif

#include "amVK_Logger.hh"
#include "amVK_Utils.hh"
#include "amVK_Types.hh"



/** 
 * FORWARD DECLARATION.... cz of Depencency Loop [this file is included in amVK_Device.hh]
 * all other amVK Headers/Files include amVK_Device.hh if they have a `amVK_DeviceMK2` var
 */
#ifndef amVK_DEVICE_HH
  class amVK_DeviceMK2;
#endif

/**
 * I just wanted to access amVK_IN::D_list with D_list[(VkDevice)D]
 * IMPL: \see amVK_Device.cpp
 */
class vec_amVK_Device : public amVK_ArrayDYN<amVK_DeviceMK2 *> {
  public:
    vec_amVK_Device(uint32_t n) : amVK_ArrayDYN<amVK_DeviceMK2 *> (n) {}
    vec_amVK_Device(void) : amVK_ArrayDYN<amVK_DeviceMK2 *>(1) {}
    ~vec_amVK_Device() {}

    amVK_DeviceMK2 *operator[](VkDevice D);
    bool doesExist(amVK_DeviceMK2 *amVK_D);
    bool erase(uint32_t nth);
    uint32_t index(amVK_DeviceMK2 *D);
};




/** 
 * HOLDS all PhysicalDevice Related information....
 * Only used for \var amVK_IN::PD... and it was intended  ([should] not [be] used anywhere else)   
 * \see \fn amVK_CX::load_PD_info() 
 */
struct loaded_PD_info_plus_plus {
  VkPhysicalDevice                 *list = nullptr;         //'Physical Devices List'
  VkPhysicalDeviceProperties      *props = nullptr;         //'Physical Devices Properties'
  VkPhysicalDeviceFeatures     *features = nullptr;         //'Physical Devices Features'
  uint32_t                             n = 0;               //'Physical Devices Count'
  amVK_Array<VkQueueFamilyProperties> *qFamily_lists = nullptr;     //'One Physical Devices can have Multiple qFamilies

  VkPhysicalDeviceMemoryProperties *mem_props = nullptr;    // Used in ImagesMK2

  /** Plus Plus Stuffs */
  bool                           *isUsed = nullptr;
  VkPhysicalDevice                chozen = nullptr;
  uint32_t                  chozen_index = 0;
  uint32_t                   *benchMarks = nullptr;
  uint32_t           *index_sortedByMark = nullptr;

  inline uint32_t index(VkPhysicalDevice pd) {
    for (uint32_t i = 0; i < n; i++) {
      if (pd == list[i]) {return i;}
    }
    return UINT32_T_NULL;  //NOT FOUND
  }

  inline const VkQueueFamilyProperties *get_qFamilies(VkPhysicalDevice pd) {return const_cast<VkQueueFamilyProperties *> (qFamily_lists[index(pd)].data);}
};




#define HEART amVK_IN::heart

/** 
 * Since amVK_CX has a lot of things, decided to separate these, cz are used in many amVK files.... 
 * So this file only contains stuffs that are INTERNALLY needed.... e.g. PD, D_list, instance, activeD
 * only used as amVK_CX base/parent class
 * 
 * amVK_IN doesn;t need to be a ABSTRACT CLASS [class that has at least 1 PURE-VIRTUAL FUNCTION....]
 *   but we decided to keep it this way.... cz amVK_IN shouldn't be created alone.... on its own
 *   so made the load_PD_info virtul....
 *   & other functions as well, cz why not,,,?
 *
 * \note Don't COnvert/TypeCast amVK_CX to this.... you should refer to these vars below from an amVK_CX object
 */
class amVK_IN {
  public:
    static inline                        amVK_IN *heart = nullptr;  /** C++17 */
    static inline amVK_DeviceMK2               *activeD = nullptr;
    static inline VkInstance                   instance = nullptr;  /** internally referred as amVK_IN::instance, for now instance related stuffs are in amVK_CX.cpp*/
    static inline VkApplicationInfo          vk_appInfo = {};
    static inline vec_amVK_Device                D_list = {};
    loaded_PD_info_plus_plus                         PD = {};   /** \ref amVK_Types.hh, call \fn load_PD_info() before, always used as 'HEART->PD' */

    /** \see amVK_CX.hh for func docs */
    virtual bool load_PD_info(bool force_load, bool auto_choose) = 0;
    virtual void activate_device(amVK_DeviceMK2 *D) = 0; /** if (D_list.doesExist(D)) activeD = D; */

    /** called by amVK_CX CONSTRUCTOR */
    amVK_IN(void) {heart = this; _LOG("amVK_IN::heart set!");}
    ~amVK_IN() {}
};

/** these doesn't need amVK_Device.hh to be included */
#define amVK_CHECK_DEVICE(PARAM_D, VAR_D) \
  if (PARAM_D && !HEART->D_list.doesExist(PARAM_D)) { \
    amVK_LOG_EX("\u0027param amVK_DeviceMK2 *" << #PARAM_D << "\u0027 doesn't exist in 'amVK_CX::heart->D_list'"); \
  } else { VAR_D = PARAM_D; }

#define amVK_SET_activeD(VAR_D) \
  if           (!HEART->activeD){ amVK_LOG_EX("Either pass a valid amVK_DeviceMK2 as param" << \
                                         "\n  ...or see amVK_CX::activeD & amVK_CX::activate_device()  "); } \
  else { VAR_D = HEART->activeD; }