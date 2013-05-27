#ifndef __ServerSocketManager_h_
#define __ServerSocketManager_h_

#include "BaseSocketManager.h"

class BinaryPacket;

/// Specialist socket manager used on the server.
class ServerSocketManager: public BaseSocketManager
{
public:
	bool start();
	void broadcastToAllClients(shared_ptr<BinaryPacket> packet);
};

#endif