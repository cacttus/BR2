/**
*    @file Client.h
*    @author Derek Page
*
*    © 2011
*
*    © 2011
*    @date 5 / 16 / 2009
*
*/

#pragma once
#ifndef __CLIENT_20835230985708570237_H__
#define __CLIENT_20835230985708570237_H__

#include "../net/ClientSocket.h"
#include "../net/NetworkObject.h"

namespace Game {;
/**
*    @class NetClient
*    @brief Client wrapper interface for connections to servers.
*/
class NetClient : public NetworkObject {
    friend class NetworkWorker;
    ClientSocket* _pClientSocket;
    t_string _strServerIp;

    //RetCode recvData(t_dword waitTimeInMs);// recv into the client's own buffer, returns success when received.
 //   RetCode recvDataRaw(DynamicBuffer<char>& __out_ b, t_int64 waitTimeInMs);
   // RetCode sendDataRaw(Game::IOBuffer& __out_ b, t_dword waitTimeInMs);

public:
    NetClient(NetworkWorker* pwn);
    virtual ~NetClient() override;

    OVERRIDES NetworkObjectType getObjectType() { return NETWORK_OBJECT_CLIENT; }
    OVERRIDES void update() OVERRIDE;
    OVERRIDES void shutdown() OVERRIDE;

    RetCode connectToServer(NetPort srcPort, NetPort dstPort, t_string& dstIP, t_int64 waitTimeInMs);    // - Connect to a server.
    t_string getConnectedServerIp() { return _strServerIp; }
    
    //RetCode sendPacket(PacketType iPackType, Serializable* pObj);
    RetCode sendPacket(PacketType iPackType, t_string& __in_ str);
    
    virtual NonBlockingSocket* getSocket();
};


}
#endif