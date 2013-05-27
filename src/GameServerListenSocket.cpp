#include "stdafx.h"
#include "GameServerListenSocket.h"

#include "BaseSocketManager.h"
#include "RemoteSocket.h"
#include "BinaryPacket.h"


void GameServerListenSocket::handleInput() 
{
        unsigned int theipaddr;
        SOCKET new_sock = acceptConnection(&theipaddr);

        int value = 1;
        setsockopt(new_sock, SOL_SOCKET, SO_DONTLINGER, (char *)&value, sizeof(value));

        if (new_sock != INVALID_SOCKET)
        {
			RemoteSocket * sock = new RemoteSocket(new_sock, theipaddr);
			int sockId = BaseSocketManager::getSingleton().addSocket(sock);
            int ipAddress = BaseSocketManager::getSingleton().getIpAddress(sockId);
        }
 }
