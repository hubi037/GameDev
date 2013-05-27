#ifndef __GameServerListenSocket_h_
#define __GameServerListenSocket_h_

#include "NetListenSocket.h"

/// Used by the server to accept new connections from the client and creates a RemoteSocket.
/// see http://code.google.com/p/gamecode4/
class GameServerListenSocket: public NetListenSocket
{
public:
	GameServerListenSocket(int portnum) { init(portnum); }
	void handleInput();
};

#endif