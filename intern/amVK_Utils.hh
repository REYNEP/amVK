#pragma once

#ifndef amVK_LOGGER_HH
  #include "amVK_Logger.hh"
#endif
#include <stdint.h>
#include <stddef.h>

/** 
 *  ╻ ╻   ┏━┓┏┳┓╻ ╻╻┏    ┏━┓┏━┓┏━┓┏━┓╻ ╻
 *  ╺╋╸   ┣━┫┃┃┃┃┏┛┣┻┓   ┣━┫┣┳┛┣┳┛┣━┫┗┳┛
 *  ╹ ╹   ╹ ╹╹ ╹┗┛ ╹ ╹╺━╸╹ ╹╹┗╸╹┗╸╹ ╹ ╹ 
 * 
 * \note if you use PUSH_BACK macro.... remember to set .data & .n first
 */
template<typename T>
struct amVK_Array {
  T *data;            /** Pointer to the first element [But we dont do/keep_track_of MALLOC, cz.... DUH-Optimizations] */
  uint32_t n;
  uint32_t neXt = 0;  /** ahhh. neXt (2020) FX TV-series 😄 */

  /** CONSTRUCTOR - More like failsafes.... */
  amVK_Array(T *D, uint32_t N) : data(D), n(N) {}
  amVK_Array(T *D, uint32_t N, uint32_t NEXT) : data(D), n(N), neXt(NEXT) {}
  amVK_Array(void) {data = nullptr; n = 0;}
  ~amVK_Array() {}

  /** 
   * inline hugely optimizes it, 
   * [the next inline copy-assignment func below too!] 
   */
  inline T& operator[](uint32_t index) {
    return data[index];
  }

  //filled
  inline size_t size(void) {return static_cast<size_t>(neXt);}
  inline size_t size_filled(void) {return static_cast<size_t>(neXt);}
  inline bool     is_filled(void) {return neXt >= n;}

  //size that can fit
  inline size_t size_alloced(void) {return static_cast<size_t>(n);}

  /** 
   * Default IMPLICIT copy-operator given by the compiler, TRY GodBolt 
   * \param otherArray: why const? https://www.cplusplus.com/articles/y8hv0pDG/  #Const correctness
   */
  inline amVK_Array<T>& operator=(const amVK_Array<T>& otherArray) {
    data = otherArray.data;
    n = otherArray.n;
    neXt = otherArray.neXt;
    return *this;
  }
};




#ifdef amVK_UTILS_IMPLIMENTATION
  #include "string.h"
  #include "stdlib.h"
  void *amVK_memcpy(void *to, void *from, size_t size) {
    return memcpy(to, from, size);
  }

  void *amVK_malloc(size_t size) {
    return malloc(size);
  }

  void amVK_ARRAY_PUSH_BACK_FILLED_LOG(uint32_t n, char *var_name) { 
    if (n == 0) { 
      amVK_LOG_EX("amVK_Array<> " << var_name << ".n = 0;" << "Did you malloc? amASSERT here" << amVK::endl); 
    }
    else {
      amVK_LOG_EX("amVK_Array FILLED!.... reUsing last SLOT"); 
    }
  }
#else
  void *amVK_malloc(size_t size);
  void *amVK_memcpy(void *to, void *from, size_t size);
  void amVK_ARRAY_PUSH_BACK_FILLED_LOG(uint32_t n, char *var_name);
#endif

