#include <event.h>
#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/util.h>
#include <event2/listener.h>
#include <event2/thread.h>
#include <event2/event-config.h>
#include <stdint.h>
#include <signal.h>
#include <Log.h>
#include <Netutil.h>
#ifdef WIN32
#pragma warning(disable: 4996)
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#endif
#include <vector>
#include "LibEvent.h"
#include <NetServer.h>

std::map<int, timer_handler> LibEvent::m_timer;

static void _accept(evutil_socket_t listener, short events, void *arg) {
	if (arg == NULL) {
		return;
	}
	LogMsg(LOG_DEBUG, "accept event");
	Connection* conn = (Connection*)arg;
	NetServer* pNet = (NetServer*)(conn->server);
	if (pNet) {
		pNet->accept(conn->fd);
	}
}

static void _read(int fd, short events, void *arg) {
	if (arg == NULL) {
		return;
	}
//	LogMsg(LOG_DEBUG,"read event");
	Connection* conn = (Connection*)arg;
	NetServer* pNet = (NetServer*)(conn->server);
	if (pNet) {
		pNet->read(conn);
	}
	
}

static void _write(int fd, short events, void *arg) {
	if (arg == NULL) {
		return;
	}
	Connection* conn = (Connection*)arg;
	NetServer* pNet = (NetServer*)(conn->server);
	if (pNet) {
		pNet->write(conn);
	}
}

static void  _signal(evutil_socket_t sig, short events, void *arg) {

}

LibEvent::LibEvent()
{
	m_id = 0;
	m_base = NULL;
	m_stop = true;
	evthread_enable_lock_debuging();
	init();
}

LibEvent::~LibEvent()
{
	event_base_free(m_base);
#ifdef WIN32
	WSACleanup();
#endif
}

int LibEvent::init()
{
#ifdef WIN32
	WSADATA		wsaData;
	DWORD		Ret;
	if ((Ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		printf("WSAStartBug\n");
	}
	evthread_use_windows_threads();
#else
	evthread_use_pthreads();
#endif
	m_base = event_base_new();
	return 0;
}

int  LibEvent::createClient(const char * ip, int port, int type)
{
	int sock = -1;
	
	if (type == CONN_TCP) {
		sock = tcpConnect(ip, port);
	}
	return sock;
}

int LibEvent::createServer(const char * ip, int port, int type)
{
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	if (ip == NULL)
	{
		sin.sin_addr.s_addr = 0;
	}
	else
	{
		sin.sin_addr.s_addr = inet_addr(ip);
	}
	sin.sin_port = htons(port);
	int listener = socket(AF_INET, SOCK_STREAM, 0);
	if (listener == -1) {
		LogMsg(LOG_ERROR, "create listen socket fail");
		return -1;
	}
	int on = 1;

#ifdef WIN32
	if (-1 == setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on)))
#else
	if (-1 == setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)))
#endif
	{
		LogMsg(LOG_ERROR, "setsockopt SO_REUSEADDR:%s", strerror(errno));
		return -1;
}
	if (::bind(listener, (struct sockaddr*)&sin, sizeof(sin)) < 0)
	{
		perror("bind");
		return -1;
	}
	if (listen(listener, SOMAXCONN) < 0)
	{
		perror("listen");
		return -1;
	}
	evutil_make_socket_nonblocking(listener);
	return listener;
	if (m_base)
	{
#ifndef WIN32
		//信号
		/*	static struct event signal_usr;
		event_assign(&signal_usr, m_base, SIGHUP, EV_SIGNAL | EV_PERSIST, signal_cb, NULL);
		event_base_set(m_base, &signal_usr);
		event_add(&signal_usr, NULL);*/
#endif
		//关闭程序信号
		static struct event signal_inti;
		event_assign(&signal_inti, m_base, SIGINT, EV_SIGNAL | EV_PERSIST, _signal, (void *)this);
		event_base_set(m_base, &signal_inti);
		event_add(&signal_inti, NULL);
	}
	else perror("event_new");
	return 0;
}

int LibEvent::addFileEvent(Connection* conn,int mask)
{
	struct event* pEv;
	if (mask & AE_READABLE) {
		pEv = conn->revent;
		memset(pEv,0, sizeof(struct event));
		event_set(pEv, conn->fd, EV_READ | EV_PERSIST, _read, (void*)conn);
	}
	else if (mask & AE_WRITABLE) {
		pEv = conn->wevent;
		memset(pEv, 0, sizeof(struct event));
		event_set(pEv, conn->fd, EV_WRITE | EV_PERSIST, _write, (void*)conn);
	}
	else if (mask & AE_ACCEPTABLE) {
		pEv = conn->revent;
		memset(pEv, 0, sizeof(struct event));
		event_set(pEv, conn->fd, EV_READ | EV_PERSIST, _accept, (void*)conn);
	}
	else {
		return -1;
	}
	event_base_set(m_base, pEv);
	event_add(pEv, NULL);
	return 0;

}

int LibEvent::delFileEvent(Connection * conn, int mask)
{
	if (mask & AE_READABLE) {
		event_del(conn->revent);
	}
	if (mask & AE_WRITABLE) {
		event_del(conn->wevent);
	}
	return 0;
}

void LibEvent::addTimer(timer_handler cb, int sec)
{
	m_id++;
	m_timer[m_id] = cb;
	struct event *timeout = new struct event;//暂时没有删除定时事件
	struct timeval tv;
	event_assign(timeout, m_base, -1, EV_PERSIST, do_timeout, (void*)m_id);
	evutil_timerclear(&tv);
	tv.tv_sec = sec;
	event_add(timeout, &tv);
}

void LibEvent::do_timeout(evutil_socket_t fd, short event, void * arg)
{
	int timer_id = reinterpret_cast<intptr_t>(arg);
	std::map<int, timer_handler>::iterator it = m_timer.find(timer_id);
	if (it != m_timer.end()) {
		it->second();
	}
}

int LibEvent::loop(const struct timeval *tv)
{
//	 event_base_loopexit(m_base,tv);
	 event_base_dispatch(m_base);
	 return 0;
}

int LibEvent::stop()
{
	event_base_loopbreak(m_base);
	return 0;
}

int  LibEvent::tcpConnect(const char * ip, int port)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		LogMsg(LOG_ERROR, "create socket fail");
		return -1;
	}
	struct sockaddr_in srvAddr;
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_addr.s_addr = inet_addr(ip);
	srvAddr.sin_port = htons(port);
	int ret = ::connect(sock, (sockaddr*)&srvAddr, sizeof(struct sockaddr_in));
	if (0 == ret)
	{
		if (0 == evutil_make_socket_nonblocking(sock))
		{
			return sock;
		}
		else {
			LogMsg(LOG_ERROR, "set nonblocking fail");
			return -1;
		}
	}
	LogMsg(LOG_ERROR, "connect[ip:%s,port:%d] fail,%s", ip, port, strerror(errno));
	return -1;
}

