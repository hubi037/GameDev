 #ifndef __BaseSocketManager_h_
 #define __BaseSocketManager_h_ 

class NetSocket;
class BinaryPacket;

/// The BaseSocketManager handles multiple sockets.
/// It takes care for proper shutdown and provides useful utility functions.
/// The class can be overwritten on client or server side.
/// see http://code.google.com/p/gamecode4/
class BaseSocketManager: public Ogre::Singleton<BaseSocketManager>
{
protected:
	WSADATA mWsaData;

	typedef std::list<NetSocket*> SocketList;
	typedef std::map<int, NetSocket*> SocketIdMap;

	SocketList mSockList;
	SocketIdMap mSockMap;

	int mNextSocketId;
	unsigned int mInbound;
	unsigned int mOutbound;
	unsigned int mMaxOpenSockets;
	unsigned int mSubnetMask;
	unsigned int mSubnet;

	NetSocket *findSocket(int sockId);

public:
    BaseSocketManager();
    virtual ~BaseSocketManager() { shutdown(); }

    void doSelect(int pauseMicroSecs, bool handleInput = true);

    bool init();
    void shutdown();
    void printError();

    int addSocket(NetSocket *socket); 
    void removeSocket(NetSocket *socket);

    unsigned int getHostByName(const std::string &hostName);
    const char *getHostByAddr(unsigned int ip);

    int getIpAddress(int sockId);

    void setSubnet(unsigned int subnet, unsigned int subnetMask)
    {
		mSubnet = subnet;
		mSubnetMask = subnetMask;
    }
    bool isInternal(unsigned int ipaddr);

    bool send(int sockId, shared_ptr<BinaryPacket> packet);

    void addToOutbound(int rc) { mOutbound += rc; }
    void addToInbound(int rc) { mInbound += rc; }
};

#endif