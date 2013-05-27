#include "stdafx.h"
#include "BaseSocketManager.h"

#include "NetSocket.h"

template<> BaseSocketManager* Ogre::Singleton<BaseSocketManager>::msSingleton = 0;

BaseSocketManager::BaseSocketManager() 
{ 
    mInbound = 0;
    mOutbound = 0;
    mMaxOpenSockets = 0;
    mSubnetMask = 0;
    mSubnet = 0xffffffff;
    mNextSocketId = 0;

    ZeroMemory(&mWsaData, sizeof(WSADATA)); 
}


bool BaseSocketManager::init()
{
    if (WSAStartup(0x0202, &mWsaData)==0)
        return true;
    else
    {
        Ogre::LogManager::getSingleton().logMessage("WSAStartup failure!");
        return false;
    }
}

void BaseSocketManager::shutdown()
{
    // Get rid of all those pesky kids...
    while (!mSockList.empty())
    {
        delete *mSockList.begin();
        mSockList.pop_front();
    }

    WSACleanup();
}

int BaseSocketManager::addSocket(NetSocket *socket) 
{ 
    socket->mId = mNextSocketId;
    mSockMap[mNextSocketId] = socket;
    ++mNextSocketId; 

    mSockList.push_front(socket);  
    if (mSockList.size() > mMaxOpenSockets)
		++mMaxOpenSockets;

    return socket->mId; 
}


void BaseSocketManager::removeSocket(NetSocket *socket) 
{ 
    mSockList.remove(socket); 
    mSockMap.erase(socket->mId);
    SAFE_DELETE(socket);
}


NetSocket *BaseSocketManager::findSocket(int sockId)
{
    SocketIdMap::iterator i = mSockMap.find(sockId);
    if (i==mSockMap.end())
    {
       return NULL;
    }
    return i->second;
}


int BaseSocketManager::getIpAddress(int sockId)
{
    NetSocket *socket = findSocket(sockId);
    if (socket)
    {
       return socket->getIpAddress();
    }
    else
    {
       return 0;
    }
}


bool BaseSocketManager::send(int sockId, shared_ptr<BinaryPacket> packet)
{
    NetSocket *sock = findSocket(sockId);
    if (!sock)
        return false;
    sock->send(packet);
    return true;
}


void BaseSocketManager::doSelect(int pauseMicroSecs, bool handleInput) 
{
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = pauseMicroSecs;    // 100 microseconds is 0.1 milliseconds or .0001 seconds

    fd_set inp_set, out_set, exc_set;
    int maxdesc;

    FD_ZERO(&inp_set);
    FD_ZERO(&out_set);
    FD_ZERO(&exc_set);

    maxdesc = 0;

    // set everything up for the select
    for (SocketList::iterator i = mSockList.begin(); i != mSockList.end(); ++i)
    {
        NetSocket *pSock = *i;
        if ((pSock->mDeleteFlag&1) || pSock->mSock == INVALID_SOCKET)
            continue;

        if (handleInput)
                FD_SET(pSock->mSock, &inp_set);

        FD_SET(pSock->mSock, &exc_set);

        if (pSock->hasOutput())
                FD_SET(pSock->mSock, &out_set);

        if ((int)pSock->mSock > maxdesc)
                maxdesc = (int)pSock->mSock;

    }
  
    int selRet = 0;

    // do the select (duration passed in as tv, NULL to block until event)
    selRet = select(maxdesc+1, &inp_set, &out_set, &exc_set, &tv) ;
    if (selRet == SOCKET_ERROR)
    {
        printError();
        return;
    }

    // handle input, output, and exceptions

    if (selRet)
    {
        for (SocketList::iterator i = mSockList.begin(); i != mSockList.end(); ++i)
        {
            NetSocket *pSock = *i;

            if ((pSock->mDeleteFlag&1) || pSock->mSock == INVALID_SOCKET)
                continue;

            if (FD_ISSET(pSock->mSock, &exc_set))
            {
               pSock->handleException();
            }

            if (!(pSock->mDeleteFlag&1) && FD_ISSET(pSock->mSock, &out_set))
            {
                 pSock->handleOutput();
            }

            if (   handleInput
                    && !(pSock->mDeleteFlag&1) && FD_ISSET(pSock->mSock, &inp_set))
            {
                pSock->handleInput();
            }
        }      
    }

    unsigned int timeNow = timeGetTime();

    // handle deleting any sockets
    SocketList::iterator i = mSockList.begin();
    while (i != mSockList.end())
    {
        NetSocket *pSock = *i;
        if (pSock->mTimeOut) 
        {
            if (pSock->mTimeOut < timeNow)
            {
                    pSock->timeOut();
            }
        }

        if (pSock->mDeleteFlag&1)
        {
            switch (pSock->mDeleteFlag) 
            {
                case 1:
                    removeSocket(pSock);
                    i = mSockList.begin();
                    break;
                case 3:
                    pSock->mDeleteFlag = 2;
                    if (pSock->mSock != INVALID_SOCKET) 
                    {
                            closesocket(pSock->mSock);
                            pSock->mSock = INVALID_SOCKET;
                    }
                    break;
            }
        }

		if (i != mSockList.end())
		{
			++i;
		}
     }
 }


