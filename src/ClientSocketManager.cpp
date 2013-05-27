#include "stdafx.h"
#include "ClientSocketManager.h"
#include "RemoteSocket.h"

bool ClientSocketManager::connect()
{
    if (!BaseSocketManager::init())
        return false;

    RemoteSocket *pSocket = new RemoteSocket;
        
    if (!pSocket->connect(getHostByName(mHostName), mPort) )
    {
        SAFE_DELETE(pSocket);
        return false;
    }
    
	addSocket(pSocket);
    return true;
}