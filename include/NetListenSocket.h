#ifndef __NetListenSocket_h_
#define __NetListenSocket_h_

#include "NetSocket.h"


/// Specialist NetSocket to listen for new connection.
/// see http://code.google.com/p/gamecode4/
class NetListenSocket: public NetSocket
{
public:
	NetListenSocket() {};
	NetListenSocket(int portnum);

	void init(int portnum);
	SOCKET acceptConnection(unsigned int *pAddr);

	unsigned short port;
};

#endif