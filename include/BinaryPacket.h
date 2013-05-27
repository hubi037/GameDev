 #ifndef __BinaryPacket_h_
 #define __BinaryPacket_h_ 

#include <Winsock2.h>

/// Base class for packets sent over the network.
/// see http://code.google.com/p/gamecode4/
class BinaryPacket
{
	char *mData;

public:
	BinaryPacket(char const* const data, unsigned long size)
	{
		mData = new char[size + sizeof(unsigned long)];
		*(unsigned long*)mData = htonl(size+sizeof(unsigned long));
		memcpy(mData + sizeof(unsigned long), data, size);
	}

	BinaryPacket(u_long size)
	{
		mData = new char[size + sizeof(unsigned long)];
		*(unsigned long*)mData = htonl(size+sizeof(unsigned long));
	}

	virtual ~BinaryPacket() { SAFE_DELETE(mData); }
	virtual char const * const getData() const { return mData; }
	virtual u_long getSize() const { return ntohl(*(unsigned long*) mData); }
	void MemCpy(char const* const data, size_t size, int destOffset)
	{
		assert(size+destOffset <= getSize() - sizeof(u_long));
		memcpy(mData + destOffset + sizeof(u_long), data, size);
	}
};

#endif