
#ifndef _THREAD_BASE_H_
#define _THREAD_BASE_H_

#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif

#ifdef WIN32
#define  thread_t  DWORD  
#define create_thread(id,attr,handler,arg)         CreateThread(NULL, 0, handler, arg, 0, &id)
#define exit_thread(v)                              ExitThread((v))  
#define thread_id()                             GetCurrentThreadId()
#else
#define  thread_t  pthread_t  
#define create_thread(id,attr,handler,arg)         pthread_create(&id, attr, handler, arg)
#define exit_thread(v)                             pthread_exit(&(v))
#define thread_id()                            pthread_self()
#endif

#define STARTED   0
#define STOPED    1

class ThreadBase
{
public:
	ThreadBase();
	virtual ~ThreadBase()
	{
		/*#ifdef WIN32
		WaitForSingleObject(m_thread_id, INFINITE);
		#else
		pthread_join(m_thread_id,NULL);
		#endif*/
	}

#ifdef WIN32
	static DWORD WINAPI
#else
	static void*
#endif
		threadProc(void* arg);
	virtual void stopThread();
	virtual void startThread();
	virtual void startThread(int num);
	virtual void run(void) = 0;
private:
	void setThreadSate(int state);
	int getThreadSate();
protected:
	thread_t m_thread_id;
	int      m_state;
};

#endif