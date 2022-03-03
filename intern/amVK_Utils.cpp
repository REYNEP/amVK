#define amVK_UTILS_CPP
#include "amVK_Utils.hh"

#include <bitset>
void amVK_Utils::memview2b(void *ptr, uint32_t how_far) {
    int n = ceil(how_far / 32);

    //we wanna show blocks of 32 bits
    uint32_t *xd = reinterpret_cast<uint32_t *> (ptr);
    LOG("memview2b: ");
    for (int i = 0; i < n; i++) {
        std::bitset<32> y(*(xd + i));
        LOG("  " << y << "    - [" << (xd + i) << "]");
    }
    LOG("");
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