#include "../net/NetworkManager.h"
#include "../net/NetServer.h"
#include "../net/NetClient.h"
#include "../net/NetworkUtils.h"
//#include "../base/Crc32.h"
//#include "../scene/EngineSystemParams.h"
//#include "../base/GlobalUtils.h"
//#include "../base/Thread2.h"
//#include "../base/ThreadManager2.h"
#include "../net/NetworkInfo.h"
//#include "../base/SyncData.h"
#include "../net/NetworkWorker.h"

namespace Game {;
NetworkManager::NetworkManager() : 
    _pWorker(NULL)
{
   // _pSyncData = new SyncData<NetworkInfo>();
}
NetworkManager::~NetworkManager()
{
    Thread2* th = Gu::GetThreadManager()->getThread(BroThread::NetworkThread);
    th->stop();

    DEL_MEM(_pWorker);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// NON-MUTEX METHODS


void NetworkManager::updateMain()
{
   // _pSyncData->updateClientSync();
}



void NetworkManager::startAsync()
{
    _pWorker = std::make_shared<NetworkWorker>();
    std::async([&]() {
        _pWorker->init();

    });

    //std::thread::start_acync
    //Thread2* th;
    //_pWorker = new NetworkWorker();

    //th = Gu::GetThreadManager()->getThread(BroThread::NetworkThread);
    //th->runWorkerAsync(_pWorker);
}

void NetworkManager::connectToServerAsync(t_string serverIp)
{
    _pWorker->connectToServer(serverIp);
}
void NetworkManager::startServerAsync()
{
    _pWorker->startServer();
}
//SyncData<NetworkInfo>* NetworkManager::getSyncData()
//{
//    return _pSyncData;
//}
//NetworkInfo* NetworkManager::getNetworkInfoMain()
//{
//    return _pSyncData->clientData();
//}
//NetworkInfo* NetworkManager::getNetworkInfoWorker()
//{
//    return _pSyncData->workerData();
//}
RetCode NetworkManager::sendClientPacket(PacketType iPackType, t_string __in_ str)
{
    return _pWorker->sendClientPacket(iPackType, str);
}
t_bool NetworkManager::getClientIsConnected()
{
    return _pWorker->getIsClientConnected();
}
}//ns game
