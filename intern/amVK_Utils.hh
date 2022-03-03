#pragma once

/** 
 * You should instead #include "amVK_IN.hh" rather than including amVK_Utils.hh directly....
 * also amVK_Logger is included there before utils....
 */
#ifndef amVK_LOGGER_HH
  #include "amVK_Logger.hh"
#endif


/** 
 *  ╻ ╻   ┏━┓┏┳┓╻ ╻╻┏    ┏━┓┏━┓┏━┓┏━┓╻ ╻
 *  ╺╋╸   ┣━┫┃┃┃┃┏┛┣┻┓   ┣━┫┣┳┛┣┳┛┣━┫┗┳┛
 *  ╹ ╹   ╹ ╹╹ ╹┗┛ ╹ ╹╺━╸╹ ╹╹┗╸╹┗╸╹ ╹ ╹ 
 * 
 * \note if you use PUSH_BACK macro.... remember to set .data & .n first
 */
#include <cstring>    // memcpy
#include <initializer_list>
/** [amVK_types.hh] */
template<typename T> 
struct amVK_Array {
    T *data;    //Pointer to the first element [But we dont do/keep_track_of MALLOC, so DUH-Optimizations]
    uint32_t n;
    uint32_t neXt = 0;  /** ahhh. neXt (2020) FX TV-series 😄 */

    /** CONSTRUCTOR - More like failsafes.... */
    amVK_Array(T *D, uint32_t N) : data(D), n(N) {}
    amVK_Array(T *D, uint32_t N, uint32_t NEXT) : data(D), n(N), neXt(NEXT) {}
    amVK_Array(void) {data = nullptr; n = 0;}
    ~amVK_Array() {}

    inline T& operator[](uint32_t index) {    //inline hugely optimizes it, [the next inline copy-assignment func below too!]
      return data[index];
    }
    /** Default IMPLICIT copy-operator given by the compiler, TRY GodBolt */
    inline amVK_Array<T>& operator=(const amVK_Array<T>& otherArray) { //why const? https://www.cplusplus.com/articles/y8hv0pDG/  #Const correctness
      data = otherArray.data;
      n = otherArray.n;
      neXt = otherArray.neXt;
      return *this;
    }

    /** [MemCpy based] Allows amVK_Array<T> smth = {1, 2, 3, ...};     and    amVK_Array<T> smth{1, 2, 3 };      why this [and not operator=] https://stackoverflow.com/q/16935295 */
    amVK_Array(std::initializer_list<T> ilist) {
      n = ilist.size();
      /** if (n > 0) {  Not really needed: [https://stackoverflow.com/q/1087042], tho we must delete... */
        data = new T[n];                            //do remember to delete[] yourself
        memcpy(data, ilist.begin(), n * sizeof(T));
        neXt = n;
    }
    /** For Optimization Purposes....  when called in 2 lines amVK_Array<T> smth; smth = {1, 2, 3}; */
    amVK_Array<T>& operator=(std::initializer_list<T> ilist) {
      n = ilist.size();
        data = new T[n];
        memcpy(data, ilist.begin(), n * sizeof(T));
        neXt = n;

      return *this;
      /** \todo maybe impl. a safe DEBUG impl. like in https://www.cplusplus.com/articles/y8hv0pDG/ */
    }

    /** GodBolt Test
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

    //filled
    inline size_t size(void) {return static_cast<size_t>(neXt);}
    inline size_t size_filled(void) {return static_cast<size_t>(neXt);}

    //size that can fit
    inline size_t size_alloced(void) {return static_cast<size_t>(n);}
};
#ifndef amVK_RELEASE
  #ifdef amVK_UTILS_CPP
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
    if (var.neXt >= var.n) { \
      amVK_ARRAY_PUSH_BACK_FILLED_LOG(var.n, #var, (char *)typeid(var).name()); \
      var.neXt--; \
    } \
    var.data[var.neXt++]
#else
  #define amVK_ARRAY_PUSH_BACK(var) var.data[var.neXt++]
#endif


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
  amVK_ArrayDYN(uint32_t n) : amVK_Array() {
    data = new T[n];
    this->n = n;
  }
  ~amVK_ArrayDYN() {}

  /** Makes it 2X sized by default */
  void resize(double size_mul = 2) {
    LOG_EX("Resizing array :( " << n);
    
    amVK_ArrayDYN<T> _NEW(n*size_mul);
    memcpy(_NEW.data, this->data, n * sizeof(T));
    delete[] this->data;

    data = _NEW.data;
    n = _NEW.n;
  }

  inline bool should_resize(void) {
    return (bool)(neXt >= n);
  }

  //Delete this object instance after calling this function
  inline void _delete(void) { delete[] data; }
};

#define amVK_ArrayDYN_PUSH_BACK(var) var.data[var.neXt++]






#include "vulkan/vulkan.h"
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


} //namespace amVK_Utils
