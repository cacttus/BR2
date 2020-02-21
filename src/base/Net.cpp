#include "../base/Net.h"
#include "../base/Logger.h"
#include "../base/oglErr.h"
#include "../base/SyncTimer.h"
#include "../base/SDLUtils.h"

#include <SDL.h>
#include <SDL_net.h>

namespace BR2 {
class NetInternal : public VirtualMemory {
public:
  TCPsocket _server_control; // the server control socket.
  std::vector<TCPsocket> _control_clients;
};
Net::Net() {
  _pInternal = std::make_unique<NetInternal>();
  init();
}
Net::~Net() {
  SDLNet_TCP_Close(_pInternal->_server_control);
  _pInternal = nullptr;
}
void Net::init() {
  // create a listening TCP socket on port 44178 (server)
  IPaddress ip;
  if (SDLNet_ResolveHost(&ip, NULL, 44178) == -1) {
    Br2LogInfo("SDLNet_ResolveHost:" + SDLNet_GetError());
    _bError = true;
    return;
  }

  _pInternal->_server_control = SDLNet_TCP_Open(&ip);
  if (!_pInternal->_server_control) {
    Br2LogInfo("SDLNet_TCP_Open:" + SDLNet_GetError());
    _bError = true;
    return;
  }

  _pTimer = std::make_shared<SyncTimer>(true);
}
void Net::update() {
  _pTimer->tick(500, [this]() {
    if (_bError == false) {
      //Simple SDL_Net example
      // https://gist.github.com/psqq/b92243f2149fcf4dd46370d4c0b5fef9

      //Check any dangling SDL errors before clearing them.
      SDLUtils::checkSDLErr();

      //Accept control clients
      TCPsocket new_control_client = SDLNet_TCP_Accept(_pInternal->_server_control);
      if (!new_control_client) {
        //SDL sets an error in the API when accept fails.  This isn't an error.  We can comment it out, but we will just swallow it here.
        SDL_ClearError();
      }
      else {
        _pInternal->_control_clients.push_back(new_control_client);
      }
      for (TCPsocket sock : _pInternal->_control_clients) {
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
    });
}








}//ns BR2
