#include "../net/ServerSocket.h"
#include "../net/NetworkManager.h"
#include "../base/GlobalUtils.h"
#include "../scene/EngineSystemParams.h"    

namespace Game {; 
/**
*    @fn newSocket()
*
*
*/
RetCode ServerSocket::createSocket( NetPort port, u_long addr )
{
    RetCode ret;

    BroLogInfo("Creating server socket.");

    //nbsocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );    // - 0 - let the service provider choose the protocol.

    ///original from NonBlockingSocket
    nbsocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );    // - 0 - let the service provider choose the protocol.

    memset(&socketAddr,0,sizeof(sockaddr));

    if( addr==NULL )
        socketAddr.sin_addr.S_un.S_addr = INADDR_ANY;
    else
        socketAddr.sin_addr.S_un.S_addr = addr;
    socketAddr.sin_family = AF_INET;
    socketAddr.sin_port = htons(port);

    setState(SOCK_CREATED);
    if( (ret=setSockOpts())!=SOCK_SUCCESS)
    {
        BroLogInfo("..SetSockOpts failed.");
        return ret;
    }
///

    // - Create the base socket, but we also need to bind the socket on the server end.
    if( bind( nbsocket, (sockaddr*)&socketAddr, sizeof(sockaddr) ) == SOCKET_ERROR )
    {
        BroLogInfo("..Server socket bind failed.");
        return SOCK_BINDFAILED;
    }
    BroLogInfo("..Server socket bound successfully.");

    // - Put the socket in listening state
    if( listen( nbsocket, 64 ) == SOCKET_ERROR )
    {
        return SOCK_LISTENERR;
    }

    t_string saddr = (addr==NULL) ? (TStr("any address")) : (TStr("address ", addr));
    BroLogInfo("..Server socket listening on port ", port, " for ", saddr);


    return SOCK_SUCCESS;
}

RetCode ServerSocket::setSockOpts()
{
    // - Set the client socket to non-blocking
    t_ulong nonBlocking=1;        // - The socket won't block on recv, accept, etc.
    t_ulong maxReadSize=1500;    // - The socket can read up to 1500B (the MTU in most cases)
    t_ulong keepAlive=1;        // - Tells the socket to send packets that are to be kept longer.
    t_ulong recvBufSize=Gu::getEngineConfig()->getMaxHardwareIncomingBufferSizeBytes();    // - Tells the socket that the recv buf should be 1500 bytes;
    t_ulong sendBufSize=Gu::getEngineConfig()->getMaxHardwareOutgoingBufferSizeBytes();    // - Tells the socket that the recv buf should be 1500 bytes;
    //t_ulong reuseAddr=1;        // - Tells the socket that the address should be able to be reused.
    //t_dword    ipTTL=128;            // - TTL for the ip.
    if ( ioctlsocket( nbsocket, FIONBIO, &nonBlocking ) == SOCKET_ERROR )
        return SOCK_COULDNOTSET;
    
    // - Returns the maximum amount of data that can be read (don't need)
    //if ( ioctlsocket( nbsocket, FIONREAD, &maxReadSize ) == SOCKET_ERROR )
    //    return SOCK_COULDNOTSET;

    //if( setsockopt( nbsocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&keepAlive, sizeof(keepAlive) ) == SOCKET_ERROR )
    //    return SOCK_COULDNOTSET;
    //
    if( setsockopt( nbsocket, SOL_SOCKET, SO_RCVBUF, (char*)&recvBufSize, sizeof(recvBufSize) ) == SOCKET_ERROR )
        return SOCK_COULDNOTSET;
    
    if( setsockopt( nbsocket, SOL_SOCKET, SO_SNDBUF, (char*)&sendBufSize, sizeof(sendBufSize) ) == SOCKET_ERROR )
        return SOCK_COULDNOTSET;

    //if( setsockopt( nbsocket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuseAddr, sizeof(reuseAddr) ) == SOCKET_ERROR )
    //    return SOCK_COULDNOTSET;

    //if( setsockopt( nbsocket, IPPROTO_IP, IP_TTL, (char*)&ipTTL, sizeof(ipTTL) ) == SOCKET_ERROR )
    //    return SOCK_COULDNOTSET;
    //
    // - Initialize the file descriptors
    FD_ZERO(&socketFD);
    FD_SET(nbsocket, &socketFD);
    socketPollVal.tv_sec = 0;
    socketPollVal.tv_usec = 0;

    return SOCK_SUCCESS;
}

// - Try to accept a new connection on this socket
// - Make sure to poll for reading of this socket first.
// - Outputs the new client connection socket that you can send to.
// - If this returns SOCK_NOCONNAVAIL, then the socket has data waiting to be read.
RetCode ServerSocket::acceptConnection( __out_ __in_ ServerClientSocket* newConn )
{
    // - Listen and accept the connection.
    if(listen( nbsocket, SOMAXCONN ) == SOCKET_ERROR)
    {
        return SOCK_LISTENERR;
    }

    newConn->acceptServerConnection(this);

    setState(SOCK_OPEN);

    return newConn->setSockOpts();
}
}//nsgame