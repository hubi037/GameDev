#ifndef __ClientSocketManager_h_
#define __ClientSocketManager_h_ 

#include "BaseSocketManager.h"

/// Specialist version of the BaseSocketManager on the client.
/// Tries to create a single socket that attaches to a known server.
/// see http://code.google.com/p/gamecode4/
class ClientSocketManager : public BaseSocketManager
{
    std::string mHostName;
    unsigned int mPort;

public:
    ClientSocketManager(const std::string &hostName, unsigned int port)
    {
        mHostName = hostName;
        mPort = port;
    }

    bool connect();
};

#endif