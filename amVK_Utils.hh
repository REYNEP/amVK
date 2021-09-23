#ifndef amVK_UTILS_H
#define amVK_UTILS_H

#include "vulkan/vulkan.h"
#include <string>
#include "amVK_Logger.hh"

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
template<typename T>
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

    uint32_t half = n / 2;
    uint32_t mid_index = first_index+half-1;
    mergeSort(first_index, mid_index, unsorted, origIndex);
    mergeSort(mid_index+1, last_index, unsorted, origIndex);

    //Finally Merge
    uint32_t p = first_index;               //First Half Start
    uint32_t p_lim = mid_index+1;           //First half End   +1 cz, we don't wanna do (p <= p_lim)
    uint32_t q = p_lim;                     //Second Half Start
    uint32_t q_lim = last_index+1;          //Second Half End

    uint32_t *sorted_tmp = (uint32_t *) calloc(n, sizeof(uint32_t));
    uint32_t *sorted_tmp2 = (uint32_t *) calloc(n, sizeof(uint32_t));   //for origIndex
    uint32_t i = 0;
    while (true) {
        if (p == p_lim) {
            if (last_index-q != n-i-1) {LOG("algorithm Failure code x010");}

            memcpy(unsorted, sorted_tmp, i*sizeof(uint32_t));
            memcpy(origIndex, sorted_tmp2, i*sizeof(uint32_t));
            return true;
        } 
        else if (q == q_lim) {
            if (mid_index-p != n-i-1) {LOG("algorithm Failure code x010");}

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
    //out of while(true) loop, code should never come this far
}

}; //NameSpace amVK_utils
#endif //#ifndef amVK_UTILS_H