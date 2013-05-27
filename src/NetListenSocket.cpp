#include "stdafx.h"
#include "NetListenSocket.h"

NetListenSocket::NetListenSocket(int portnum) 
{
    port = 0;
    init(portnum);
}


void NetListenSocket::init(int portnum)
{
    struct sockaddr_in sa;
    int value = 1;

    if ((mSock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
		int errorCode = WSAGetLastError();
		Ogre::String error = "NetListenSocket Error: Init failed to create socket handle" + errorCode;
		Ogre::LogManager::getSingleton().logMessage(error);
    }

    if (setsockopt(mSock, SOL_SOCKET, SO_REUSEADDR, (char *)&value, sizeof(value))== SOCKET_ERROR) 
    {
        perror("NetListenSocket::Init: setsockopt");
        closesocket(mSock);
        mSock = INVALID_SOCKET;
        Ogre::LogManager::getSingleton().logMessage("NetListenSocket Error: Init failed to set socket options");
    }
        
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = ADDR_ANY;
    sa.sin_port = htons(portnum);

    // bind to port
    if (bind(mSock, (struct sockaddr *)&sa, sizeof(sa)) == SOCKET_ERROR) 
    {
        Ogre::LogManager::getSingleton().logMessage("NetListenSocket::Init: bind");
        closesocket(mSock);
        mSock = INVALID_SOCKET;
        Ogre::LogManager::getSingleton().logMessage("NetListenSocket Error: Init failed to bind");
    }

    // set nonblocking - accept() blocks under some odd circumstances otherwise
    setBlocking(false);

    // start listening
    if (listen(mSock, 256) == SOCKET_ERROR) 
    {
        closesocket(mSock);
        mSock = INVALID_SOCKET;
        Ogre::LogManager::getSingleton().logMessage("NetListenSocket Error: Init failed to listen");
    }

    port = portnum;
}


SOCKET NetListenSocket::acceptConnection(unsigned int *pAddr)
{
	Ogre::LogManager::getSingleton().logMessage("acceptConnection");
    SOCKET new_sock;
    struct sockaddr_in sock;
    int size = sizeof(sock);

    if ((new_sock = accept(mSock, (struct sockaddr *)&sock, &size))== INVALID_SOCKET)
        return INVALID_SOCKET;

    if (getpeername(new_sock, (struct sockaddr *)&sock, &size) == SOCKET_ERROR)
    {
        closesocket(new_sock);
        return INVALID_SOCKET;
    }
    *pAddr = ntohl(sock.sin_addr.s_addr);
    return new_sock;
 }
