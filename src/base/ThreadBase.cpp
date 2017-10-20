#include "ThreadBase.h"

ThreadBase::ThreadBase()
{
	m_state = -1;
	m_thread_id = 0;
}

#ifdef WIN32
DWORD WINAPI
#else
void*
#endif 
ThreadBase::threadProc(void * arg)
{
	ThreadBase* pThread = (ThreadBase*)arg;
	if (pThread)
		pThread->run();
	return NULL;
}

void ThreadBase::startThread()
{
	if (getThreadSate() == STARTED) {
		return;
	}
	setThreadSate(STARTED);
	create_thread(m_thread_id, NULL, threadProc, this);
}

void ThreadBase::startThread(int num)
{
	for (int i = 0; i < num; ++i) {
		create_thread(m_thread_id, NULL, threadProc, this);
	}
}

void ThreadBase::setThreadSate(int state)
{
	m_state = state;
}

int ThreadBase::getThreadSate()
{
	return m_state;
}

void ThreadBase::stopThread()
{
#ifdef WIN32
	DWORD code = 0;
#else
	int code;
#endif
	exit_thread(code);
}
