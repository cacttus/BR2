/**
*  @file BitHacks.h
*  @date March 23, 2014
*  @author MetalMario971
*/
#pragma once
#ifndef __BITHACKS_90737118105551832723055_H__
#define __BITHACKS_90737118105551832723055_H__

#include "../base/MachineTypes.h"
namespace BR2 {

//low level bit stuff
class BitHacks {
public:
    static void pbin(uint8_t b);//write the tbyte in binary
    static void pbin(uint16_t b);
    static void pbin(uint8_t b, char c);
    static void pbinx(uint8_t b);//No LF chars
    static char bzzb(char* sz);// - Convert a binary char string to a byte
    static uint16_t bzzw(char* sz);// - Convert binary string to a word
    static uint8_t bitcount(uint8_t x);// - Count the number of bits in a byte. Kernighan made it faster?? but wtf.
    static uint8_t bitcount(uint16_t x);// - Count the number of bits in a tbyte. Kernighan made it faster?? but wtf.
    static uint8_t bitcount(int32_t x);// - Count the number of bits in ny tbyte/word
    static uint8_t bitcount(uint32_t x);// - Count the number of bits in ny tbyte/word
    static uint8_t bitcount(int64_t x);// - Count the number of bits in ny tbyte/word
    static uint8_t trailingzeros(int32_t x);// - Returns the position of the first bit in the little endian tbyte. Returns the LOG2 of the first bit.
};

}//ns game



#endif
