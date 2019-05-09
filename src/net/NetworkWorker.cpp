#include "../net/NetworkWorker.h"
#include "../net/NetworkManager.h"
#include "../net/NetServer.h"
#include "../net/NetClient.h"
#include "../net/NetworkUtils.h"
#include "../base/Crc32.h"
#include "../base/EngineConfig.h"
#include "../base/StringUtil.h"
#include "../net/NetworkInfo.h"
#include "../base/Logger.h"
//#include "../base/SyncData.h"
//#include "../base/ThreadMessage2.h"

namespace Game {
///////////////////////////////////////////////////////////////////
#pragma region Public Methods
NetworkWorker::NetworkWorker()
{
}
NetworkWorker::~NetworkWorker() 
{
    shutdown();
}

t_string NetworkWorker::getPacketPrependTag()
{
    //The tag that comes at the beginning to identify a packet.
    //Constant - we may change this in the future, but for now there's no need.
    return "VT";
}

RetCode NetworkWorker::startServer()
{
    std::lock_guard<std::mutex> guard(getMutex());

    NetPort iCommPort = Gu::getEngineConfig()->getTcpServerPort();
    BroLogInfo("Starting server on port ", iCommPort);

    if (_pServer)
    {
        _pServer->shutdown();
        DEL_MEM(_pServer);
    }

    // initPackTypeMaps();
    restartNetworkSystem();

    _pServer = new NetServer(this);
    RetCode ret = _pServer->init(iCommPort);
    if (ret != SOCK_SUCCESS)
        addError(ret);

    //Run an update on the server
    //_pServer->update();
    return ret;
}
RetCode NetworkWorker::connectToServer(t_string serverIp, int32_t iClientPort, int32_t iServerPort)
{
    std::lock_guard<std::mutex> guard(getMutex());

    if (_pClient)
    {
        _pClient->shutdown();
        DEL_MEM(_pClient);
    }

    _pClient = new NetClient(this);


    int iTimeout = Gu::getEngineConfig()->getNetClientConnectTimeoutSeconds();

    BroLogInfo("Connecting to server with a timeout of ", iTimeout, " seconds.");

    RetCode sr;
    sr = _pClient->connectToServer(
        iClientPort
        , iServerPort
        , serverIp
        , iTimeout * 1000
    );
    if (sr != SOCK_SUCCESS)
    {
        BroLogError("Client failed to connect to server: ", NetworkUtils::GetSockErrStr(sr));
    }
    return sr;
}

RetCode NetworkWorker::sendClientPacket(PacketType iPackType, t_string __in_ str)
{
    std::lock_guard<std::mutex> guard(getMutex());
    
    if (_pClient)
        return _pClient->sendPacket(iPackType, str);
    else
        BroLogError("Client was not runnijng.");
    return RetCode::SOCK_CONNERR;
}
t_bool NetworkWorker::getIsClientConnected()
{
    std::lock_guard<std::mutex> guard(getMutex());
    
    if (_pClient == NULL)
        return false;
    return StringUtil::isNotEmpty(_pClient->getConnectedServerIp());
}
#pragma endregion


#pragma region Private methods
void NetworkWorker::init()
{
}
void NetworkWorker::updateWorker()
{
    if (_pClient) {
        _pClient->update();
    }
    if (_pServer) {
        _pServer->update();
    }

   // getNetworkInfoWorker()->_iNetworkTicks++;

    checkErrorsAsync(__LINE__, __FILE__, false);

 //   _pSyncData->updateWorkerSync();
}
//void NetworkWorker::processMessage(ThreadMessage2* ptm)
//{
//    if (ptm->getMessage() == NetMessages::NetStartServer) {
//        startServer();
//    }
//}
void NetworkWorker::stop()
{
    shutdown();
}
void NetworkWorker::checkErrorsAsync(int line, char* file, t_bool bThrow)
{
    int wsaError = WSAGetLastError();
    if (wsaError == SOCKET_ERROR)
    {
        t_string errstr = TStr("Network error", wsaError, "(", StringUtil::toHex(wsaError, true), ") Line: ", line, ", File: ", file);
        if (bThrow) {
            BroThrowException(errstr);
        }
        else {
            BroLogError(errstr);
        }
    }
}
void NetworkWorker::stopServerOrClient()
{
    BroLogInfo("Stopping server/client");
    shutdown();
}

void NetworkWorker::restartNetworkSystem()
{
    shutdown();

    RetCode ret = startup();

    if (ret != SOCK_SUCCESS)
        BroLogError("Could not start/restart network host. Wsa Error: ", ret);
}

RetCode NetworkWorker::startup()
{
    int err = WSAStartup(0x0101, &_wsaData);

    if (err == SOCKET_ERROR) {
        return SOCK_STARTUPERR;
    }

    _eNetworkState = NETWORK_RUNNING;

    return SOCK_SUCCESS;
}
RetCode NetworkWorker::shutdown()
{
    if (_pClient) {
        _pClient->shutdown();
    }
    if (_pServer) {
        _pServer->shutdown();
    }
    DEL_MEM(_pClient);
    DEL_MEM(_pServer);

    if (_eNetworkState == NETWORK_RUNNING)
        WSACleanup();

    _eNetworkState = NETWORK_SHUTDOWN;

    return SOCK_SUCCESS;
}
void NetworkWorker::addError(RetCode e)
{
    t_string strError = TStr(" [NET] Socket Error: ", NetworkUtils::GetSockErrStr(e));
    BroLogError(strError);
    _vecErrors.add(strError);
}

#pragma endregion






}//ns Game
