#include "stdafx.h"
#include "ClientSocketManager.h"
#include "RemoteSocket.h"

bool ClientSocketManager::connect()
{
    if (!BaseSocketManager::init())
        return false;

    RemoteSocket* remoteSocket = new RemoteSocket();
        
    if (!remoteSocket->connect(getHostByName(mHostName), mPort) )
    {
        SAFE_DELETE(remoteSocket);
        return false;
    }
    
	addSocket(remoteSocket);
    return true;
}

void ClientSocketManager::sendToServer(shared_ptr<BinaryPacket> packet)
{
	if (mSockList.size() > 0)
	{
		(*mSockList.begin())->send(packet);
	}
}