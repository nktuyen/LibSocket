#ifndef __SOCKET_OPTION_H__
#define __SOCKET_OPTION_H__
//////////////////////////////////////////////////////////////////////////

#include "LibSocket.h"

namespace t{
//////////////////////////////////////////////////////////////////////////
	class SocketOption
	{
		friend class SocketOptions;
	public:
		inline ESocketOption getName() { return mName; }
	protected:
		virtual int getInt(int def = 0) {;}
		virtual unsigned int getUInt(unsigned int def = 0) { ; }
		virtual const char* getCharPtr(const char* def = nullptr) { ; }
		virtual bool getBool(bool def = false) { ; }
		virtual bool setInt(int value) { ; }
		virtual bool setUInt(unsigned int value) { ; }
		virtual bool setCharPtr(const char* value) { ; }
		virtual bool setBool(bool value) { ; }
	protected:
		inline void setSocketHandle(SocketHandle sock) { mSock = sock; }
		inline SocketHandle getSocketHandle() { return mSock; }
	protected:
		SocketOption(SocketHandle sock, ESocketOption optionName) {
			mName  = optionName;
			mSock  = sock;
		}
		virtual ~SocketOption() { ; }
	private:
		ESocketOption mName;
		SocketHandle mSock;
	};

	class SocketOptionInt : public SocketOption
	{
		friend class SocketOptions;
	protected:
		SocketOptionInt(SocketHandle sock, ESocketOption optionName) 
			: SocketOption(sock, optionName)
		{

		}

		virtual ~SocketOptionInt() 
		{

		}

	public:
		int getInt(int def = 0) {
			char value[255] = {0};
			socklen_t len = sizeof(value);
			int res = def;
			if(InvalidSocket != getSocketHandle()) {
				int err = getsockopt(getSocketHandle(), SOL_SOCKET, static_cast<int>(getName()), value, &len );
				if(SocketError !=  err) {
					memcpy(&res, &value, sizeof(int));
				}
			}

			return res;
		}
	};

	class SocketOptionBool : public SocketOption
	{
		friend class SocketOptions;
	public:
		bool getBool(bool def /* = false */) {
			char value[255] = {0};
			socklen_t len = sizeof(value);
			bool res = def;
			if(InvalidSocket != getSocketHandle()) {
				int err = getsockopt(getSocketHandle(), SOL_SOCKET, static_cast<int>(getName()), value, &len );
				if(SocketError != err) {
					res = value[0]==1?true:false;
				}
			}

			return res;
		}
	};
//////////////////////////////////////////////////////////////////////////
};	//namespace
//////////////////////////////////////////////////////////////////////////
#endif	//__SOCKET_OPTION_H__