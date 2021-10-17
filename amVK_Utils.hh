#ifndef amVK_UTILS_H
#define amVK_UTILS_H

#include "vulkan/vulkan.h"
#include <vector>   //vec_amVK_Device for now
#if defined(amVK_DEVICE_CPP) || defined(VEC_amVK_DEVICE)
  #ifndef amVK_DEVICE_H
    class amVK_Device;
  #endif
#endif
#include <string>
#include "amVK_Logger.hh"


/** 
 *  ╻ ╻   ┏━┓┏┳┓╻ ╻╻┏    ┏━┓┏━┓┏━┓┏━┓╻ ╻
 *  ╺╋╸   ┣━┫┃┃┃┃┏┛┣┻┓   ┣━┫┣┳┛┣┳┛┣━┫┗┳┛
 *  ╹ ╹   ╹ ╹╹ ╹┗┛ ╹ ╹╺━╸╹ ╹╹┗╸╹┗╸╹ ╹ ╹ 
 */
/** [amVK_types.hh] */
template<typename T> 
struct amVK_Array {
    T *data;    //Pointer to the first element [But we dont do/keep_track_of MALLOC, so DUH-Optimizations]
    uint32_t n;
    uint32_t next_add_where = 0;

    /** CONSTRUCTOR - More like failsafes.... */
    amVK_Array(T *D, uint32_t N) : data(D), n(N) {if (D == nullptr || N == 0) {LOG_EX("ERROR");}}
    amVK_Array(void) {data = nullptr; n = 0;}

