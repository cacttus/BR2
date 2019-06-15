#include "Net.h"


namespace Game {
///////////////////////////////////////////////////////////////////
Net::Net(){
    // create a listening TCP socket on port 44178 (server)
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, NULL, 44178) == -1) {
        printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        exit(1);
    }
    _server_control = SDLNet_TCP_Open(&ip);
    if (!_server_control) {
        printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        exit(2);
    }
}
Net::~Net() {
    SDLNet_TCP_Close(_server_control);
}
///////////////////////////////////////////////////////////////////
void Net::init() {
}
void Net::update() {

    //Accept control clients
    TCPsocket new_control_client = SDLNet_TCP_Accept(_server_control);
    if (!new_control_client) {
    }
    else {
        _control_clients.push_back(new_control_client);
    }
    for (TCPsocket sock : _control_clients) {
        char buf[2048];
        int maxlen = 2048;
        int result = SDLNet_TCP_Recv(sock, buf, maxlen);
        if (result < 0) {
            //Error.  Disconnect the sock
            SDLNet_TCP_Close(sock);
        }
        else if (result > 0) {
            int n = 0;
            n++;
        }

    }

}








}//ns Game
