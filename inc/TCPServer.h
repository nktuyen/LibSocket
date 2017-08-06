#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__
//////////////////////////////////////////////////////////////////////////
#include <map>
#include "../LibSocket/inc/Thread.h"
#include "../LibSocket/inc/Socket.h"
#include "TCPConnection.h"

namespace t{
//////////////////////////////////////////////////////////////////////////

class TCPServer : public Thread
{
public:
protected:
private:
	Socket mSocket;
	char mIP[MAX_IP_ADDR_LEN];
	UShort	mPort;
	std::map<t::Socket*,TCPConnection*> mConnections;
	typedef std::map<t::Socket*,TCPConnection*>::iterator ConnectionIter;
public:
	TCPServer(UShort uPort);
	virtual ~TCPServer();

	inline const char* getAddress() { return mIP; }
	inline UShort getPortNumber() { return mPort; }
protected:
	bool onInitialize();
	void onRun();
	int onFinalize();
private:
	void Dump(char* data, int len);
	void closeAllConnection();
};
//////////////////////////////////////////////////////////////////////////
};	//namespace
//////////////////////////////////////////////////////////////////////////
#endif	//__TCP_SERVER_H__