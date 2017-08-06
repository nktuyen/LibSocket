#ifndef __SOCKET_OPTIONS_H__
#define __SOCKET_OPTIONS_H__
//////////////////////////////////////////////////////////////////////////
#include <map>
#include "SocketOption.h"

namespace t{
//////////////////////////////////////////////////////////////////////////

class SocketOptions
{
	friend class Socket;
private:
	std::map<ESocketOption, SocketOption*> mOptions;
	typedef std::map<ESocketOption, SocketOption*>::iterator SocketOptionIter;
	SocketHandle mSock;
private:
	SocketOptions(SocketHandle sock = InvalidSocket);
	virtual ~SocketOptions(void);
private:
	inline SocketHandle getSocketHandle() { return mSock; }
private:
	void setSocketHandle(SocketHandle sock);
	void removeAllOptions();
public:
	const SocketOption* getOption(ESocketOption optName);
};
//////////////////////////////////////////////////////////////////////////
};
//////////////////////////////////////////////////////////////////////////
#endif	//__SOCKET_OPTIONS_H__