/**
*    @file Server.h
*    @author Derek Page
*
*    © 2011
*    @date 5 / 16 / 2009
*    @brief  - Holds the list of client connections and checks their FD's in an update function
*/
#pragma once
#ifndef __NETSERVER_298357239857293857239857_H__
#define __NETSERVER_298357239857293857239857_H__

#include "../net/NetworkObject.h"
#include "../net/VirtualClient.h"
#include "../net/ServerSocket.h"

namespace Game {
;

/**
*    @class Server
*    @details Control center for managing clients, for the server.
*
*/
class NetServer : public NetworkObject {
    friend class NetworkWorker;
protected:
    ServerSocket*                _pMySocket;        // - This socket will receive new connections.
    VectorEx<ServerClientSocket*>    _vecClients;    // - List of client addresses.
    VectorEx<ServerClientSocket*>    _vecDeadClients; // to clean
    VectorEx<ServerClientSocket*>    _vecNewClients;        // clients that connetc tg

    void cleanup();
    RetCode handleClientData(ServerClientSocket* pClient);    // - Poll for new data 
    virtual RetCode pollForNewClients();                // - Poll for new connections.
   // virtual RetCode waitClientForData( __in_ VirtualClient *client, DataBuffer& pdb, int waitTimeInMs=-1, int* out_size=NULL );
    virtual RetCode pollClientActivity(ServerClientSocket*);                    // - Polls the client to see if he is active, if there is a timeout, the client is disconnected.

public:

    NetServer(NetworkWorker* pwn);
    virtual ~NetServer() override;

    RetCode init(NetPort port); // - Initialize, must be called first.
    OVERRIDES void update();
    void closeClient(ServerClientSocket*); // - Close the client socket.

    FORCE_INLINE NonBlockingSocket* getSocket() { return (NonBlockingSocket*)_pMySocket; } // - Get the socket.
    FORCE_INLINE ServerSocket* getServerSocket() { return _pMySocket; } // - Get the socket.
    FORCE_INLINE t_memsize getNumClients() { return _vecClients.size(); }                        // - Returns the number of clients.
    OVERRIDES NetworkObjectType getObjectType() { return NETWORK_OBJECT_SERVER; }
    OVERRIDES void shutdown() OVERRIDE;
};

}//ns game

#endif