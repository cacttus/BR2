//#ifdef _DEBUG
#include "../math/BitHacks.h"
#include "../base/Gu.h"
namespace BR2 {
//
//    FUNCTIONS
//
//write the tbyte in binary
void BitHacks::pbin(uint8_t b) {
    for (int i = 7; i >= 0; --i) {
        if (b&(1 << i)) {
            Gu::print("1");
        }

        else {
            Gu::print("0");
        }
    }
    std::cout << std::endl;
}
void BitHacks::pbin(uint16_t b)
{
    for (int i = 15; i >= 0; --i) {
        if (b&(1 << i)) {
            Gu::print("1");
        }
        else {
            Gu::print("0");
        }
    }
    std::cout << std::endl;
}
void BitHacks::pbin(uint8_t b, char c) {
    for (int i = 7; i >= 0; --i) {
        if (b&(1 << i)) {
            Gu::print(c);
        }
        else {
            Gu::print(" ");
        }
    }
    std::cout << std::endl;
}
//No LF chars
void BitHacks::pbinx(uint8_t b) {
    for (int i = 7; i >= 0; --i) {
        if (b&(1 << i)) {
            Gu::print("1");
        }
        else {
            Gu::print("0");
        }
    }
}
// - Convert a binary char string to a byte
char BitHacks::bzzb(char* sz) {
    char* c = sz;
    char ret = 0;
    for (int i = 0; i < 8; ++i) {
        if ((c[(7 - i)]) == '1') {
            ret |= (1 << (i));
        }

    }
    return ret;
}
// - Convert binary string to a word
uint16_t BitHacks::bzzw(char* sz) {
    char* c = sz;
    char ret = 0;
    for (int i = 0; i < 16; ++i) {
        if ((c[(15 - i)]) == '1') {
            ret |= (1 << (i));
        }
    }
    return ret;
}
// - Count the number of bits in a byte.
// Kernighan made it faster?? but wtf.
uint8_t BitHacks::bitcount(uint8_t x) {/*
                                    old
                                    int n=0;
                                    for(int i=0; i<8; ++i)
                                    if(b&(1<<i))
                                    n++;
                                    return n;

                                    */
                                    //    while(x) if((x>>=1)&1) r++;

    uint8_t r = 0;
    for (; x; x >>= 1) {
        r += (uint8_t)x & 1;
    }
    return r;
}
// - Count the number of bits in a tbyte.
// Kernighan made it faster?? but wtf.
uint8_t BitHacks::bitcount(uint16_t x) {
    uint8_t r = 0;
    for (; x; x >>= 1) {
        r += (uint8_t)x & 1;
    }
    return r;
}

// - Count the number of bits in ny tbyte/word
uint8_t BitHacks::bitcount(int32_t x) {
    uint8_t r = 0;
    for (; x; x >>= 1) {
        r += (uint8_t)x & 1;
    }
    return r;
}
// - Count the number of bits in ny tbyte/word
uint8_t BitHacks::bitcount(uint32_t x) {
    uint8_t r = 0;
    for (; x; x >>= 1) {
        r += (uint8_t)x & 1;
    }
    return r;
}
// - Count the number of bits in ny tbyte/word
uint8_t BitHacks::bitcount(int64_t x) {
    uint8_t r = 0;
    for (; x; x >>= 1) {
        r += (uint8_t)x & 1;
    }
    return r;
}
// - Returns the position of the first bit in the little endian tbyte.
// Returns the LOG2 of the first bit.
uint8_t BitHacks::trailingzeros(int32_t x) {
    uint8_t r = 0;
    for (; !(x & 1); x >>= 1) {
        r++;
    }

    return r;
}


}



//#endif
