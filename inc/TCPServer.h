#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__
//////////////////////////////////////////////////////////////////////////
#include <map>
#include "../LibSocket/inc/Thread.h"
#include "../LibSocket/inc/Socket.h"
#include "TCPConnection.h"

class TCPServer : public t::Thread
{
public:
protected:
private:
	t::Socket mSocket;
	std::map<t::Socket*,TCPConnection*> mConnections;
	typedef std::map<t::Socket*,TCPConnection*>::iterator ConnectionIter;
public:
	TCPServer();
	virtual ~TCPServer();
protected:
	bool onInitialize();
	void onRun();
	int onFinalize();
private:
	void Dump(char* data, int len);
	void closeAllConnection();
};

//////////////////////////////////////////////////////////////////////////
#endif	//__TCP_SERVER_H__