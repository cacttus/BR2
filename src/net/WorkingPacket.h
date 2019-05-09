/**
*
*    @file WorkingPacket.h
*    @date May 16, 2016
*    @author Derek Page
*
*    © 2016 
*
*
*/
#pragma once
#ifndef __WORKINGPACKET_14634195271543313686_H__
#define __WORKINGPACKET_14634195271543313686_H__

#include "../net/NetHeader.h"

namespace Game {
/**
*    @class WorkingPacket
*    @brief
*
*/
class WorkingPacket : public VirtualMemory {
    friend class PacketMakerBase;
private:
    static const int InvalidPacketSize = -1;
    t_string _strPrependTag;
    t_int64 _iLastPacketSize;
    GrowBuffer<char>* _pGrowBuffer;
    void reset();
    t_bool unpackAndCheckCrc(); // Checks the CRC at the end of the packet.
    GrowBuffer<char>* getGrowBuffer() { return _pGrowBuffer; }
    Crc32* _pCrc32;
    void debugPrintPackData(t_string& str);
    //void setLastPacketSize(int i) { _iLastPacketSize = i; }
    //t_int32 getLastPacketSize() { return _iLastPacketSize; }
public:
    void tryGetHeader();
    RetCode tryShitPacket(t_uint64& iPackHeader, t_uint64& iPackOrdinal, t_uint64& iPackSize, t_string& strPackData);

    WorkingPacket(t_string& strPrependTag, Crc32* pCrc32);
    virtual ~WorkingPacket() override;
};

}//ns Game



#endif
