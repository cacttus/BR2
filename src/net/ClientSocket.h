/**
*    @file ClientSocket.h
*    @author Derek Page
*
*    © 2011
*
*    © 2011
*    @date 5 / 16 / 2009
*/
#pragma once
#ifndef __CLIENTSOCKET_H__
#define __CLIENTSOCKET_H__

#include "../net/NonBlockingSocket.h"

namespace Game {;
/**
*    @class ClientSocket
*    @brief A socket for a client.  Connect to a server.
*
*/
class ClientSocket : public NonBlockingSocket {
public:
    sockaddr_in serverAddr;

    RetCode newSocket(NetPort, NetPort, const char*);    // - Initializes a connection.
    RetCode newSocket(NetPort, NetPort, t_ulong);        // - Initializes a connection.
    RetCode tryConnect();    // - try to connect to the serversocket in question.
    RetCode pollConnect();                                    // - Polls to see if the client is connected.

    virtual RetCode    setSockOpts();

    ClientSocket(NetworkWorker* pwn) :
        NonBlockingSocket(pwn)
    {
    }
    virtual ~ClientSocket() override {
    }
};

}
#endif