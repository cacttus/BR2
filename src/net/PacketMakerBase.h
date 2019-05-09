/**
*
*    @file PacketMakerBase.h
*    @date May 16, 2016
*    @author Derek Page
*
*    © 2016 
*
*
*/
#pragma once
#ifndef __PACKETMAKERBASE_1463418997380732688_H__
#define __PACKETMAKERBASE_1463418997380732688_H__

#include "../base/BaseHeader.h"
#include "../net/NetHeader.h"

namespace Game {
/**
*    @class PacketMakerBase
*    @brief Constructs packets from the TCP stream. Send / Recv.
*       also performs error checking on packets.
*/
class PacketMakerBase : public VirtualMemory {
public:
    const int _cPacketCrcSizeBytes = 4;

    t_uint64 _iCurrentOrdinal; //Current packet being generated **1-based!!!!! Zero is INVALID

    WorkingPacket* _pOutgoingPacket;
    StreamBuffer<char>* _pIncomingDataBuffer;
    StreamBuffer<char>* _pOutgoingDataBuffer;
    std::vector<Packet*> _vecOutgoingPackets;
    WorkingPacket* _pIncomingPacket;//May not need ** we prorbaly just need databuffer.
    std::vector<Packet*> _vecIncomingPackets;
    NonBlockingSocket* _pSocket;
    std::shared_ptr<Crc32> _pCrc32;

    RetCode updateReadSocket();
    Packet* popPacket();
    void pushPacket(Packet*);
    t_bool checkAddPacket();
    void addReceivedDataToPacket();
    void addDataToOutgoingBuffer();
    RetCode updateWriteSocket();
    Packet* makeOutgoingPacket(t_uint64 __in_ iPackId, const t_string& __in_ strData);
    t_uint32 computeCrc32(const t_string& str);//Unsafe.  This should be passed into the packetmaker

public:
    PacketMakerBase( NonBlockingSocket* pSocket);
    virtual ~PacketMakerBase() override;

    RetCode updateSocketComm();
    void getNextPacket(Packet* __out_ out_pack);

    //We may not want ot use ENUMS. Implementation dependent.
   // RetCode sendData(NetPackType __in_ ePackType, const t_string& __in_ strData);
    RetCode sendData(t_uint64 __in_ iPackType, const t_string& __in_ strData);
};

}//ns Game



#endif
