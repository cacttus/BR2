/**
*
*    @file Crc32.h
*    @date May 16, 2016
*    @author Derek Page
*
*    © 2016 
*
*
*/
#pragma once
#ifndef __CRC32_1463460399427663938_H__
#define __CRC32_1463460399427663938_H__

#include "../base/BaseHeader.h"

namespace Game {
/**
*    @class Crc32
*    @brief
* Simplified version of https://damieng.com/blog/2006/08/08/calculating_crc32_in_c_and_net
*/
class Crc32 : public VirtualMemory {
    enum { UiTableLen = 256 };
    uint32_t _uiTable[UiTableLen];
    uint32_t _uiPolynomial;
    uint32_t _uiSeed;

    void initializeTable(uint32_t polynomial, uint32_t* createTable, uint32_t tableLen = 256);
    uint32_t calculateHash(uint32_t* table, uint32_t seed, const char* buffer, int start, int size);
public:
    Crc32(uint32_t polynomial = 0xedb88320, uint32_t seed=0xffffffff );
    virtual ~Crc32() override;

    uint32_t compute(const t_string& buffer);
};



}//ns Game



#endif
