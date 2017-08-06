#include "inc/SocketOptions.h"
#include "inc/SocketOption.h"

namespace t{
//////////////////////////////////////////////////////////////////////////

SocketOptions::SocketOptions(SocketHandle sock)
{
	mSock = sock;


}

SocketOptions::~SocketOptions(void)
{
	removeAllOptions();
}

void SocketOptions::removeAllOptions()
{
	if(mOptions.size() > 0) {
		SocketOptionIter ite = mOptions.begin();
		SocketOption* pOption = nullptr;
		for(;ite != mOptions.end();++ite) {
			pOption = ite->second;
			if(nullptr != pOption) {
				delete pOption;
				pOption = nullptr;
			}
		}
		mOptions.clear();
	}
}

void SocketOptions::setSocketHandle(SocketHandle sock)
{
	mSock = sock;
	if(mOptions.size() > 0) {
		SocketOptionIter ite = mOptions.begin();
		SocketOption* pOption = nullptr;
		for(;ite != mOptions.end();++ite) {
			pOption = ite->second;
			if(nullptr != pOption) {
				pOption->setSocketHandle(sock);
				pOption = nullptr;
			}
		}
	}
}

const SocketOption* SocketOptions::getOption(ESocketOption optName)
{
	SocketOptionIter ite = mOptions.find(optName);
	if(mOptions.end() == ite) {
		return (const SocketOption*)nullptr;
	}
	else {
		const SocketOption* option = ite->second;
		return option;
	}
}
//////////////////////////////////////////////////////////////////////////
};