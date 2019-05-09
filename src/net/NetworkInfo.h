/**
*
*    @file NetworkInfo.h
*    @date July 12, 2016
*    @author Derek Page
*
*    © 2016 
*
*
*/
#pragma once
#ifndef __NETWORKINFO_14683699464004725019_H__
#define __NETWORKINFO_14683699464004725019_H__

#include "../net/NetHeader.h"

namespace Game {

class SocketInfo : public ThreadPersistentSyncObject {
public:
    t_bool _bConnected;
    t_uint64 _nPacketsReceived;
    t_uint64 _nPacketsSent;
    RetCode _eLastError;
    
    SocketInfo();
};
class ClientInfo : public SocketInfo {
public:
    t_string _strServerIp;

    ClientInfo();
};
class ServerInfo : public SocketInfo {
public:
    t_uint64 _iNumConnectedClients;

    ServerInfo();
};
class NetworkInfo : public ThreadPersistentSyncObject {
public:
    //Do not write to this class
    ClientInfo _clientInfo;
    ServerInfo _serverInfo;
    t_uint64 _iNetworkTicks;

    NetworkInfo();
};

}//ns Game



#endif
