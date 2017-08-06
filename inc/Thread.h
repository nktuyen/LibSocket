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
		bool m_bInitDone;
		std::mutex m_mutexRun;
		std::mutex m_mutexInit;
	public:
		Thread();
		virtual ~Thread();
	public:
		bool Create(size_t stackSize = 0);
		int Join();
		void Stop() { m_mutexRun.lock(); m_bRunning = false; m_mutexRun.unlock(); }
	protected:
		bool Initialize();
		int Run();
		int Finalize();
	protected:
		inline bool isRunning() { m_mutexRun.lock(); bool bRun = m_bRunning; m_mutexRun.unlock(); return bRun; }
	protected:
		virtual bool onCreating() { return true; }
		virtual void onCreated() { ; }
		virtual bool onInitialize() { return true; }
		virtual void onRun() { ; }
		virtual int onFinalize() { return 0; }
	private:
		inline void setRunning(bool bRun) { m_mutexRun.lock(); m_bRunning = bRun; m_mutexRun.unlock(); }
		inline bool isInitDone() {m_mutexInit.lock(); bool bDone = m_bInitDone; m_mutexInit.unlock(); return bDone; }
		inline void setInitDone(bool bDone) { m_mutexInit.lock(); m_bInitDone = bDone; m_mutexInit.unlock(); }
	};
	//////////////////////////////////////////////////////////////////////////
};
//////////////////////////////////////////////////////////////////////////
#endif	//__THREAD_H__