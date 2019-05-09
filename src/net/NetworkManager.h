/**
*
*    @file NetworkManager.h
*    @date June 20, 2015
*    @author Derek Page
*
*    © 2015 
*/
#pragma once
#ifndef __NETWORKMANAGER_920985422704380428835_H__
#define __NETWORKMANAGER_920985422704380428835_H__

#include "../net/NetHeader.h"

namespace Game {;
/**
*    @class NetworkManager
*    @brief Manages network
*/
class NetworkManager : public VirtualMemory {
private:
    std::shared_ptr<NetworkWorker> _pWorker;

public:
    NetworkManager();
    virtual ~NetworkManager() override;

    void startAsync(); //Must call this before all others.
    void updateMain();
    
    void connectToServerAsync(t_string serverIp);// - start a client (ports are defined in engine.dat)
    void startServerAsync();// - start a server (ports are defined in engine.dat);

    RetCode sendClientPacket(PacketType iPackType, t_string __in_ str);

    //SyncData<NetworkInfo>* getSyncData();

    //NetworkInfo* getNetworkInfoMain();
    //NetworkInfo* getNetworkInfoWorker();

    t_bool getClientIsConnected();
};

}//ns game



#endif
