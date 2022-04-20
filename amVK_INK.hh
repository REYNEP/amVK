#pragma once
#ifndef amVK_INCLUDES_HH
#define amVK_INCLUDES_HH

#include "amVK_Defines.hh"    // You should check this one 😉
#include "vulkan/vulkan.h"

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

#endif