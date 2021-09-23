#ifndef amVK_LOGGER
#define amVK_LOGGER
//Include Guard

#include <iostream>
#include <bitset>

// LOGGER
#define LOG(x) std::cout << x << std::endl

#define LOG_LOOP(log_heading, iterator_var, loop_limit, log_inside_loop) \
    LOG(log_heading); \
    for (int iterator_var = 0, lim = loop_limit; iterator_var < lim; iterator_var++) { \
        LOG(log_inside_loop); \
    } \
    LOG("\n")


//See: https://stackoverflow.com/questions/27174929/what-is-a-best-way-to-check-if-compiler-supports-func-etc
//__FUNCTION__, __func__ are not Macros, rather they are Variables
//But the reason why LOG_EX Works is because LOG_EX uses another function LOG which doesn't let __LINE__ to be converted directly on this files line 23
#if defined(__GNUC__)
  #define __FUNCINFO__ __FUNCTION__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
  #define __FUNCINFO__ __func__
#elif defined(_MSC_VER)
  #define __FUNCINFO__ __func__
#else
  #define __FUNCINFO__ "[FuncInfo not supported by current compiler]"
#endif
#define LOG_EX(x) LOG(__FILE__ << " || Function: " << __FUNCINFO__ << "() - Line" << __LINE__ << std::endl << "Log-Message:- "<< x << std::endl)
// MSVC Has Got a Huge amount of Predefined Macros:- https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros
#define LOG_MANGLED_MSVC(x) LOG_EX(x << " MANGLED NAME: " << __FUNCDNAME__)


//am_DEV should be defined by the developer if they want to debug..
#if defined(am_DEV)
  #define amLOG(x) LOG(x)
  #define amLOG_EX(x) LOG_EX(x)
#else
  #define amLOG(x)
  #define amLOG_EX(x)
#endif

#define amLOG_LOOP(log_heading, iterator_var, loop_limit, log_inside_loop) \
    amLOG(log_heading); \
    for (int iterator_var = 0, lim = loop_limit; iterator_var < lim; iterator_var++) { \
        amLOG(log_inside_loop); \
    } \
    amLOG("\n")




//amVK_CHK
#define amVK_CHK(x, y) if(!x){LOG_EX(y);}

//amVK_ABBORT [TODO: Check if it has any BUGS. If its a Bug rel. to ';' then remove the BRACKETS after for() statement, and remove the ';' after sleep_for(1000ms)]
#ifdef amVK_ABBORT_SUPPORT
  #include <cstdlib>
  #include <chrono>
  #include <thread>

  #define amABBORT(sec) for (int amVK_LOOP_COUNTER = 0; amVK_LOOP_COUNTER < sec; amVK_LOOP_COUNTER++) {\
                          LOG((sec - amVK_LOOP_COUNTER) << "...");\
                          std::this_thread::sleep_for(std::chrono::seconds(sec));\
                        }
#else 
  #define amABBORT(sec)
#endif //amVK_ABBORT_SUPPORT

#include "amVK_Utils.hh"
#define VK_CHECK(res, return_err) \
    if (res != VK_SUCCESS) {LOG_EX(amVK_Utils::vulkan_result_msg(res)); return return_err;}

#endif //#ifndef amVK_LOGGER



//TIMER
#ifdef INCLUDE_TIMER
#include <chrono>

typedef struct noob_timer__ {
  std::chrono::steady_clock::time_point time_now, time_flushPoint;
} noob_timer;

/** Make a array of this STRUCT (like below), then pass the  .time_spent to TIMER_STORE
 *   func_timer all[2] = {
 *     {"amVK_CreateDevice", 0.0},
 *     {"amVK_Pipeline", 0.0}
 *   };
*/
typedef struct noob_timer_store__ {
  char *func_name;
  double time_spent;
} noob_timer_store;


#define TIMER_FLUSH(var) \
  var.time_flushPoint = std::chrono::high_resolution_clock::now();

// You can just FLUSH at the Beginning and keep LOGGING, but Logging takes ~0.0006s
#define TIMER_LOG(var) \
  var.time_flushPoint = std::chrono::high_resolution_clock::now(); \
  LOG(((std::chrono::duration<double>)(var.time_now - var.time_flushPoint)).count());

  
// Use something like     arrayDouble[10]     [type of variable must be 'double'  but it can be an array]
#define TIMER_STORE(var, x) \
  var.time_now = std::chrono::high_resolution_clock::now(); \
  x = ((std::chrono::duration<double>)(var.time_now - var.time_flushPoint)).count();

#endif    //#ifdef INCLUDE_TIMER
