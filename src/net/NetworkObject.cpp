#include "../net/NetworkObject.h"
#include "../net/NonBlockingSocket.h"

namespace Game {;
NetworkObject::NetworkObject(NetworkWorker* pa) :
    _pNetworkManager(pa) 
{
}
NetworkObject::~NetworkObject()
{
}
void NetworkObject::checkIfGood( SocketState state )
{
     AssertOrThrow2(getSocket()!=NULL);
    getSocket()->assertState(state);
}




}//ns game
