#ifndef __THREAD_H__
#define __THREAD_H__
//////////////////////////////////////////////////////////////////////////
#include <mutex>
#include "LibSocket.h"

namespace t {
	//////////////////////////////////////////////////////////////////////////
	class Thread
	{
#if(defined(_WIN32) || defined(_WIN64))
		friend DWORD WINAPI ThreadFunc(LPVOID lparam);
#else
		friend	void* ThreadFunc(void* lparam);
#endif
	public:
	protected:
	private:
		ThreadHandle m_hThread;
		bool m_bRunning;
		std::mutex m_mutex;
	public:
		Thread();
		virtual ~Thread();
	public:
		bool Create(size_t stackSize = 0);
		int Join();
		void Stop() { m_mutex.lock(); m_bRunning = false; m_mutex.unlock(); }
	protected:
		bool Initialize();
		int Run();
		int Finalize();
	protected:
		inline bool isRunning() { m_mutex.lock(); bool bRun = m_bRunning; m_mutex.unlock(); return bRun; }
	protected:
		virtual bool onCreating() { return true; }
		virtual void onCreated() { ; }
		virtual bool onInitialize() { return true; }
		virtual void onRun() { ; }
		virtual int onFinalize() { return 0; }
	};
	//////////////////////////////////////////////////////////////////////////
};
//////////////////////////////////////////////////////////////////////////
#endif	//__THREAD_H__