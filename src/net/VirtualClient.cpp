#include "../net/VirtualClient.h"
#include "../net/NetworkManager.h"
#include "../net/NetworkUtils.h"
#include "../dev/DebugHelper.h"
#include "../base/GlobalUtils.h"
#include "../scene/EngineSystemParams.h"
#include "../net/PacketMakerBase.h"

namespace Game{ 
ServerClientSocket::ServerClientSocket(NetworkWorker* pwn) :
    NonBlockingSocket(pwn)
    , _iClientConnectionTimeoutInSeconds( NET_SEVER_VIRTUAL_CLIENT_DEFAULT_TIMEOUT_SECONDS )
{
    _iClientConnectionTimeoutInSeconds = Gu::getEngineConfig()->getNetClientTimeoutInSeconds();
}
ServerClientSocket::~ServerClientSocket()
{
}
//////////////////////////////////////////////////////////////////////////
RetCode ServerClientSocket::acceptServerConnection(NonBlockingSocket* pServerSocket)
{
    t_ulong ret;
    t_int sockaddrSize = sizeof(SOCKADDR_IN);

    nbsocket = accept( pServerSocket->getWsaSocket(), (sockaddr*)&socketAddr, &sockaddrSize );

    ret = (t_ulong)(nbsocket);
    
    if( ret == SOCKET_ERROR )
    {
        int err = WSAGetLastError();
        if( err==WSAEWOULDBLOCK )
        {
            setState(SOCK_LISTENING);
            return SOCK_NOCONNAVAIL;    // - No connections are available. // Will return if data is to be read.
        }
        else if(err!=0) 
        {
            //_pNetworkManager->addError(err);
            BroLogError(" WSA error trying to accept server connection.. err ",err);
            DebugHelper::debugBreak();
        }
        return SOCK_ACCEPTERR;
    }
    
    SOCKADDR_IN client_info = {0};
//int addrsize = sizeof(client_info);

// get it during the accept call
//SOCKET client_sock = accept(serv, (struct sockaddr*)&client_info, &addrsize);

// or get it from the socket itself at any time
    int namelen = sizeof(client_info);
    getpeername(nbsocket, (sockaddr*)&client_info, &namelen);
   _strClientIp = TStr(inet_ntoa(client_info.sin_addr));

    BroLogInfo("Server accepted new client ", _strClientIp);

    return SOCK_SUCCESS;
}
/**
*    @fn pollClientForActive()  [server]
*    @details Polls the client for activity, disconnecting this client if he is not active.
*
*/
RetCode ServerClientSocket::pollForActive()
{
    t_dword timetout = NetworkUtils::stoms(_iClientConnectionTimeoutInSeconds);
    t_dword lastMsg = OperatingSystem::getTickCount64() - lastRecv;

    if(lastMsg>timetout)    // - 5 min timeout.
    {
        closesocket(nbsocket);
        return SOCK_DISCONNECT;    // - Inform the caller that he has to handle this client as a disconnection.
    }

    return SOCK_SUCCESS;
}
// @fn setSockOpts()
// @brief Set the socket options for the client socket located on the server.
//
RetCode ServerClientSocket::setSockOpts(){
    // - Set the client socket to non-blocking
    t_ulong nonBlocking=1;        // - The socket won't block on recv, accept, etc.
    t_ulong maxReadSize=1500;    // - The socket can read up to 1500B (the MTU in most cases)
    t_ulong keepAlive=1;        // - Tells the socket to send packets that are to be kept longer.
    t_ulong recvBufSize=1500;    // - Tells the socket that the recv buf should be 1500 bytes;
    t_ulong sendBufSize=1500;    // - Tells the socket that the recv buf should be 1500 bytes;
    //t_ulong reuseAddr=1;        // - Tells the socket that the address should be able to be reused.
    t_ulong    linger=0;            // - Don't linger on close.
    t_dword    ipTTL=128;            // - TTL for the ip.

    if ( ioctlsocket( nbsocket, FIONBIO, &nonBlocking ) == SOCKET_ERROR )
        return SOCK_COULDNOTSET;
    
    //see server setSOckOpts()
    //if ( ioctlsocket( nbsocket, FIONREAD, &maxReadSize ) == SOCKET_ERROR )
    //    return SOCK_COULDNOTSET;

    //if( setsockopt( nbsocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&keepAlive, sizeof(keepAlive) ) == SOCKET_ERROR )
    //    return SOCK_COULDNOTSET;
    //
    if( setsockopt( nbsocket, SOL_SOCKET, SO_RCVBUF, (char*)&recvBufSize, sizeof(recvBufSize) ) == SOCKET_ERROR )
        return SOCK_COULDNOTSET;
    
    if( setsockopt( nbsocket, SOL_SOCKET, SO_SNDBUF, (char*)&sendBufSize, sizeof(sendBufSize) ) == SOCKET_ERROR )
        return SOCK_COULDNOTSET;

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


}//ns game