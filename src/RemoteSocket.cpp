#include "stdafx.h"
#include "RemoteSocket.h"
#include "BinaryPacket.h"
#include <strstream>

#include "GameApplication.h"
#include "Spacecraft.h"

void RemoteSocket::handleInput()
{
    NetSocket::handleInput();

    // traverse the list of m_InList packets and do something useful with them
    while (!mInList.empty())
    {
        shared_ptr<BinaryPacket> packet = *mInList.begin();
        mInList.pop_front();

		const char *buf = packet->getData();
        int size = static_cast<int>(packet->getSize());

		String logMsg = String("incoming: ");
		logMsg.append(buf, size);
		LogManager::getSingleton().logMessage(logMsg);

        std::istrstream in(buf+sizeof(u_long), (size-sizeof(u_long)));

		// TODO unserialize input
    }
}