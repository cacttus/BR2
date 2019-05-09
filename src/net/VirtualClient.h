/**
*    @file VirtualClient.h
*    @author Derek Page
*
*    @date 5 / 16 / 2009
*
*/
#pragma once
#ifndef __VIRTUAL_CLIENT_H__
#define __VIRTUAL_CLIENT_H__

#include "../net/NonBlockingSocket.h"
namespace Game {; 

/**
*    @class VirtualClient
*    @brief A virtual client representation for the server.
*/
class ServerClientSocket : public NonBlockingSocket {
    t_uint32 _iClientConnectionTimeoutInSeconds;
    t_string _strClientIp;
public:
    RetCode pollForActive();    // - Polls the socket for receive activity.
    RetCode setSockOpts();

    RetCode acceptServerConnection(NonBlockingSocket* pServerSocket);
    t_string getClientIp() { return _strClientIp; }
    ServerClientSocket(NetworkWorker* pwn);
    virtual ~ServerClientSocket() override;
};


}
#endif