/** reUse, cz we dont want it to Crash... and if doesn't have much effect */
#define amVK_ARRAY_PUSH_BACK(var) \
  if (var.neXt >= var.n) { \
    amVK_ARRAY_PUSH_BACK_FILLED_LOG(var.n, #var); \
    var.neXt--; \
  } \
  var.data[var.neXt++]

#define amVK_ARRAY_PUSH_BACK_SAFE(var) var.data[var.neXt++]

#define amVK_ArrayDYN_PUSH_BACK(var)      amVK_ARRAY_PUSH_BACK(var)
#define amVK_ArrayDYN_PUSH_BACK_SAFE(var) amVK_ARRAY_PUSH_BACK_SAFE(var)

#define amVK_ARRAY_IS_ELEMENT(array, bool_var, match_what) \
      for (uint32_t i = 0; i < array.n; i++) { \
        if (array.data[i] == match_what) { \
          bool_var = true; \
          break; \
        } \
      }




/** 
 * Dynamic Array.... a.k.a std::vector
 * .n doesn't mean size anymore.... use \fn size() 
 */
template<typename T> 
struct amVK_ArrayDYN : public amVK_Array<T> {
  amVK_ArrayDYN(uint32_t n) : amVK_Array<T>() {
    data = new T[n];
    this->n = n;
  }
  ~amVK_ArrayDYN() {}

  /** you can Delete this object instance after calling this function */
  inline void _delete(void) { delete[] data; }

  /** Makes it 2X sized by default */
  void resize(double size_mul = 2);
  
  /**
   * neXt >= n
   * masterly advisory. unsteady content
   */
  inline bool should_resize(void) { return (bool)(neXt >= n); }

  /**
   * data[n] = data[neXt-1]
   * masterly advisory. unsteady content
   */
  inline void lazy_erase(uint32_t n) {
    if (neXt <= 1) neXt = 0;
    else {
      if (n != neXt-1) data[n] = data[neXt-1];
      neXt--;
    }
  }

  /**
   * neXt--;
   * masterly advisory. unsteady content
   */
  inline void lazy_erase_last_elem(void) { neXt--; }
};

template<typename T>
void amVK_ArrayDYN<T>::resize(double size_mul) {
  amVK_LOG_EX("Resizing array :( " << n);
  
  amVK_ArrayDYN<T> _NEW(n*size_mul);
  amVK_memcpy(_NEW.data, this->data, n * sizeof(T));
  delete[] this->data;

  data = _NEW.data;
  n = _NEW.n;
}


/** GodBolt Test

      // [MemCpy based] Allows amVK_Array<T> smth = {1, 2, 3, ...};     and    amVK_Array<T> smth{1, 2, 3 };      why this [and not operator=] https://stackoverflow.com/q/16935295
      amVK_Array(std::initializer_list<T> ilist) {
        n = ilist.size();
        // if (n > 0) {  Not really needed: [https://stackoverflow.com/q/1087042], tho we must delete...
        data = new T[n];                            //do remember to delete[] yourself
        memcpy(data, ilist.begin(), n * sizeof(T));
        neXt = n;
      }
      // For Optimization Purposes....  when called in 2 lines amVK_Array<T> smth; smth = {1, 2, 3};
      amVK_Array<T>& operator=(std::initializer_list<T> ilist) {
        n = ilist.size();
        data = new T[n];
        memcpy(data, ilist.begin(), n * sizeof(T));
        neXt = n;

        return *this;
        // \todo maybe impl. a safe DEBUG impl. like in https://www.cplusplus.com/articles/y8hv0pDG/
      }

      int main(void) {
        // Calls CONSTRUCTOR with initializer list
        amVK_Array<double> bunch_of_doubles2 = {100.0, 200.0};

        // Calls default constructor
        amVK_Array<double> bunch_of_doubles;
        // Calls operator=
        bunch_of_doubles = {0.0, 1.0, 2.0, 3.0};
        // So combining both of these actually saves CPU time

        amVK_Array<double> another_bunch_of_doubles = bunch_of_doubles;
        return 0;
      }
*/






#define amVK_RESULT uint32_t

/** 
 *              █████╗ ███╗   ███╗██╗   ██╗██╗  ██╗        ██╗   ██╗████████╗██╗██╗     ███████╗
 *   ▄ ██╗▄    ██╔══██╗████╗ ████║██║   ██║██║ ██╔╝        ██║   ██║╚══██╔══╝██║██║     ██╔════╝
 *    ████╗    ███████║██╔████╔██║██║   ██║█████╔╝         ██║   ██║   ██║   ██║██║     ███████╗
 *   ▀╚██╔▀    ██╔══██║██║╚██╔╝██║╚██╗ ██╔╝██╔═██╗         ██║   ██║   ██║   ██║██║     ╚════██║        ┏━  ┏┓╻┏━┓┏┳┓┏━╸┏━┓┏━┓┏━┓┏━╸┏━╸  ━┓
 *     ╚═╝     ██║  ██║██║ ╚═╝ ██║ ╚████╔╝ ██║  ██╗███████╗╚██████╔╝   ██║   ██║███████╗███████║        ┃   ┃┗┫┣━┫┃┃┃┣╸ ┗━┓┣━┛┣━┫┃  ┣╸    ┃
 *             ╚═╝  ╚═╝╚═╝     ╚═╝  ╚═══╝  ╚═╝  ╚═╝╚══════╝ ╚═════╝    ╚═╝   ╚═╝╚══════╝╚══════╝        ┗━  ╹ ╹╹ ╹╹ ╹┗━╸┗━┛╹  ╹ ╹┗━╸┗━╸  ━┛
*/
namespace amVK_Utils {
/**
 * Reports Warnings and Errors based on VkResult
 * Error Messages From [kh-reg-vk/specs/1.2-extensions/man/html/VkResult.html]
 * 
 * BASED-ON: Glfw
 * I later fusioned some words... or added simplified details
 * 
 * \param amVK_RESULT:   #define amVK_RESULT uint32_t.... converted to VkResult inside implimentation
 */
const char *vulkan_result_msg(amVK_RESULT param_result);



/**
 * well.... 😉 see the implementation
 * \param how_far: How many bits [not BYTES] you wanna see
 * \param ptr: pointer to first bit   [a.k.a BYTE Technically]
 */
void memview2b(void *ptr, uint32_t how_far);

/**
 * BYTE    0: The Sign, NEGATIVE if '1', POSITIVE if '0'
 * BYTE  1-7: The Exponent, 1.0 = [0]01111111   [first zero is SIGN],    2.0 = [0]10000000, 4.0 = [0]10000001, 0.5 = [0]01111110
 * BYTE 8-31: The Mantisse....
 */
void view_fp32(float fp32);



/** 
 * TODO: Add support for ReverseSort
 * \param first_index: well the name says it
 * \param  last_index: as the name says. [index of \param unsorted]
 * \param    unsorted: the data that is used/compared to sort
 * \param   origIndex: should be as big as unsorted. Keeps track off OriginalIndexes of Elements from \param first_index to \param last_index
 */
template<typename T>  /** , typename uintXX_t   \todo change in the next commit */
bool mergeSort(uint32_t first_index, uint32_t last_index, T *unsorted, uint32_t *origIndex) {
  //First and 2nd Variable must be MALLOCED before. 2nd one should have filled numbers from zero to last index
    uint32_t n = last_index - first_index + 1;  //number of elements to sort
    if (n <= 1) {
        return true;
    } else if (n == 2) {
        if (unsorted[first_index] < unsorted[last_index]) {
            uint32_t tmp = unsorted[last_index];
            unsorted[last_index] = unsorted[first_index];
            unsorted[first_index] = tmp;

            uint32_t tmp2 = origIndex[last_index];
            origIndex[last_index] = origIndex[first_index];
            origIndex[first_index] = origIndex[tmp2];
            return true;
        }
    }

    uint32_t mid_index = first_index + (n / 2);
    mergeSort(first_index, mid_index, unsorted, origIndex);
    mergeSort(mid_index+1, last_index, unsorted, origIndex);

    //Finally Merge
    uint32_t p = first_index;               //First Half Start
    uint32_t p_lim = mid_index+1;           //First half End   +1 cz, we don't wanna do (p <= p_lim), we wanna do (p < p_lim)
    uint32_t q = p_lim;                     //Second Half Start
    uint32_t q_lim = last_index+1;          //Second Half End

    uint32_t *sorted_tmp = (uint32_t *) amVK_malloc(n * sizeof(uint32_t));
    uint32_t *sorted_tmp2 = (uint32_t *) amVK_malloc(n * sizeof(uint32_t));   //for origIndex
    uint32_t i = 0;
    while (true) {
        if (p == p_lim) {
            if (last_index-q != n-i-1) {amVK_LOG("algorithm Failure code x010");}

            amVK_memcpy(unsorted, sorted_tmp, i*sizeof(uint32_t));
            amVK_memcpy(origIndex, sorted_tmp2, i*sizeof(uint32_t));
            //The Rest of unsorted is already SORTED
            return true;
        } 
        else if (q == q_lim) {
            if (mid_index-p != n-i-1) {amVK_LOG("algorithm Failure code x010");}

            //Copy the rest of Unsorted-Left (a.k.a unsorted+p)
            amVK_memcpy((unsorted+i), (unsorted+p), (p-first_index)*sizeof(uint32_t));
            amVK_memcpy(unsorted, sorted_tmp, i*sizeof(uint32_t));

            amVK_memcpy((origIndex+i), (origIndex+p), (p-first_index)*sizeof(uint32_t));
            amVK_memcpy(origIndex, sorted_tmp2, i*sizeof(uint32_t));
            return true;
        }
        
        if (unsorted[p] < unsorted[q]) {
            sorted_tmp[i] = unsorted[q];
            sorted_tmp2[i] = origIndex[q];
            q++;
            i++;
        } else {
            sorted_tmp[i] = unsorted[p];
            sorted_tmp2[i] = origIndex[p];
            p++;
            i++;
        }
    }
    /** out of while(true) loop, code should never come this far */
}

} //namespace amVK_Utils





#ifdef amVK_UTILS_IMPLIMENTATION
#include <vulkan/vulkan.h>
#include <iostream>
#include <bitset>


const char *amVK_Utils::vulkan_result_msg(amVK_RESULT param_result) {
  switch (param_result)
  {
    case VK_SUCCESS:
        return "Success";

    case VK_NOT_READY:
        return "A fence or query has not yet completed. [Code: VK_NOT_READY]";

    case VK_TIMEOUT:
        return "A wait operation has not completed in the specified time [Code: VK_TIMEOUT]";

    case VK_EVENT_SET:
        return "An event is signaled [Code: VK_EVENT_SET]";

    case VK_EVENT_RESET:
        return "An event is unsignaled [Code: VK_EVENT_RESET]";

    case VK_INCOMPLETE:
        return "A return array was too small for the result [Code: VK_INCOMPLETE]";
        



    case VK_ERROR_OUT_OF_HOST_MEMORY:
        return "A host memory allocation has failed [Code: VK_ERROR_OUT_OF_HOST_MEMORY]";
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        return "A device memory allocation has failed [Code: VK_ERROR_OUT_OF_DEVICE_MEMORY]";
    case VK_ERROR_INITIALIZATION_FAILED:
        return "Initialization of an object could not be completed for implementation-specific reasons [Code: VK_ERROR_INITIALIZATION_FAILED]";
    case VK_ERROR_DEVICE_LOST:
        return "The logical or physical device has been lost [Code: VK_ERROR_DEVICE_LOST]";
    case VK_ERROR_MEMORY_MAP_FAILED:
        return "Mapping of a memory object has failed [Code: VK_ERROR_MEMORY_MAP_FAILED]";




    case VK_ERROR_LAYER_NOT_PRESENT:
        return "A requested layer is not present or could not be loaded [Code: VK_ERROR_LAYER_NOT_PRESENT]";

    case VK_ERROR_EXTENSION_NOT_PRESENT:
        return "A requested extension is not supported [Code: VK_ERROR_EXTENSION_NOT_PRESENT]";

    case VK_ERROR_FEATURE_NOT_PRESENT:
        return "A requested feature is not supported [Code: VK_ERROR_FEATURE_NOT_PRESENT]";

    case VK_ERROR_INCOMPATIBLE_DRIVER:   
        //[VkApplicationInfo] Specs Specified that Above VK1.1 should not return this for any .apiVersion [passed on to vkCreateInstance in CONSTRUCTOR]
        return "The requested version of Vulkan is not supported by the driver or is otherwise incompatible [Code: VK_ERROR_IMCOMPATIBLE_DRIVER]";

    case VK_ERROR_TOO_MANY_OBJECTS:
        return "Too many objects of the type have already been created [Code: VK_ERROR_TOO_MANY_OBJECTS]";

    case VK_ERROR_FORMAT_NOT_SUPPORTED:
        return "A requested format is not supported on this device [Code: VK_ERROR_FORMAT_NOT_SUPPORTED]";

        



    case VK_ERROR_SURFACE_LOST_KHR:
        return "A surface is no longer available [Code: VK_ERROR_SURFACE_LOST_KHR]";
        
    case VK_SUBOPTIMAL_KHR:
        return "A swapchain no longer matches the surface properties exactly, but can still be used [Code: VK_SUBOPTIMAL_KHR]";
    case VK_ERROR_OUT_OF_DATE_KHR:
        return "A surface has changed in such a way that it is no longer compatible with the swapchain [Code: VK_ERROR_OUT_OF_DATE_KHR]";
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
        return "The display used by a swapchain does not use the same presentable ImageLayout [Code: VK_ERROR_INCOMPATIBLE_DISPLAY_KHR]";

    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
        return "The requested window is already connected to a VkSurfaceKHR, or to some other non-Vulkan API [Code: VK_ERROR_NATIVE_WINDOW_IN_USE_KHR]";
    case VK_ERROR_VALIDATION_FAILED_EXT:
        return "A validation layer found an error [Code: VK_ERROR_VALIDATION_FAILED_EXT]";
    default:
        return "ERROR: UNKNOWN VULKAN ERROR";
  }
}

void amVK_Utils::memview2b(void *ptr, uint32_t how_far) {
    int n = ceil(how_far / 32);

    //we wanna show blocks of 32 bits
    uint32_t *xd = reinterpret_cast<uint32_t *> (ptr);
    std::cout << "memview2b: " << std::endl;
    for (int i = 0; i < n; i++) {
        std::bitset<32> y(*(xd + i));
        std::cout << "  " << y << "    - [" << (xd + i) << "]" << std::endl;
    }
    std::cout << std::endl;
}

void amVK_Utils::view_fp32(float fp32) {
    uint32_t the_input_int = *( reinterpret_cast<uint32_t *>(&fp32) );
    if (the_input_int & 0x80000000) {
        std::cout << "-";
        the_input_int = (the_input_int << 1) >> 1;  //Wiki says right/left shift on unsigned int is gonna give '0' as new gen bits
    }
    std::cout << "1.";
    std::cout << std::bitset<23>(the_input_int & 0x007FFFFF);   //0x007FFFF = Mantisse BIT-MASK
    std::cout << " * 2^" << (int)(the_input_int >> 23) - 127 << std::endl << std::endl;
}
#endif
