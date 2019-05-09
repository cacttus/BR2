
#include "../net/NetServer.h"
#include "../net/ServerSocket.h"
#include "../net/VirtualClient.h"
#include "../game/RoomBase.h"
#include "../base/GlobalUtils.h"
#include "../game/IRoomManager.h"
#include "../net/Packet.h"
#include "../net/NetworkUtils.h"
#include "../net/NetworkManager.h"
#include "../net/NetworkInfo.h"

namespace Game {; 
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
NetServer::NetServer(NetworkWorker* pwn) :
NetworkObject(pwn),
_pMySocket(NULL)
{

}
NetServer::~NetServer()
{
    shutdown();
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//
//    @param noWSAStartup - Does not call WSAStartup, this is useful if you have another application that has already initialized winsock.
//    @param port - The port to listen for connections on, if zero, the server will listen on port 4111 or any other default port.
//
RetCode NetServer::init(NetPort port)
{
    cleanup();
    
    //HEAP:
    _pMySocket = new ServerSocket(_pNetworkManager);

    // - We're gonna init, and also bind the socket.
    return _pMySocket->createSocket(port);    // - Will also bind. 
}
void NetServer::update()
{
    pollForNewClients();

    for(ServerClientSocket* pClient : (_vecClients))
    {
        if(pollClientActivity(pClient) == SOCK_DISCONNECT)
            continue;
        handleClientData(pClient);
    }

    for(t_vecsize iClient=0; iClient<_vecDeadClients.count(); iClient++)
    {
        ServerClientSocket* pClient = _vecDeadClients[iClient];
        closeClient(pClient);
        _vecClients.remove(pClient);
        delete pClient;
    }

  //  _pNetworkManager->getNetworkInfoWorker()->_serverInfo._iNumConnectedClients = _vecClients.count();

    _vecDeadClients.resize(0);

}
// Check for new packets, also read data form the client into working packets.
RetCode NetServer::handleClientData(ServerClientSocket* pClient)
{
    Packet temp;
    RetCode ret;
    t_bool bHasData;

    //Create packets and update buffers
    //TODO: put this method in a loop BUT
    //Beware of buffer lock.  Add a "Max KB/Frame" parameter.
    ret = pClient->updateComm();

    if(ret==SOCK_CONNCLOSE)
    {
        BroLogInfo("Client ", pClient->getClientIp(), " has closed.");
        _vecDeadClients.add(pClient);
    }
    else if(ret!=SOCK_SUCCESS && ret!=SOCK_NODATA)
        //Note - probably we want to kill the client here as well.
        BroLogError(" Socket Error ", NetworkUtils::GetSockErrStr(ret));
    else
    {
        do
        {
            temp.reset();

            // - Receives packets from buffered data.
            //Note: so we copy packets, this prevents the system from allowing rogue packets.
            pClient->getNextPacket(&temp);


            bHasData = temp.getHasData();

            if( temp.getHasData() )
            {
                temp._pClient = pClient;
                if(Gu::GetRoomManager() && Gu::GetRoomManager()->getCurrentRoom())
                    Gu::GetRoomManager()->getCurrentRoom()->clientPacketReceived(pClient, &temp);
                else
                    BroLogError("Network -- Room was not found. 232590");
            }
        }while(bHasData);
    }
    return ret;
}
/**
*   @fn cleanup();
*   @brief Clean up the socket. remove clients and disconnect from winsock.
*/
void NetServer::cleanup()
{
    for( t_memsize i=0; i<_vecClients.size(); ++i )
    {
        closeClient(_vecClients[i]);
        DEL_MEM(_vecClients[i]);
    }
    _vecClients.resize(0);

    _vecDeadClients.resize(0);
    _vecNewClients.resize(0);

    DEL_MEM(_pMySocket);//Socket gets closed here
}

/**
*    @fn pollForNewClients()
*    @details Polls to see if new clients are available.
*    @param newClientList [OUT] Returns a list of new clients.
*    @return SOCK_SUCCESS - Added a new client to the client connection list.
*
*/
RetCode NetServer::pollForNewClients()
{
    RetCode ret;
    ServerClientSocket* pNextClient;    // - The 'temp' client that is new'd for the next connection.
    
    _vecNewClients.resize(0);

    ret = _pMySocket->pollForRead();

    // - Check if a new client is trying to connect.
    while(ret==SOCK_TRUE)
    {
        pNextClient = new ServerClientSocket(_pNetworkManager);

        // - Data can be read
        RetCode s = _pMySocket->acceptConnection( pNextClient );
        
        if( s!=SOCK_SUCCESS )
        {
            if( _vecNewClients.size() )
                return SOCK_SUCCESS;

            return s;    // - Return either an error, or, most likely SOCK_NOCONNAVAIL, meaning do nothing.
        }

        _vecClients.add( pNextClient );
        _vecNewClients.add( pNextClient );

        return s;
    }
    
    // - Send connected messages to room
    for(ServerClientSocket* pClient : _vecNewClients)
    {
        if(Gu::GetRoomManager() && Gu::GetRoomManager()->getCurrentRoom())
        {
            Gu::GetRoomManager()->getCurrentRoom()->clientConnected(pClient);
        }
        else
            BroLogError("Network -- Room was not found.");
                
    }

    return SOCK_SUCCESS;
}

/**
*    @return SOCK_DISCONNECT            The client was disconnected.
*    @return SOCK_SUCCESS            Success.
*    @return SOCK_ARRAYOUTOFBOUNDS    The index specified was out of bounds.
*/
RetCode NetServer::pollClientActivity( ServerClientSocket* pClient )
{
     AssertOrThrow2(pClient!=NULL);

    if( pClient->pollForActive() == SOCK_DISCONNECT )
    {
        _vecDeadClients.add(pClient);
        return SOCK_DISCONNECT;
    }

    return SOCK_SUCCESS;
}
/**
*    @fn closeClient
*    @details Close the client socket.
*    @return SOCK_SUCCESS
*    @return SOCK_ARRAYOUTOFBOUNDS
*    @return SOCK_ALREADYCLOSED
*/
void NetServer::closeClient( ServerClientSocket* pClient )
{
     AssertOrThrow2(pClient!=NULL);
    RetCode ret = pClient->closeSocket();
}
void NetServer::shutdown()
{
    cleanup();
}
/**
*    ** success
*    @return SOCK_NODATA - There is no data to be read in the buffer.
*    @return SOCK_CANNOTBEREAD - The socket could not be read from at this time.
*    @return SOCK_SUCCESS - XML Document was successfully set.
*    ** failure
*    @return SOCK_ARRAYOUTOFBOUNDS    - The array is out of bounds.
*    @return Other socket errors
*/
//This is no longer needed (right now)
//RetCode NetServer::pollClientForData( DataBuffer& pdb, 
//                                           VirtualClient* __in_ client, 
//                                           t_int* out_size )
//{ 
//    // - Check for new data from clients.
//    // - I assume you call pollForNewConn() before this, so mySocket.pollForRead() is not necessary.
//    t_int retsize=0;
//   
//    AssertOrThrow2(client!=NULL);
//
//    if(client->pollForRead()==SOCK_FALSE)
//        return SOCK_NODATA;
//
//    RetCode ret = client->receiveData((char*)pdb.ptr(), pdb.count(), out_size);
//    
//    if(ret!=SOCK_SUCCESS)
//        return ret;
//
//    return SOCK_SUCCESS;
//    
//}
/**
*   @fn pollForData
*   @brief Poll a client.  Optinoally Put the server in waiting state.
*   @details 
*   waitTimeInMs if -1 we return immediately if there was no data.
*/
//RetCode NetServer::waitClientForData(VirtualClient __in_ *client, DataBuffer& pdb, int waitTimeInMs, int* out_size)
//{
//    DWORD d;
//    
//    if(waitTimeInMs!=-1)
//        d = GetTickCount();
//
//    //TODO: make a timer and run this in the main engine loop - or
//    // on a separate thread.
//    while(true)
//    {
//        RetCode s = this->pollClientForData(pdb, client, out_size);
//        
//        if(s!=SOCK_NOCONNAVAIL)
//            return s;
//        
//        if( (waitTimeInMs!=-1) && (int)(GetTickCount() - d) > waitTimeInMs ) 
//            return SOCK_TIMEOUT;
//    }
//    return SOCK_SUCCESS;
//}



}