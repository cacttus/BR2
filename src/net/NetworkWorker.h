/**
*
*    @file NetworkWorker.h
*    @date July 12, 2016
*    @author Derek Page
*
*    © 2016 
*
*
*/
#pragma once
#ifndef __NETWORKWORKER_14683827203046131305_H__
#define __NETWORKWORKER_14683827203046131305_H__

#include "../net/NetHeader.h"

namespace Game {
class NetworkWorker : public VirtualMemory {
private:
    std::mutex _mutex;
    std::mutex& getMutex() { return _mutex;  }
    WSADATA _wsaData;
    NetworkState _eNetworkState;
    std::vector<t_string> _vecErrors;
    NetClient* _pClient = NULL;//you can be both a client and server (shludS)
    NetServer* _pServer = NULL;//you can be both a client and server
    
    RetCode startup();
    RetCode shutdown();

    void addError(RetCode e);
    void restartNetworkSystem();
    NetworkState getNetworkState() { return _eNetworkState; }
    static void checkErrorsAsync(int line, char* file, t_bool bThrow = false);

    void stopServerOrClient();

public:
    void init();        // Thread functions called in the main thread routine for the audio manager.
    void updateWorker();
   // void processMessage(ThreadMessage2* ptm);
    void stop();

    NetworkWorker();
    virtual ~NetworkWorker() override;
    static t_string getPacketPrependTag();
    NetClient* getClient() { return _pClient; }
    NetServer* getServer() { return _pServer; }
    RetCode startServer();// - start a server (ports are defined in engine.dat);
    RetCode connectToServer(t_string serverIp, int32_t iClientPort, int32_t iServerPort);// - start a client (ports are defined in engine.dat)
    RetCode sendClientPacket(PacketType iPackType, t_string __in_ str);
    t_bool getIsClientConnected();
};

}//ns Game



#endif
