#include "../base/Net.h"
#include "../base/Logger.h"
#include "../base/oglErr.h"
#include "../base/SyncTimer.h"
#include "../base/SDLUtils.h"

#include <SDL_net.h>


namespace BR2 {
class Net_Internal {
public:
  bool _bError = false;
  TCPsocket _server_control; // the server control socket.
  std::vector<TCPsocket> _control_clients;
  std::shared_ptr<SyncTimer> _pTimer;

  void init() {
    // create a listening TCP socket on port 44178 (server)
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, NULL, 44178) == -1) {
      BRLogInfo("SDLNet_ResolveHost:" + SDLNet_GetError());
      _bError = true;
      return;
    }

    _server_control = SDLNet_TCP_Open(&ip);
    if (!_server_control) {
      BRLogInfo("SDLNet_TCP_Open:" + SDLNet_GetError());
      _bError = true;
      return;
    }

    _pTimer = std::make_shared<SyncTimer>(true);
  }
};
Net::Net() {
  _pint = std::make_unique<Net_Internal>();
  _pint->init();
}
Net::~Net() {
  SDLNet_TCP_Close(_pint->_server_control);
  _pint = nullptr;
}
void Net::update() {
  _pint->_pTimer->tick(500, [this]() {
    if (_pint->_bError == false) {
      //Simple SDL_Net example
      // https://gist.github.com/psqq/b92243f2149fcf4dd46370d4c0b5fef9

      //Check any dangling SDL errors before clearing them.
      SDLUtils::checkSDLErr();

      //Accept control clients
      TCPsocket new_control_client = SDLNet_TCP_Accept(_pint->_server_control);
      if (!new_control_client) {
        //SDL sets an error in the API when accept fails.  This isn't an error.  We can comment it out, but we will just swallow it here.
        SDL_ClearError();
      }
      else {
        _pint->_control_clients.push_back(new_control_client);
      }
      for (TCPsocket sock : _pint->_control_clients) {
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








}//ns Game