//
// BaseSocketManager::IsInternal                                        - Chapter 19, page 682
//
bool BaseSocketManager::isInternal(unsigned int ipaddr)
{
   if (!mSubnetMask)
      return false;

   if ((ipaddr & mSubnetMask) == mSubnet)
      return false;

   return true;
}


//
// BaseSocketManager::GetHostByName                                     - Chapter 19, page 683
//
unsigned int BaseSocketManager::getHostByName(const std::string &hostName)
{
   //This will retrieve the ip details and put it into pHostEnt structure
        struct hostent *pHostEnt = gethostbyname(hostName.c_str());
    struct sockaddr_in tmpSockAddr; //placeholder for the ip address

    if(pHostEnt == NULL)
    {
        assert(0);
        return 0;
    }

    memcpy(&tmpSockAddr.sin_addr,pHostEnt->h_addr,pHostEnt->h_length);
        return ntohl(tmpSockAddr.sin_addr.s_addr);
}

//
// BaseSocketManager::GetHostByAddr                                     - Chapter 19, page 683
//
const char *BaseSocketManager::getHostByAddr(unsigned int ip)
{
    static char host[256];

    int netip = htonl(ip);
    struct hostent *lpHostEnt = gethostbyaddr((const char *)&netip, 4, PF_INET);

    if (lpHostEnt)
    {
        strcpy_s(host, 256, lpHostEnt->h_name);
        return host;
    }

    return NULL;
}

//
// BaseSocketManager::PrintError                                        - not described in the book
//
//   A helper function to send a human readable message to the error log
//
void BaseSocketManager::printError()
{
    int realError = WSAGetLastError();
    char* reason;

    switch(realError)
    {
        case WSANOTINITIALISED: reason = "A successful WSAStartup must occur before using this API."; break;
        case WSAEFAULT: reason = "The Windows Sockets implementation was unable to allocated needed resources for its internal operations, or the readfds, writefds, exceptfds, or timeval parameters are not part of the user address space."; break;
        case WSAENETDOWN: reason = "The network subsystem has failed."; break;
        case WSAEINVAL: reason = "The timeout value is not valid, or all three descriptor parameters were NULL."; break;
        case WSAEINTR: reason = "The (blocking) call was canceled via WSACancelBlockingCall."; break;
        case WSAEINPROGRESS: reason = "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function."; break;
        case WSAENOTSOCK: reason = "One of the descriptor sets contains an entry which is not a socket."; break;
        default: reason = "Unknown."; 
    }

    char buffer[256];
    sprintf_s(buffer, "Network: SOCKET error: %s", reason);
	Ogre::LogManager::getSingleton().logMessage(buffer);
}
