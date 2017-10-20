#include "Timer.h"
#include <Log.h>
#ifndef WIN32
#include <unistd.h>
#endif
#define run_with_period(_ms_) if (!(m_cronloops%((_ms_)/1000)))

static CRWLock                 m_lock;
std::list<tm_t*>  Timer::m_timers;
static int _init=0;
Timer::Timer()
{
	m_pNetMgr = NetManager::getInstance();
}

Timer * Timer::getInstance()
{
	static Timer instance;
	return &instance;
}

Timer::~Timer()
{
	CAutoRWLock lock(&m_lock, 'w');
	std::list<tm_t*>::iterator it = m_timers.begin();
	while (it != m_timers.end()) {
		delete *it;
		m_timers.erase(it++);
	}
}

void Timer::startTimer()
{
	m_pNetMgr->addTimer(serverCron,1);
}

void Timer::addTimer(timer_handler cb, int sec)
{
	tm_t* tm = new tm_t;
	if (tm == NULL) {
		return;
	}
	tm->cb = cb;
	tm->sec = sec;
	CAutoRWLock lock(&m_lock, 'w');
	m_timers.push_back(tm);
}

void Timer::serverCron()
{
	CAutoRWLock lock(&m_lock, 'r');
	if(!_init){
#ifdef WIN32
	Sleep(2*1000);
#else
	sleep(2);
#endif
	_init=1;
		
	}
	std::list<tm_t*>::iterator it = m_timers.begin();
	while (it != m_timers.end()) {
		(*it)->cb();
		++it;
	}
}
