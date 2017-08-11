#ifndef __TCP_CONNECTION_H__
#define __TCP_CONNECTION_H__
//////////////////////////////////////////////////////////////////////////
#include "Socket.h"
#include "Thread.h"

class TCPConnection : public t::Thread
{
public:
protected:
private:
	t::Socket* mSocket;
public:
	TCPConnection(t::Socket* pSocket);
virtual ~TCPConnection();
private:
	bool onInitialize();
	void onRun();
	int onFinalize();
	void Dump(char* data, int len);
};
//////////////////////////////////////////////////////////////////////////
#endif	//__TCP_CONNECTION_H__