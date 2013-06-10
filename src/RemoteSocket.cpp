#include "stdafx.h"
#include "RemoteSocket.h"
#include "BinaryPacket.h"
#include <strstream>

#include "GameApplication.h"
#include "Spacecraft.h"
#include "RemoteHumanController.h"


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
		logMsg.append(buf+sizeof(u_long), size-sizeof(u_long));
		LogManager::getSingleton().logMessage(logMsg);

        std::istrstream in(buf+sizeof(u_long), (size-sizeof(u_long)));

		int type;
		in >> type;

		switch (type)
		{
		case MESSAGE_INPUT:
			handleInputMsg(in);
			break;
		case MESSAGE_STATE:
			handleStateMsg(in);
			break;
		}
    }
}

void RemoteSocket::handleInputMsg(std::istrstream& in)
{
	// process input
	int id;

	in >> id;

	RemoteHumanController* controller = dynamic_cast<RemoteHumanController*> (GameApplication::getSingleton().getController(id));
	_ASSERT(controller != NULL);

	controller->unserialize(in);

	// respond current state to client

	std::ostrstream out;
	out << MESSAGE_STATE << " ";

	int count = GameApplication::getSingleton().getSpacecraftCount();

	out << count << " ";

	for (int i=0; i < count; i++)
	{
		Spacecraft* craft = GameApplication::getSingleton().getSpacecraft(i);

		out << craft->getId() << " ";
		craft->serialize(out);
	}

	shared_ptr<BinaryPacket> pkt(new BinaryPacket(out.rdbuf()->str(), out.pcount()));
	send(pkt);
}

void RemoteSocket::handleStateMsg(std::istrstream& in)
{
	int count;
	in >> count;

	for (int i=0; i<count; i++)
	{
		int idx;
		in >> idx;
		Spacecraft* craft = GameApplication::getSingleton().getSpacecraft(idx);
		craft->unserialize(in);
	}
}