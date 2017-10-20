#pragma once
#include "NetLibBase.h"
#include <Connection.h>

class LibEvent :public NetLibBase {
public:
	LibEvent();
	~LibEvent();
	int init();
	int  createClient(const char * ip, int port, int type);
	int createServer(const char * ip, int port, int type);
	int addFileEvent(Connection * conn, int mask);
	int delFileEvent(Connection * conn,int mask);
	void addTimer(timer_handler cb, int sec);
	static void do_timeout(evutil_socket_t fd, short event, void * arg);
	int loop(const timeval * tv);
	int stop();
private:
	int tcpConnect(const char * ip, int port);

private:
	struct event_base* m_base;
	bool               m_stop;
	static std::map<int, timer_handler> m_timer;
	int                m_id;
};