    inline T& operator[](uint32_t index) {
      return data[index];
    }
    inline size_t size(void) {return static_cast<size_t>(n);}
};
#ifndef amVK_RELEASE
  #ifdef amVK_CPP
    void amVK_ARRAY_PUSH_BACK_FILLED_LOG(uint32_t n, char *var_name, char *type_id_name) { 
      if (n == 0) { LOG_EX("amVK_Array<> " << var_name << ".n = 0;" << "Did you malloc? amASSERT here" << std::endl << "typeid(" << var_name << ").name() :- " << type_id_name);
                    amASSERT(true); }
      LOG_EX("amVK_Array FILLED!.... reUsing last SLOT"); 
    }  
  #else
    void amVK_ARRAY_PUSH_BACK_FILLED_LOG(uint32_t n, char *var_name, char *type_id_name);
  #endif

  #include <typeinfo>
  /** reUse, cz we dont want it to Crash... and if doesn't have much effect */
  #define amVK_ARRAY_PUSH_BACK(var) \
    if (var.next_add_where >= var.n) { \
      amVK_ARRAY_PUSH_BACK_FILLED_LOG(var.n, #var, (char *)typeid(var).name()); \
      var.next_add_where--; \
    } \
    var.data[var.next_add_where++]
#else
  #define amVK_ARRAY_PUSH_BACK(var) var.data[var.next_add_where++]
#endif



/** 
 *              █████╗ ███╗   ███╗██╗   ██╗██╗  ██╗        ██╗   ██╗████████╗██╗██╗     ███████╗
 *   ▄ ██╗▄    ██╔══██╗████╗ ████║██║   ██║██║ ██╔╝        ██║   ██║╚══██╔══╝██║██║     ██╔════╝
 *    ████╗    ███████║██╔████╔██║██║   ██║█████╔╝         ██║   ██║   ██║   ██║██║     ███████╗
 *   ▀╚██╔▀    ██╔══██║██║╚██╔╝██║╚██╗ ██╔╝██╔═██╗         ██║   ██║   ██║   ██║██║     ╚════██║        ┏━  ┏┓╻┏━┓┏┳┓┏━╸┏━┓┏━┓┏━┓┏━╸┏━╸  ━┓
 *     ╚═╝     ██║  ██║██║ ╚═╝ ██║ ╚████╔╝ ██║  ██╗███████╗╚██████╔╝   ██║   ██║███████╗███████║        ┃   ┃┗┫┣━┫┃┃┃┣╸ ┗━┓┣━┛┣━┫┃  ┣╸    ┃
 *             ╚═╝  ╚═╝╚═╝     ╚═╝  ╚═══╝  ╚═╝  ╚═╝╚══════╝ ╚═════╝    ╚═╝   ╚═╝╚══════╝╚══════╝        ┗━  ╹ ╹╹ ╹╹ ╹┗━╸┗━┛╹  ╹ ╹┗━╸┗━╸  ━┛
 *
*/
namespace amVK_Utils {
/**
 * Reports Warnings and Errors based on VkResult
 * Error Messages From [kh-reg-vk/specs/1.2-extensions/man/html/VkResult.html]
 * \see impl in amVK.cpp
 */
const char *vulkan_result_msg(VkResult result);



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

    uint32_t *sorted_tmp = (uint32_t *) malloc(n * sizeof(uint32_t));
    uint32_t *sorted_tmp2 = (uint32_t *) malloc(n * sizeof(uint32_t));   //for origIndex
    uint32_t i = 0;
    while (true) {
        if (p == p_lim) {
            if (last_index-q != n-i-1) {LOG("algorithm Failure code x010");}

            memcpy(unsorted, sorted_tmp, i*sizeof(uint32_t));
            memcpy(origIndex, sorted_tmp2, i*sizeof(uint32_t));
            //The Rest of unsorted is already SORTED
            return true;
        } 
        else if (q == q_lim) {
            if (mid_index-p != n-i-1) {LOG("algorithm Failure code x010");}

            //Copy the rest of Unsorted-Left (a.k.a unsorted+p)
            memcpy((unsorted+i), (unsorted+p), (p-first_index)*sizeof(uint32_t));
            memcpy(unsorted, sorted_tmp, i*sizeof(uint32_t));

            memcpy((origIndex+i), (origIndex+p), (p-first_index)*sizeof(uint32_t));
            memcpy(origIndex, sorted_tmp2, i*sizeof(uint32_t));
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





/**
 * I just wanted to access amVK_CX::_device_list with _device_list[(VkDevice)D]
 * IMPL: \see amVK_Device.cpp
 * Declarations and Includes at Top
 */
#if defined(amVK_DEVICE_CPP) || defined(VEC_amVK_DEVICE)

  class vec_amVK_Device : public std::vector<amVK_Device *> {
   public:
    vec_amVK_Device(uint32_t n) : std::vector<amVK_Device *> (n) {}
    vec_amVK_Device(void) : std::vector<amVK_Device *>() {}
    ~vec_amVK_Device() {}

    //amVK Stores a lot of different kinda data like this. Maybe enable a option to store these in HDD as cache
    amVK_Device *operator[](VkDevice D);
    bool doesExist(amVK_Device *amVK_D);  /** cz, VkDevice is inside amVK_Device class, that means the need to include amVK_Device.hh in every single file*/
  };

  #ifdef IMPL_VEC_amVK_DEVICE   //in amVK_Device.cpp
    #ifndef amVK_LOGGER
      #include "amVK_Logger.hh"
    #endif
    amVK_Device *vec_amVK_Device::operator[](VkDevice D) {
      amVK_Device **data = this->data();
      for (int i = 0, lim = this->size(); i < lim; i++) {
        if (data[i]->_D == D) {
          return data[i];
        }
      }
      LOG_EX("LogicalDevice doesn't Exist");
      return nullptr;
    }


    #include <typeinfo>
    bool vec_amVK_Device::doesExist(amVK_Device *amVK_D) {
      amVK_Device **data = this->data();
      for (int i = 0, lim = this->size(); i < lim; i++) {
        if (data[i] == amVK_D) {
          return true;
        }
      }
      LOG_EX("amVK_Device doesn't Exist in " << typeid(this).name());
    }
  #endif //IMPL_VEC_amVK_DEVICE
#endif //amVK_DEVICE_CPP || VEC_amVK_DEVICE


}; //NameSpace amVK_utils
#endif //#ifndef amVK_UTILS_H