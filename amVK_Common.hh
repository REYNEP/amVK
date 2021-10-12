#ifndef amVK_COMMON_H
#define amVK_COMMON_H
//Includes all the common & preprocessors for all amVK headers/files/modules
#define amVK_LOGGER_BLI_ASSERT  /** \todo there are other modules inside amVK_Logger.hh.... */
#include "amVK_Logger.hh"
#include "amVK_Utils.hh"
#define VEC_amVK_DEVICE
#include "amVK_Types.hh"

#define HEART amVK_IN::heart
#define HEART_CX static_cast<amVK_CX *>(HEART)
#define LOG_activeD_nullptr() LOG_EX("Either pass a valid amVK_Device (created with amVK_CX::CreateDevice/MK2) as param,    or see amVK_CX::activeD & amVK_CX::activate_device()")

/** Used in ALL [mostly] CLASS Constructors.... \see LOG in amVK_Logger.h, LOG_EX is prints an extra FIRST-LINE giving __func__, __line__*/
#define amVK_CHECK_DEVICE(PARAM_D, VAR_D) \
  if (PARAM_D && !HEART->D_list.doesExist(PARAM_D)) { \
    LOG_EX("\u0027param amVK_Device *" << #PARAM_D << "\u0027 doesn't exist in 'amVK_CX::heart->D_list'"); \
  } else { VAR_D = PARAM_D; }

#define amVK_SET_activeD(VAR_D) \
  if           (!HEART->activeD){ LOG_activeD_nullptr(); } \
  else { VAR_D = HEART->activeD; }

#include "vulkan/vulkan.h"
#ifndef amVK_HH
    class amVK_CX;
#endif
#ifndef amVK_DEVICE_H
    class amVK_Device;
#endif



/** Since amVK_CX has a lot of things, decided to separate these, cz are used in many amVK files.... */
class amVK_IN {
  public:
    static amVK_IN *heart;      
    static amVK_Device *activeD;
    static VkInstance instance;
    static VkApplicationInfo vk_appInfo;

    amVK_IN(void) {heart = this; LOG("amVK_IN::heart set! \n\n");} //called by amVK_CX CONSTRUCTOR
    ~amVK_IN() {}

    /** HEART->D_list.doesExist() used to check if Device exists or not */
    vec_amVK_Device         D_list{};
};

#endif //amVK_COMMON_H