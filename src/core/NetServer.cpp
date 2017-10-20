#include <event.h>
#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/util.h>
#include <event2/listener.h>
#include <event2/thread.h>
#include <event2/event-config.h>
#include "NetServer.h"
#include <stdint.h>
#include <signal.h>
#include "PbBuf.h"
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
#include <Service.h>
#include <vector>
#include <NetManager.h>

static bool _init = false;
static bool _start = false;
static bool _stop = false;
static void getSockInfo(int fd, uint16_t& port, char* ip) {
#ifdef WIN32
	int
#else 
	socklen_t
#endif	
		len;
	struct sockaddr_in addr;
	len = sizeof(addr);
	getpeername(fd, (struct sockaddr *)&addr, &len);
	ip = inet_ntoa(addr.sin_addr);
	port = ntohs(addr.sin_port);
}
NetServer::~NetServer()
{
}
NetServer::NetServer(int type):m_recver(type)
{
	m_pNetMgr=NetManager::getInstance();
	m_type = type;
}

int NetServer::init(int port, const char * ip)
{
	m_port = port;

	strncpy(m_ip, ip, strlen(ip));
	Connection* conn = m_pNetMgr->listen(ip, port, m_type);
	if (conn) {
		conn->server = this;
		m_pNetMgr->addFileEvent(conn->fd, AE_ACCEPTABLE);
		return 0;
	}
	return -1;
}

int NetServer::prepare()
{
	m_recver.start();
	return 0;
}

void NetServer::start()
{
	if (_start) {
		return;
	}
	_start = true;
	m_pNetMgr->loop();
}

void NetServer::stop()
{
	if (_stop) {
		return;
	}
	_stop = true;
	m_pNetMgr->stop();
}

Connection * NetServer::connect(const char * ip, int port)
{
	Connection* conn = m_pNetMgr->connect(ip, port, m_type);
	if (conn) {
		conn->flag = conn_client;
		conn->server = this;
		m_pNetMgr->addFileEvent(conn->fd, AE_READABLE);
	}
	return conn;
}

void NetServer::accept(int listener)
{
	struct sockaddr_storage ss;
#ifdef WIN32
	int slen = sizeof(ss);
#else
	socklen_t slen = sizeof(ss);
#endif
	int fd = ::accept(listener, (struct sockaddr*)&ss, &slen);
	if (fd <= 0)
	{
		LogMsg(LOG_DEBUG, "accept fail");
		return;
	}
	else
	{
		LogMsg(LOG_DEBUG, "fd=%d connect", fd);
		if (0 == evutil_make_socket_nonblocking(fd))
		{
			struct event *pEvRead = new struct event;
			Connection* conn = new (std::nothrow)Connection;
			if (NULL == conn) {
				LogMsg(LOG_ERROR, "alloc memory fail");
				return;
			}
			conn->fd = fd;
			conn->flag = conn_server;
			conn->server = this;
			conn->setState(STATE_AUTH);
			getSockInfo(fd, conn->port,conn->ip);
			
			if (m_type == CONN_HTTP) {
				this->initContext(conn);
			}
			m_pNetMgr->addConnection(conn);

			m_pNetMgr->addFileEvent(fd, AE_READABLE);

		}
		else perror("nonblocking\n");
	}
}

void NetServer::close(int fd)
{
	m_pNetMgr->closeConnection(fd);
}

int NetServer::get_port()
{
	return m_port;
}
std::string NetServer::get_ip()
{
	return m_ip;
}

/*void NetServer::signal_cb(evutil_socket_t sig, short events, void *arg)
{
	NetServer* pThis = (NetServer*)(arg);
	if (pThis)
	{
		pThis->signal(sig, events, NULL);
	}
}*/

