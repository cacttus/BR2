/**
*
*    @file Packet.h
*    @date May 16, 2016
*    @author Derek Page
*
*    © 2016 
*
*
*/
#pragma once
#ifndef __PACKET_1463447516422061023_H__
#define __PACKET_1463447516422061023_H__

#include "../net/NetHeader.h"

namespace Game {
/**
*    @class Packet
*    @brief
*
*/
class Packet : public VirtualMemory {
friend class WorkingPacket;
friend class PacketMakerBase;
friend class NetServer;
public:
   // static const t_int64 PacketTagSize = 2;  //This is now in NetworkManager
    static const t_uint64 PacketLengthSize = 8;
    static const t_uint64 PacketHeaderIdSizeBytes = 8;
    static const t_uint64 PacketOrdinalSizeBytes = 8;
    //**Data..
    static const t_uint64 PacketCrcSizeBytes = 4;
    static const t_uint64 PacketDefaultChunkSize = 1024;

    //**WARNING: we return packets by value in packetmaker.  Only add limited data here.
private:
    //**NOTE: if adding new members make sure to update copyTo()
    t_uint64 _iHeaderId;   // Packet Header Id parsed from 8 values. [xxxxxxxx]
    t_uint64 _iOrdinal;   // Packet Id Order as appears on the client who sent hte packet. - Increment this this is a standin for time - packet time not totally reliable.
    t_uint64 _iTotalSizeBytes;
    DynamicBuffer<char>* _pData;//May be Null

    ServerClientSocket* _pClient; // May be NULL

    void setData(t_string& strData);

public:
    Packet();
    virtual ~Packet() override;

    DynamicBuffer<char>* getData();  // May be NULLL
    t_uint64 getOrdinal();
    t_uint64 getTotalSizeBytes();
    ServerClientSocket* getClient() { return _pClient; }
    t_bool getHasData();

    t_uint64 getPacketTypeId();
    void copyTo(Packet* pOther);

    void reset();
};

}//ns Game



#endif
