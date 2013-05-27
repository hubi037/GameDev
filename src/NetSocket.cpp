#include "stdafx.h"
#include "NetSocket.h"

#include "BinaryPacket.h"
#include "BaseSocketManager.h"



NetSocket::NetSocket() 
{ 
    mSock = INVALID_SOCKET;
    mDeleteFlag = 0;
    mSendOfs = 0;
    mTimeOut = 0;

    mRecvOfs = mRecvBegin = 0;
    mInternal = 0;
    mBinaryProtocol = 1;
}


NetSocket::NetSocket(SOCKET new_sock, unsigned int hostIP)
{
    mDeleteFlag = 0;
    mSendOfs = 0;
    mTimeOut = 0;

    mBinaryProtocol = 1;

    mRecvOfs = mRecvBegin = 0;
    mInternal = 0;

    mTimeCreated = timeGetTime();

    mSock = new_sock;
    mIpaddr = hostIP;

	mInternal = BaseSocketManager::getSingleton().isInternal(mIpaddr);

    setsockopt (mSock, SOL_SOCKET, SO_DONTLINGER, NULL, 0);

        /***
        // Here's how to find the host address of the connection. It is very slow, however.
        if (m_ipaddr)
        {
                TCHAR buffer[128];
                const char *ansiIpaddress = g_pSocketManager->GetHostByAddr(m_ipaddr);
                if (ansiIpaddress)
                {
                        TCHAR genIpaddress[64];
                        AnsiToGenericCch(genIpaddress, ansiIpaddress, static_cast<int>(strlen(ansiIpaddress)+1));
                        _tcssprintf(buffer, _T("User connected: %s %s"), genIpaddress, (m_internal) ? _T("(internal)") : _T(""));
            LogManager::getSingleton().logMessage(buffer);
                }
        }
        ***/
}

NetSocket::~NetSocket() 
{
    if (mSock != INVALID_SOCKET) 
    {
            closesocket(mSock);
            mSock = INVALID_SOCKET;
    }
 }


bool NetSocket::connect(unsigned int ip, unsigned int port, bool forceCoalesce) 
{
        struct sockaddr_in sa;
        int x = 1;

        if ((mSock = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
                return false;

        if (!forceCoalesce)
        {
                setsockopt(mSock, IPPROTO_TCP, TCP_NODELAY, (char *)&x, sizeof(x));
        }

        sa.sin_family = AF_INET;
        sa.sin_addr.s_addr = htonl(ip);
        sa.sin_port = htons(port);

        if (::connect(mSock, (struct sockaddr *)&sa, sizeof(sa))) 
        {
            closesocket(mSock);
            mSock = INVALID_SOCKET;
            return false;
        }

        return true;
 }

void NetSocket::send(shared_ptr<BinaryPacket> pkt, bool clearTimeOut)
{
    if (clearTimeOut)
            mTimeOut = 0;

    mOutList.push_back(pkt);
}

void NetSocket::setBlocking(bool blocking) 
{
        #ifdef WIN32
                unsigned long val = blocking ? 0 : 1;
                ioctlsocket(mSock, FIONBIO, &val);
        #else
                int val = fcntl(m_sock, F_GETFL, 0);
                if (blocking)
                        val &= ~(O_NONBLOCK);
                else
                        val |= O_NONBLOCK;

                fcntl(m_sock, F_SETFL, val);
        #endif
}


void NetSocket::handleOutput() 
{
        int fSent = 0;
        do 
        {
                assert(!mOutList.empty());
                PacketList::iterator i = mOutList.begin();

                shared_ptr<BinaryPacket> pkt = *i;
                const char *buf = pkt->getData();
                int len = static_cast<int>(pkt->getSize());

                int rc = ::send(mSock, buf+mSendOfs, len-mSendOfs, 0);
                if (rc > 0) 
                {
						BaseSocketManager::getSingleton().addToOutbound(rc);
                        mSendOfs += rc;
                        fSent = 1;
                }
                else if (WSAGetLastError() != WSAEWOULDBLOCK)
                {
                        handleException();
                        fSent = 0;
                }
                else
                {
                        fSent = 0;
                }

                if (mSendOfs == pkt->getSize()) 
                {
                        mOutList.pop_front();
                        mSendOfs = 0;
                }

        } while ( fSent && !mOutList.empty() );
}

void NetSocket::handleInput() 
{
        bool bPktRecieved = false;
        u_long packetSize = 0;
        int rc = recv(mSock, mRecvBuf+mRecvBegin+mRecvOfs, RECV_BUFFER_SIZE-(mRecvBegin+mRecvOfs), 0);

        char metrics[1024];
        sprintf_s(metrics, 1024, "Network: Incoming: %6d bytes. Begin %6d Offset %4d\n", rc, mRecvBegin, mRecvOfs);
		Ogre::LogManager::getSingleton().logMessage(metrics);

        if (rc==0)
        {
                return;
        }

        if (rc==SOCKET_ERROR)
        {
                mDeleteFlag = 1;
                return;
        }

        const int hdrSize = sizeof(u_long);
        unsigned int newData = mRecvOfs + rc;
        int processedData = 0;

        while (newData > hdrSize)
        {
                // There are two types of packets at the lowest level of our design:
                // BinaryPacket - Sends the size as a positive 4 byte integer
                // TextPacket - Sends 0 for the size, the parser will search for a CR

                packetSize = *(reinterpret_cast<u_long*>(mRecvBuf+mRecvBegin));
                packetSize = ntohl(packetSize);

                if (mBinaryProtocol)
                {
                        // we don't have enough new data to grab the next packet
                        if (newData < packetSize)
                                break;

                        if (packetSize > MAX_PACKET_SIZE)
                        {
                                // prevent nasty buffer overruns!
                                handleException();
                                return;
                        }

                        if (newData >= packetSize)
                        {
                                shared_ptr<BinaryPacket> pkt(new BinaryPacket(&mRecvBuf[mRecvBegin+hdrSize], packetSize-hdrSize));
                                mInList.push_back(pkt);
                                bPktRecieved = true;
                                processedData += packetSize;
                                newData -= packetSize;
                                mRecvBegin += packetSize;
                        }
                }
        }

		BaseSocketManager::getSingleton().addToInbound(rc);
        mRecvOfs = newData;

        if (bPktRecieved)
        {
                if (mRecvOfs == 0)
                {
                        mRecvBegin = 0;
                }
                else if (mRecvBegin + mRecvOfs + MAX_PACKET_SIZE > RECV_BUFFER_SIZE)
                {
                        // we don't want to overrun the buffer - so we copy the leftover bits 
                        // to the beginning of the recieve buffer and start over
                        int leftover = mRecvOfs;
                        memcpy(mRecvBuf, &mRecvBuf[mRecvBegin], mRecvOfs);
                        mRecvBegin = 0;
                }
        }       
}