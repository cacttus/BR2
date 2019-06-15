/**
*
*    @file Net.h
*    @date May 9, 2019
*    @author Derek Page
*
*    © 2019 
*
*
*/
#pragma once
#ifndef __NET_15574636493489018174_H__
#define __NET_15574636493489018174_H__


#include "../base/BaseHeader.h"

namespace Game {
/**
*    @class Net
*    @brief
*
* https://www.libsdl.org/projects/SDL_net/
*/
class Net : public VirtualMemory {
    TCPsocket _server_control; // the server control socket.
    std::vector<TCPsocket> _control_clients;

public:
    void init();
    void update();
    Net();
    virtual ~Net() override;
};

}//ns Game



#endif
