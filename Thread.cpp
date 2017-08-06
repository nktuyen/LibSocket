#include <map>
#include "inc/Thread.h"


#if(defined(_WIN32) || defined(_WIN64))
#pragma comment(lib, "kernel32.lib")
#endif

namespace t {
//////////////////////////////////////////////////////////////////////////
extern std::map<ThreadHandle, Thread*>					gThread;
extern std::map<ThreadHandle, Thread*>::iterator		gThreadIte;
extern std::map<ThreadHandle, Thread*>::const_iterator	gThreadCIte;

#if(defined(_WIN32) || defined(_WIN64))
DWORD WINAPI ThreadFunc(LPVOID lparam)
#else
void* ThreadFunc(void* lparam)
#endif
{
	Thread* pThread = (Thread*)lparam;
	int res = 0;
	if (nullptr != pThread) {
		if (!pThread->Initialize()) {
			return pThread->Finalize();
		}
		else {
			return pThread->Run();
		}
	}
	return res;
}

Thread::Thread()
{
	m_hThread = InvalidThread;
	m_bInitDone = false;
}


Thread::~Thread()
{
}

bool Thread::Create(size_t stackSize /* = 0 */)
{
	if (!this->onCreating()) {
		return false;
	}

#if(defined(_WIN32) || defined(_WIN64))
	DWORD dwID = 0;

	m_bInitDone = false;
	m_hThread = CreateThread(nullptr, stackSize, (LPTHREAD_START_ROUTINE)ThreadFunc, this, CREATE_SUSPENDED, &dwID);
#else
	m_hThread = new ThreadObject();
	pthread_attr_t	attr = { 0 };
	int res = pthread_attr_init(&attr);
	res = pthread_attr_setstacksize(&attr, stackSize);
	res = pthread_create(m_hThread, &attr, ThreadFunc, this);
	if (res != 0) { //Failed
		CloseThread(m_hThread);
	}
	res = pthread_attr_destroy(&attr);
#endif
	
	if (InvalidThread == m_hThread) {
		return false;
	}
	else {
#if(defined(_WIN32) || defined(_WIN64))
		ResumeThread(m_hThread);
#else
#endif
		while (!isInitDone()){;}
		return true;
	}
}

bool Thread::Initialize()
{
	if (!this->onInitialize()) {
		setInitDone(true);
		return false;
	}

	setInitDone(true);
	return true;
}

int Thread::Run()
{
	setRunning(true);

	this->onRun();
	return Finalize();
}

int Thread::Finalize()
{
	setRunning(false);

	int res = this->onFinalize();
	CloseThread(m_hThread);

	return res;
}

int Thread::Join()
{
	if (InvalidThread == m_hThread) {
		return 0;
	}

#if(defined(_WIN32) || defined(_WIN64))
	DWORD dwWait = WaitForSingleObject(m_hThread, INFINITE);
	return dwWait;
#else
	int res = 0;
	void* pRes = &res;
	int err = pthread_join(*m_hThread, &pRes);
	return res;
#endif
}
//////////////////////////////////////////////////////////////////////////
};