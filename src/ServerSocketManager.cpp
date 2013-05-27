#include "stdafx.h"
#include "ServerSocketManager.h"
#include "BinaryPacket.h"
#include "RemoteSocket.h"
#include "GameServerListenSocket.h"

bool ServerSocketManager::start()
{
	if (!BaseSocketManager::init())
        return false;

	GameServerListenSocket* socket = new GameServerListenSocket(3709);
	addSocket(socket);

	return true;
}

void ServerSocketManager::broadcastToAllClients(shared_ptr<BinaryPacket> packet)
{
	for (SocketList::iterator i = mSockList.begin(); i != mSockList.end(); ++i)
    {
		NetSocket *sock = *i;

		if ((dynamic_cast<RemoteSocket*>(sock)) != NULL)
		{
			sock->send(packet);
		}
	}
}