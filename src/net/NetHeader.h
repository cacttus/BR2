/**
*
*    @file NetworkHeader.h
*    @date June 20, 2015
*    @author Derek Page
*
*    © 2015 
*
*
*/
#pragma once
#ifndef __NETWORKHEADER_93691092617651749622729_H__
#define __NETWORKHEADER_93691092617651749622729_H__

//**KEEP here for reference
//#include "mysql++.h"
//#pragma comment( lib, "libmysql.lib" )
//#pragma comment( lib, "mysqlclient.lib" )


#include "../base/BaseHeader.h"

#include "../base/DynamicBuffer.h"

namespace Game {;

#define NET_SEVER_VIRTUAL_CLIENT_DEFAULT_TIMEOUT_SECONDS (60*5)
    
typedef enum {
     SOCK_CLOSED        // - The socket has been closed
    ,SOCK_CREATED           // - The initial state of a socket when it is instantiated.
    ,SOCK_OPEN              // - the socket is bound and accepted, but is not receiving data.
    ,SOCK_LISTENING     // - The socket is listening [server only].
    ,SOCK_CONNECTING // - The socket is connecting to the server [client only]/
    ,SOCK_CONNECTED     // - The socket is connected to the server [client only].
    ,SOCK_RECEIVING         // - The socket is receiving data [server only].
}SocketState;

// - @enum NetworkState
// - @brief State of the network.
typedef enum {
    NETWORK_UNINITIALIZED,
    NETWORK_RUNNING,
    NETWORK_SHUTDOWN,
}NetworkState;

typedef enum {
    NETWORK_OBJECT_CLIENT,
    NETWORK_OBJECT_SERVER,
}NetworkObjectType;

typedef enum {
    HOST_MODE_NONE,
    HOST_MODE_CLIENT,
    HOST_MODE_SERVER
}NetClientHostMode;

typedef enum {
    NETWORK_STATE_CONNECTED,
    NETWORK_STATE_NOT_CONNECTED,
}NetworkConnectionState;

typedef enum{
    NetStartServer
}NetMessages;

///We won't need this later. Doesn't make sense to hard code enums at the engine level.
//typedef enum {
//    Unknown
//    //Note: Make sure to modify NetworkManager::initPakcetTypes() if you add new enums
//    //And also update the TexView application's NetworkSettings::CreateDefaultDictionary for Binary Types
//    ,StringMessage
//    ,LongMessage
//    ,TexData
//}NetPackType;

typedef uint64_t PacketType;

//////////////////////////////////////////////////////////////////////////
typedef uint16_t NetPort;

//////////////////////////////////////////////////////////////////////////

class NetClient;
class ClientSocket;
class Database;
class DBAdmin;
class DBHeader;
class DBManager;
class DBMySql;
class NetworkManager;
class NetworkObject;
class NetworkUI;
class NonBlockingSocket;
class Packet;
class QueryResult;
class  NetServer;
class ServerSocket;
class sqlTest;
class ServerClientSocket;
class PacketMakerBase;
class WorkingPacket;
class Serializable;
class ClientInfo;
class ServerInfo;
class NetworkInfo;
class NetworkWorker;





}//ns game



#endif
