/**
*    @file ServerSocket.h
*    @author Derek Page
*
*    © 2011 
*    @date 5 / 16 / 2009
*/
#pragma once
#ifndef __SERVERSOCKET_H__
#define __SERVERSOCKET_H__

#include "../net/VirtualClient.h"
namespace Game {; 
/**
*    @fn ServerSocket
*    @brief TCP - The socket on the server that accepts connections from clients.
*       This doesn't accept data.  Use VirtualClient for getting data from clients.
*/
class ServerSocket : public NonBlockingSocket {
    
public:
    virtual RetCode    createSocket( NetPort port, u_long addr=NULL );
    virtual RetCode    setSockOpts();
    RetCode            acceptConnection( __out_ __in_ ServerClientSocket* newConn );    // - 

    ServerSocket(NetworkWorker* pwn) : NonBlockingSocket(pwn)
    {
    }
    virtual ~ServerSocket() override
    {
    }

};
}
#endif