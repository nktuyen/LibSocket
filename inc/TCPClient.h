#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__
//////////////////////////////////////////////////////////////////////////
#include "../LibSocket/inc/Thread.h"
#include "../LibSocket/inc/Socket.h"

namespace t{
//////////////////////////////////////////////////////////////////////////

class TCPClient : public t::Thread
{
public:
protected:
private:
	t::Socket mSocket;
	std::mutex mLocker;
	char mServerAddr[MAX_IP_ADDR_LEN];
	unsigned short mServerPort;
public:
	TCPClient(char* serverIP, unsigned short serverPort);
	virtual ~TCPClient();
	int Send(char* buffer, int len);
protected:
	bool onInitialize();
	void onRun();
	int onFinalize();
	void onDisconnected();
private:
	void Dump(char* data, int len);
};
//////////////////////////////////////////////////////////////////////////
};	//namespace
//////////////////////////////////////////////////////////////////////////
#endif // !__TCP_CLIENT_H__