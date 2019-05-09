#include "../net/ClientSocket.h"
#include "../net/ServerSocket.h"
#include "../base/Logger.h"
#include "../base/EngineConfig.h"

namespace Game {
;

/**
*    @fn initConnect
*    @details Initializes a connect from the server.  Subsequent calls should be made to pollConnect, if
*    the return value of this is not SOCK_CONNPENDING.
*/
RetCode ClientSocket::newSocket(NetPort srcport, NetPort destport, const char* addr)
{
    BroLogInfo("Initializing client connection on port ", srcport, " to server port ", destport, " at address ", addr);

    t_ulong laddr = inet_addr(addr);
    return newSocket(srcport, destport, laddr);
}
/**
*    @fn initConnect
*    @details Initializes a connect from the server.  Subsequent calls should be made to pollConnect, if
*    the return value of this is not SOCK_CONNPENDING.
*/
RetCode ClientSocket::newSocket(NetPort srcport, NetPort destport, t_ulong addr)
{
    nbsocket = socket(AF_INET, SOCK_STREAM, 0);

    // - Set the address structure
    socketAddr.sin_port = htons(srcport);
    socketAddr.sin_family = AF_INET;
    socketAddr.sin_addr.S_un.S_addr = INADDR_ANY;

    // - If we succeed in binding, set socket toptions.
    if(bind(nbsocket, (sockaddr*)&socketAddr, sizeof(sockaddr)) == SOCKET_ERROR)
    {
        BroLogError(" Socket bind failed ");
        return SOCK_BINDFAILED;
    }

    if(setSockOpts() != SOCK_SUCCESS)
        return SOCK_COULDNOTSET;

    // - Set the server address parameters.
    serverAddr.sin_port = htons(destport);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.S_un.S_addr = addr;

    // - Now try to connect to the server.
    return tryConnect();
}
/**
*    @fn pollConnect()
*    @details Polls a connection with a server, only useable if a previous call has been made to initConnect().
*   @return SOCK_CONNERR - The socket was not in the connecting state when you called this.
*   @return SOCK_SUCCESS - The socket has connected successfully.
*   @return SOCK_CONNPENDING - The socket is still waiting for a connection.
*/
RetCode ClientSocket::pollConnect()
{
    //From wsa api
    //Due to ambiguities in version 1.1 of the Windows Sockets specification, error codes
    //returned from connect while a connection is already pending may vary among implementations.
    //    As a result, it is not recommended that applications use multiple calls to connect to 
    //    detect connection completion.
    if(getState() != SOCK_CONNECTING)
        return SOCK_CONNERR;
    if(pollForWrite() == SOCK_TRUE)
    {
        setState(SOCK_CONNECTED);
        return SOCK_SUCCESS;
    }
    return SOCK_CONNPENDING;
}


RetCode ClientSocket::setSockOpts()
{
    // - Set the client socket to non-blocking
    t_ulong nonBlocking = 1;        // - The socket won't block on recv, accept, etc.
    t_ulong maxReadSize = 1500;    // - The socket can read up to 1500B (the MTU in most cases)
    t_ulong keepAlive = 1;        // - Tells the socket to send packets that are to be kept longer.
    t_ulong recvBufSize = Gu::getEngineConfig()->getMaxHardwareIncomingBufferSizeBytes();    // - Tells the socket that the recv buf should be 1500 bytes;
    t_ulong sendBufSize = Gu::getEngineConfig()->getMaxHardwareOutgoingBufferSizeBytes();    // - Tells the socket that the recv buf should be 1500 bytes;
    //t_ulong reuseAddr=1;        // - Tells the socket that the address should be able to be reused.
    t_ulong    linger = 0;            // - Don't linger on close.
    DWORD    ipTTL = 128;            // - TTL for the ip.

    if (ioctlsocket(nbsocket, FIONBIO, &nonBlocking) == SOCKET_ERROR) {
        return SOCK_COULDNOTSET;
    }

    //see server setSOckOpts()
    //if ( ioctlsocket( nbsocket, FIONREAD, &maxReadSize ) == SOCKET_ERROR )
    //    return SOCK_COULDNOTSET;

    //if( setsockopt( nbsocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&keepAlive, sizeof(keepAlive) ) == SOCKET_ERROR )
    //    return SOCK_COULDNOTSET;
    //
    if (setsockopt(nbsocket, SOL_SOCKET, SO_RCVBUF, (char*)&recvBufSize, sizeof(recvBufSize)) == SOCKET_ERROR) {
        return SOCK_COULDNOTSET;
    }

    if (setsockopt(nbsocket, SOL_SOCKET, SO_SNDBUF, (char*)&sendBufSize, sizeof(sendBufSize)) == SOCKET_ERROR) {
        return SOCK_COULDNOTSET;
    }

    //if( setsockopt( nbsocket, IPPROTO_IP, IP_TTL, (char*)&ipTTL, sizeof(ipTTL) ) == SOCKET_ERROR )
    //    return SOCK_COULDNOTSET;
    //
    //if( setsockopt( nbsocket, IPPROTO_IP, IP_TTL, (char*)&linger, sizeof(linger) ) == SOCKET_ERROR )
    //    return SOCK_COULDNOTSET;

    // - Initialize the file descriptors
    FD_ZERO(&socketFD);
    FD_SET(nbsocket, &socketFD);
    socketPollVal.tv_sec = 0;
    socketPollVal.tv_usec = 0;

    return SOCK_SUCCESS;
}
/**
*    @fn tryConnect()
*    @brief Tries to connect to the server asynchronously.
*    @remarks This will not return successfully the first time.  The socket must be polled for write operations henceforth by
*   calling pollForWrite() and then calling tryConnect().
*   @return SOCK_CONNECTING - The socket is still trying to connect to the server.
*   @return SOCK_CONNERR - There was en error when trying to connect.
*   @return SOCK_SUCCESS - The client is now connected to the server.
*/
RetCode ClientSocket::tryConnect()
{
    int32_t nMaxErrors = 1000;
    while(nMaxErrors-- > 0)
    {
        if(connect(nbsocket, (sockaddr*)&serverAddr, sizeof(sockaddr)) == SOCKET_ERROR)
        {
            int err = WSAGetLastError();
            if(err == WSAEWOULDBLOCK)
            {
                // - good, set the flag and be done with it.
                setState(SOCK_CONNECTING);
                return SOCK_CONNPENDING;
            }
            else if(err==WSAEADDRINUSE)
            {
                setState(SOCK_CLOSED);
                return SOCK_ERR_ADDRINUSE;
            }
            else
            {
                BroLogError(" Client had problem connecting, WSA error = ", err);
                setState(SOCK_CLOSED);
                return SOCK_CONNERR;
            }
        }
    }

    BroLogInfo(" [NET] Client Connected..");

    setState(SOCK_CONNECTED);
    return SOCK_SUCCESS;
}







































}//ns game