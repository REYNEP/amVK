#pragma once
#ifndef amVK_INCLUDES_HH
#define amVK_INCLUDES_HH

#include "amVK_Defines.hh"    // You should check this one 😉
#include "vulkan/vulkan.h"

                              // TODO: These four are heavy.... consider making amVK Local Alternatives....
#include <stdint.h>           // These two are included in amVK_Utils too 🤔
#include <stddef.h>
#ifndef amVK_IN_NOSTDLIB
  #include <stdlib.h>         // Currently these 4 can be considered like... used at many places.... 😄
  #include <string.h>         // memcpy: amVK_ArrayDYN 
#else
  #pragma message("amVK_IN_NOSTDLIIB    defined")
#endif

#include "amVK_Logger.hh"
#include "amVK_Utils.hh"
#include "amVK_Types.hh"


#define amVK_pNext_Search(pNext_TYPE, pNext_IN, pNext_VAR) \
    const VkBaseOutStructure* pNext_VAR = reinterpret_cast<const VkBaseOutStructure *>(pNext_IN);    \
    while(pNext_VAR != nullptr) {    \
        if(pNext_VAR->sType == pNext_TYPE)  \
            break;   \
        pNext_VAR = pNext_VAR->pNext; \
    }

#endif