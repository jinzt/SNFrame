#include "NetManager.h"
#include <Log.h>
#include <LibEvent.h>

#ifndef WIN32
#include <unistd.h>
#endif


NetManager::NetManager()
{
	m_pNet = new LibEvent;
	m_stop = false;
}

NetManager * NetManager::getInstance()
{
	static NetManager instance;
	return &instance;
}

Connection* NetManager::connect(const char * ip, int port,int type)
{
	int fd = m_pNet->createClient(ip,port,type);
	Connection* conn = NULL;
	if (fd >0 ) {
		conn = new Connection();
		if (conn == NULL) {
			LogMsg(LOG_ERROR, "create connection error,alloc memory fail");
			return NULL;
		}
		conn->type = type;
		conn->fd=fd;
		m_conns.push_back(conn);
		
	}
	return conn;
}

Connection* NetManager::listen(const char * ip, int port, int type)
{
	int fd = m_pNet->createServer(ip, port, type);
	Connection* conn = NULL;
	if (fd >0) {
		 conn= new Connection();
		if (conn == NULL) {
			LogMsg(LOG_ERROR, "create connection error,alloc memory fail");
			return NULL;
		}
		conn->type = type;
		conn->fd=fd;
		m_conns.push_back(conn);
	}
	return conn;
}

int NetManager::addConnection(Connection * conn)
{
	if (!conn) {
		return -1;
	}
	m_conns.push_back(conn);
	return 0;
}

int NetManager::closeConnection(int fd)
{
	LogMsg(LOG_DEBUG, "close connection");
	CAutoRWLock lock(&m_lock, 'w');
	std::list<Connection*>::iterator it = m_conns.begin();
	while (it != m_conns.end()) {
		if ((*it)->fd == fd) {
			delete *it;
			m_conns.erase(it);
			break;
		}
		++it;
	}
	return 0;
}

void NetManager::addFileEvent(int fd, int mask)
{
	std::list<Connection*>::iterator it = m_conns.begin();
	while (it != m_conns.end()) {
		if ((*it)->fd == fd) {
			if ((*it)->evs & mask) {
				break;
			}
			(*it)->evs |= mask;
			m_pNet->addFileEvent((*it), mask);
			break;
		}
		++it;
	}
	return ;
}

void NetManager::delFileEvent(int fd, int mask)
{
	 CAutoRWLock lock(&m_lock, 'w');
	std::list<Connection*>::iterator it = m_conns.begin();
	while (it != m_conns.end()) {
		if ((*it)->fd == fd) {
			int ev = (*it)->evs & mask;
			if (ev) {
				(*it)->evs = (*it)->evs &(~mask);
				m_pNet->delFileEvent((*it), ev);
			}
			break;
		}
		++it;
	}
	return ;
}

void NetManager::addTimer(timer_handler cb, int sec)
{
	m_pNet->addTimer(cb, sec);
}

int NetManager::sendSock(msg_t& msg,int fd)
{
	CAutoRWLock lock(&m_lock, 'w');
	std::list<Connection*>::iterator it = m_conns.begin();
	while (it != m_conns.end()) {
		if ((*it)->fd == fd) {
			(*it)->push(msg);
			if ((*it)->state == STATE_CONNECTED) {
				addFileEvent((*it)->fd, AE_WRITABLE);
			}
			return 0;
		}
		++it;
	}
	LogMsg(LOG_ERROR,"not find fd(%d)",fd);
	return -1;
}

int NetManager::sendNode(msg_t & msg, uint32_t nid)
{
	CAutoRWLock lock(&m_lock, 'w');
	std::list<Connection*>::iterator it = m_conns.begin();
	while (it != m_conns.end()) {
		if ((*it)->nid == nid) {
			(*it)->push(msg);
			if ((*it)->state == STATE_CONNECTED || (*it)->state ==STATE_AUTH) {
				addFileEvent((*it)->fd, AE_WRITABLE);
			}
			return 0;
		}
		++it;
	}
	LogMsg(LOG_ERROR,"not find node(%u)",nid);
	return -1;
}

int NetManager::sendDb(msg_t & msg)
{
	CAutoRWLock lock(&m_lock, 'w');
	std::list<Connection*>::iterator it = m_conns.begin();
	while (it != m_conns.end()) {
		if ((*it)->sid == IM::BaseDefine::ServiceID::SID_DB_PROXY) {
			(*it)->push(msg);
			if ((*it)->state == STATE_CONNECTED  || (*it)->state ==STATE_AUTH ) {
				addFileEvent((*it)->fd, AE_WRITABLE);
			}
			return 0;
		}
		++it;
	}
	LogMsg(LOG_ERROR, "not find db node");
	return -1;
}
int NetManager::sendState(msg_t & msg)
{
	CAutoRWLock lock(&m_lock, 'w');
	std::list<Connection*>::iterator it = m_conns.begin();
	while (it != m_conns.end()) {
		if ((*it)->sid == IM::BaseDefine::ServiceID::SID_USER_STATE) {
			(*it)->push(msg);
			if ((*it)->state == STATE_CONNECTED || (*it)->state == STATE_AUTH) {
				addFileEvent((*it)->fd, AE_WRITABLE);
			}
			return 0;
		}
		++it;
	}
	LogMsg(LOG_ERROR, "not find db node");
	return -1;
}

void NetManager::loop()
{
	struct timeval  tv;
	tv.tv_sec = 0;
	tv.tv_usec = 200;
	while (!m_stop) {
		m_pNet->loop(&tv);
	}
}

void NetManager::stop() {
	LogMsg(LOG_DEBUG, "will exit process");
	m_stop=true;
	m_pNet->stop();
}
