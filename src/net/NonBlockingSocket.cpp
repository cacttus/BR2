#include "../base/TypeConv.h"
#include "../base/DynamicBuffer.h"
#include "../net/NonBlockingSocket.h"
#include "../net/NetworkUtils.h"
#include "../net/PacketMakerBase.h"
#include "../net/Serializable.h"
#include "../mem/StreamBuffer.h"
#include "../base/GlobalUtils.h"
#include "../scene/EngineSystemParams.h"

namespace Game {;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
NonBlockingSocket::NonBlockingSocket(NetworkWorker* pwn) :
    _pNetworkManager(pwn)
{
    lastSend = lastRecv = OperatingSystem::getTickCount64();
    setState(SOCK_CREATED);
    _pPacketMaker = new PacketMakerBase(this);
    
    
    t_ulong recvBufSize = Gu::getEngineConfig()->getMaxHardwareIncomingBufferSizeBytes();
    t_ulong sendBufSize = Gu::getEngineConfig()->getMaxHardwareOutgoingBufferSizeBytes();
    t_ulong maxSize = max(recvBufSize, sendBufSize);
    _scratchBuffer._alloca(maxSize);
}
NonBlockingSocket::~NonBlockingSocket()
{
    if (nbsocket != NULL)
    {
        closeSocket();
        nbsocket = NULL;
    }

    DEL_MEM(_pPacketMaker);

    _scratchBuffer.dealloc();
}
//////////////////////////////////////////////////////////////////////////
// - Get a string rep of the IP
t_string NonBlockingSocket::getIPStr()
{
    return t_string(inet_ntoa(socketAddr.sin_addr));
}
// Get a string rep of the port.
t_string NonBlockingSocket::getPortStr()
{
    return  Game::intToStr((t_int)ntohs(socketAddr.sin_port));
}
// - Return true if this socket can be read from. http://msdn.microsoft.com/en-us/library/ms740141(VS.85).aspx
// - In a server this might mean that a client has connected.
RetCode NonBlockingSocket::pollForRead()
{
    // - Make sure the FD is actually in the set. (may be a performance issue).
    if (!FD_ISSET(nbsocket, &socketFD))
        FD_SET(nbsocket, &socketFD);

    // - Check if the socket can be read from.
    fd_set read = socketFD;
    if (select(NULL, &read, NULL, NULL, &socketPollVal) == SOCKET_ERROR)
        return SOCK_POLLFAIL;

    if (read.fd_count)
        return SOCK_TRUE;

    return SOCK_FALSE;
}

// - Return true if this socket can be written to (if you can call sendData). http://msdn.microsoft.com/en-us/library/ms740141(VS.85).aspx
// - This function also re-initializes a client socket when connecting to a server.
RetCode NonBlockingSocket::pollForWrite()
{
    // - Make sure the FD is actually in the set. (may be a performance issue).
    if (!FD_ISSET(nbsocket, &socketFD))
        FD_SET(nbsocket, &socketFD);

    // - Check if the socket can be written to.
    fd_set write = socketFD;
    if (select(NULL, NULL, &write, NULL, &socketPollVal) == SOCKET_ERROR)
        return SOCK_POLLFAIL;

    if (write.fd_count)
        return SOCK_TRUE;

    return SOCK_FALSE;
}
RetCode NonBlockingSocket::receiveData(StreamBuffer<char>* pBuf)
{
    RetCode sr;
    t_int64 readSiz;

    sr = pollForRead();

    if(sr!= SOCK_TRUE)
        return sr;

    if(pBuf->getFreeCount()==0)
        return SOCK_BUFFER_FULL;

    AssertOrThrow2(pBuf->getBuffer()->byteSize() <= _scratchBuffer.byteSize());

    sr = receiveData(_scratchBuffer.ptr(), pBuf->getFreeCount(), &readSiz);
    if(sr != SOCK_SUCCESS)
        return sr;
     
    //Write to the circular buffer.
    sr = pBuf->write(_scratchBuffer.ptr(), readSiz);

    return sr;
}
// - Receive data from this socket.
// - Outputs data into the buffer you specify.
// * Make sure to poll for reading before calling this.
// - retSize - The number of bytes read.
// - @return SOCK_CONNCLOSE    The connection has been closed by the client.
// - @return SOCK_NODATA    There was no data on this socket to be read.
// - @return SOCK_SUCCESS    Success in reading data.
RetCode NonBlockingSocket::receiveData(char* dataBuf, t_int64 dataBufLen, __out_ t_int64* readSize)
{
    *readSize = recv(nbsocket, dataBuf, (int)dataBufLen, 0);

    if (*readSize == SOCKET_ERROR)
    {
        DWORD err = GetLastError();
        if (err == WSAEWOULDBLOCK)
            return SOCK_NODATA;
        else if (err == WSAECONNRESET)
            return SOCK_CONNCLOSE;
        else if (err == WSAENOBUFS)//no buffer space avail
            return SOCK_NODATA;
        else
            BroLogError("Winsock error during recv: ", err);
        return SOCK_RECVERR;
    }

    if (*readSize == 0)
        return SOCK_CONNCLOSE;

    lastRecv = OperatingSystem::getTickCount64();

    return SOCK_SUCCESS;
}
RetCode NonBlockingSocket::sendData(StreamBuffer<char>* pBuf)
{
    RetCode sr;
    t_int64 writeSiz;

    sr = pollForWrite();

    if (sr != SOCK_TRUE)
        return sr;

    if (pBuf->getUsedCount() == 0) //UsedCount = Read
        return SOCK_BUFFER_FULL;

    AssertOrThrow2(pBuf->getBuffer()->byteSize() <= _scratchBuffer.byteSize());
    
    //Read from circle buf & upodate read head
    pBuf->read(_scratchBuffer.ptr(), writeSiz);

    //send
    sr = sendData(_scratchBuffer.ptr(), pBuf->getUsedCount(), &writeSiz);

    return sr;
}
// - Send data to this socket.
// - Make sure to poll for writing before calling this.
RetCode NonBlockingSocket::sendData(const char* dataBuf, t_int64 dataBufLen, t_int64* outBytes)
{
    t_int sentBytes = 0;
    sentBytes = send(nbsocket, dataBuf, (int)dataBufLen, 0);

    if (sentBytes == SOCKET_ERROR)
    {
        int err = GetLastError();
        if (err == WSAEWOULDBLOCK)
            return SOCK_NODATA;
        else if (err == WSAENOBUFS)//no buffer space avail
            return SOCK_NODATA;
        else if (err == WSAECONNRESET)
            return SOCK_CONNCLOSE;
        else
            BroLogError("Winsock error during send: ", err);
        return SOCK_SENDERR;
    }

    if (outBytes != NULL)
        *outBytes = sentBytes;

    lastSend = OperatingSystem::getTickCount64();

    return SOCK_SUCCESS;
}
// - Close the socket an nullify all values
RetCode NonBlockingSocket::closeSocket()
{
    if (nbsocket == NULL)
        return SOCK_ALREADYCLOSED;

    closesocket(nbsocket);
    nbsocket = NULL;
    memset(&socketAddr, 0, sizeof(SOCKADDR_IN));
    FD_ZERO(&socketFD);
    return SOCK_SUCCESS;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// - Assert the socket state.
void NonBlockingSocket::assertState(SocketState state)
{
    AssertOrThrow2(getState() == state);
}
// - Return the state of the socket
SocketState NonBlockingSocket::getState()
{
    if (stateList.size() == 0)
        BroThrowException(" [socket] The state list size was empty, there must be at least one state");

    //Warning due to changes in vectorlist this may wig out
    return stateList.at(stateList.size() - 1);
}
void NonBlockingSocket::setState(SocketState state)
{
    stateList.push_back(state);
}
// - Return the state list.
VectorEx<SocketState>& NonBlockingSocket::getStateList()
{
    return stateList;
}
void NonBlockingSocket::printStateList()
{
    for (t_memsize i = 0; i < stateList.size(); ++i)
    {
        SocketState s = stateList[i];
        std::cout << NetworkUtils::GetSockStateName(s).c_str() << std::endl;
    }
}
//RetCode NonBlockingSocket::sendPacket(Packet* __in_ pPack)
//{
//    AssertOrThrow2(pPack!=NULL);
//    BroThrowNotImplementedException();
//    ///IDK - impalment
//    return RetCode::SOCK_FALSE;
//}
void NonBlockingSocket::getNextPacket(Packet* __out_ pPack)
{
    AssertOrThrow2(pPack != NULL);
    _pPacketMaker->getNextPacket(pPack);
}
//RetCode NonBlockingSocket::sendData(NetPackType __in_ ePackType, const t_string& __in_ strData)
//{
//    return _pPacketMaker->sendData(ePackType, strData);
//}
RetCode NonBlockingSocket::updateComm()
{
    return _pPacketMaker->updateSocketComm();
}
//RetCode NonBlockingSocket::sendData(PacketType __in_ iPackType, Serializable* __in_ pObj)
//{
//    t_string str;
//
//    DynamicBuffer<char> buf(1024);
//    pObj->serialize(&buf);
//    str = buf.toString();
//
//    return _pPacketMaker->sendData(iPackType, str);
//}
RetCode NonBlockingSocket::sendData(PacketType __in_ iPackType, const t_string& str)
{
    return _pPacketMaker->sendData(iPackType, str);
}





























}//ns game