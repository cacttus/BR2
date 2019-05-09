#include "../base/GlobalUtils.h"
#include "../net/NetClient.h"
#include "../net/ClientSocket.h"
#include "../net/Packet.h"
#include "../net/NetworkUtils.h"
#include "../game/IRoomManager.h"
#include "../game/RoomBase.h"
#include "../net/NetworkInfo.h"
#include "../net/NetworkManager.h"

namespace Game {; 
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
NetClient::NetClient(NetworkWorker* pwn) : 
 NetworkObject(pwn),
_pClientSocket(NULL)
{
    _strServerIp = StringUtil::empty();
}
NetClient::~NetClient()
{
    shutdown();
}

//////////////////////////////////////////////////////////////////////////
void NetClient::update()
{
    Packet temp;
    RetCode ret;
    t_bool bHasData;

    //Update the socket communication state.
    ret = _pClientSocket->updateComm();

    if (ret == SOCK_CONNCLOSE)
    {
        BroLogInfo("Client: Client has closed.");
       // NetworkWorker->getNetworkInfoWorker()->_clientInfo._eLastError = ret;
        _strServerIp = StringUtil::empty();
    }
    else if (ret != SOCK_SUCCESS && ret != SOCK_NODATA)
    {
        //Note - probably we want to kill the client here as well.
        BroLogError("Client: Socket Error ", NetworkUtils::GetSockErrStr(ret));
        //_pNetworkManager->getNetworkInfoWorker()->_clientInfo._eLastError = ret;
    }
    else
    {
        do
        {
            temp.reset();

            // - Receives packets from buffered data.
            _pClientSocket->getNextPacket(&temp);

            bHasData = temp.getHasData();

            if (temp.getHasData())
            {
              //  _pNetworkManager->getNetworkInfoWorker()->_clientInfo._nPacketsReceived++;

                if (Gu::GetRoomManager() && Gu::GetRoomManager()->getCurrentRoom())
                {
                    Gu::GetRoomManager()->getCurrentRoom()->serverPacketReceived(&temp);
                }
                else
                {
                    BroLogError("Network -- Room was not found. 232590");
                }
            }
        } while(bHasData);
    }

 //   return ret;
}

/**
*    @fn connectToServer()
*    @brief Function used to establish a connection to a server socket on the other end.
*    @remarks Call this until you get a SOCK_SUCCESS return code.
*    @return SOCK_TIMEOUT if the connection timed out.
*    @return SOCK_SUCCESS if the connection was established.
*    @return A @c RetCode code.
*/
RetCode NetClient::connectToServer(NetPort srcPort, NetPort dstPort, t_string& dstIP, t_int64 waitTimeInMs )
{
    RetCode s;
    t_int64 d = OperatingSystem::getTickCount64();

    DEL_MEM(_pClientSocket);

    _pClientSocket = new ClientSocket(_pNetworkManager);

    checkIfGood(SOCK_CREATED);

    s = _pClientSocket->newSocket( srcPort, dstPort, dstIP.c_str() );
    
    if(s!=SOCK_SUCCESS && s!=SOCK_CONNPENDING)
        return s;

    while( true ) 
    {
        if( _pClientSocket->getState() == SOCK_CONNECTING ) 
        {
            s = _pClientSocket->pollConnect();//changed!!!!! pollconnect
        }

        if( s==SOCK_CONNPENDING ) 
        {
            if( OperatingSystem::getTickCount64() - d > waitTimeInMs ) 
            {
                return SOCK_TIMEOUT;
            }
        }
        else 
        {
            _strServerIp = dstIP;
            // return either success or an error
            return s;
        }

    }
}
/**
*   @fn recvData()
*   @brief Receive data from the server.
*/
//RetCode NetClient::recvDataRaw( Game::IOBuffer& __out_ b, t_int64 waitTimeInMs )
//{
//    if(getSocket()->getState()==SocketState::SOCK_CONNECTING)
//        return RetCode::SOCK_SUCCESS;
//
//    checkIfGood(SOCK_CONNECTED);
//
//    RetCode sr;
//    t_int64 d = OperatingSystem::getTickCount64();
//
//
//    //TODO: PacketMaker
//    BroThrowNotImplementedException();
//
//    t_int64 size;
//    while( true ) 
//    {
//        sr = getSocket()->pollForRead();
//
//        if( sr==SOCK_TRUE ) 
//        {
//            sr = getSocket()->receiveData( (t_char*)b.getBuffer()->ptr(), (t_int64)b.getBuffer()->byteSize(), &size );
//        }
//        else if(sr!=SOCK_FALSE) 
//        {
//            return sr;//poll fail
//        }
//        
//        if( sr==SOCK_FALSE ) 
//        {
//            if( OperatingSystem::getTickCount64() - d > waitTimeInMs ) 
//                return SOCK_TIMEOUT;
//        }
//        else {
//            // return either success or an error
//            return sr;
//        }
//
//    }
//    return SOCK_SUCCESS;
//}
//RetCode NetClient::sendPacket(PacketType __in_ iPackType, Serializable* pObj)
//{
//    return getSocket()->sendData(iPackType, pObj);
//}
RetCode NetClient::sendPacket(PacketType iPackType, t_string& __in_ str)
{
    return getSocket()->sendData(iPackType, str);
}
/**
*    @fn sendData()
*    @brief Send data.
*
*/
//RetCode NetClient::sendDataRaw( Game::IOBuffer& __out_ b, t_dword waitTimeInMs )
//{
//    checkIfGood(SOCK_CONNECTED);
//
//    RetCode s;
//    t_dword d = OperatingSystem::getTickCount64();
//
//    t_int64 out_bytes;
//    while( true ) 
//    {
//        s=this->getSocket()->pollForWrite();
//        if( s==SOCK_TRUE ) {
//            s=this->getSocket()->sendData((t_char*)b.getBuffer()->ptr(), b.getBuffer()->byteSize(), &out_bytes);
//        }
//        else if(s!=SOCK_FALSE) 
//            return s;//poll fail
//
//        if( s==SOCK_FALSE ) {
//            if( OperatingSystem::getTickCount64() - d > waitTimeInMs ) 
//                return SOCK_TIMEOUT;
//        }
//        else {
//            // return either success or an error
//            return s;
//        }
//    }
//    return SOCK_SUCCESS;
//}
/**
*    @fn getSocket() 
*    @brief Returns the socket for the client.
*/
NonBlockingSocket* NetClient::getSocket() 
{ 
    return _pClientSocket; 
}
void NetClient::shutdown()
{
    _strServerIp = StringUtil::empty();
    DEL_MEM(_pClientSocket);
}


}//ns game