#include "NetworkInfo.h"


namespace Game {

SocketInfo::SocketInfo() :
_bConnected(false)
,_nPacketsReceived(0)
,_nPacketsSent(0)
,_eLastError(RetCode::SOCK_TRUE)
{

}
ClientInfo::ClientInfo() : 
     _strServerIp("") 
{ 
}
ServerInfo::ServerInfo() :
    _iNumConnectedClients(0) 
{
}
NetworkInfo::NetworkInfo() : 
    _iNetworkTicks(0) 
{

}






}//ns Game
