#ifndef __RemnoteSocket_h_
#define __RemnoteSocket_h_

#include "NetSocket.h"

/// Socket used for communication between client and server.
/// see http://code.google.com/p/gamecode4/
class RemoteSocket: public NetSocket 
{
public:
	enum Message
	{
		MESSAGE_NONE,
		MESSAGE_INPUT,
		MESSAGE_STATE,
		MESSAGE_NEW_PLAYER,
		MESSAGE_REMOVE_PLAYER
	};

    // server accepting a client
    RemoteSocket(SOCKET new_sock, unsigned int hostIP)         
    : NetSocket(new_sock, hostIP)
    {}

    // client attach to server
    RemoteSocket() { };                                                                                

    virtual void handleInput();

	void handleInputMsg(std::istrstream& in);
	void handleStateMsg(std::istrstream& in);
};

#endif