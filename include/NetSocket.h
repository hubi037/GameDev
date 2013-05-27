 #ifndef __NetSocket_h_
 #define __NetSocket_h_ 

class BinaryPacket;

#define MAX_PACKET_SIZE (256)
#define RECV_BUFFER_SIZE (MAX_PACKET_SIZE * 512)
#define MAX_QUEUE_PER_PLAYER (10000)

/// Encapsulates socket handling.
/// Classes can derive from the NetSocket and provide specialist implementation
/// for handlin in and output of the socket (handleOutput, handleInput).
/// see http://code.google.com/p/gamecode4/
class NetSocket 
{
        friend class BaseSocketManager;
        typedef std::list<shared_ptr<BinaryPacket>> PacketList;

public:
        NetSocket();                                                                                    // clients use this to initialize a NetSocket prior to calling Connect.
        NetSocket(SOCKET new_sock, unsigned int hostIP);                // servers use this when new clients attach.
        virtual ~NetSocket();

        bool connect(unsigned int ip, unsigned int port, bool forceCoalesce = 0);
        void setBlocking(bool blocking);
        void send(shared_ptr<BinaryPacket> pkt, bool clearTimeOut=1);

        virtual int hasOutput() { return !mOutList.empty(); }
        virtual void handleOutput();
        virtual void handleInput();
        virtual void timeOut() { mTimeOut=0; }

        void handleException() { mDeleteFlag |= 1; }

        void setTimeOut(unsigned int ms=45*1000) { mTimeOut = timeGetTime() + ms; }

        int getIpAddress() { return mIpaddr; }

protected:
    SOCKET mSock;
    int mId;                               // a unique ID given by the socket manager

        // note: if deleteFlag has bit 2 set, exceptions only close the
        //   socket and set to INVALID_SOCKET, and do not delete the NetSocket
    int mDeleteFlag;
 
    PacketList mOutList;
    PacketList mInList;

    char mRecvBuf[RECV_BUFFER_SIZE];
    unsigned int mRecvOfs, mRecvBegin;
    bool mBinaryProtocol;

    int mSendOfs;
    unsigned int mTimeOut;
    unsigned int mIpaddr;

    int mInternal;
    int mTimeCreated;
};

#endif