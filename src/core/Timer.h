#ifndef _TIMER_H
#define _TIMER_H
#include <NetManager.h>
#include <Lock.h>
#include <list>
typedef struct timer_s {
	timer_handler  cb;
	int            sec;
}tm_t;
class Timer {
public:
public:
	static Timer* getInstance();
	~Timer();
	void startTimer();
	void addTimer(timer_handler  cb, int sec);
private:
	static void serverCron();
	Timer();
private:
	static std::list<tm_t*>     m_timers;
	NetManager*             m_pNetMgr;
	
};
#endif
