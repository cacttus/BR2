/**
*
*    @file NetworkObject.h
*    @date December 9, 2010
*    @author Derek Page
*
*    © 2011 
*
*    © 2011 
*
*
*/
#pragma once
#ifndef __NETWORKOBJECT_24521682113618583620159_H__
#define __NETWORKOBJECT_24521682113618583620159_H__

#include "../net/NetHeader.h"

namespace Game {;
/**
*    @class
*    @brief Base class for net work classes like client or server
*/
class NetworkObject : public VirtualMemory { 
protected:
    NetworkWorker* _pNetworkManager;
    //enum { STATIC_BUF_LEN = 1500 };

    //char _staticBuf[ STATIC_BUF_LEN ];    // - The buffer for holding recv() data.
public:

    virtual void update() = 0;
    virtual NonBlockingSocket* getSocket() = 0;
    virtual NetworkObjectType getObjectType() = 0;
    virtual void shutdown()=0;

    // - Assert that this network object is in a good state.
    void checkIfGood( SocketState state );

    NetworkObject(NetworkWorker* pa);
    virtual ~NetworkObject() override;
};




}//ns game



#endif
