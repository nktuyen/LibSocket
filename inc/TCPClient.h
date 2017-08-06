#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__
//////////////////////////////////////////////////////////////////////////
#include "../LibSocket/inc/Thread.h"
#include "../LibSocket/inc/Socket.h"

#define MAX_IP_ADDR_LEN	64

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
	TCPClient(char* ip, unsigned short port);
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
#endif // !__TCP_CLIENT_H__