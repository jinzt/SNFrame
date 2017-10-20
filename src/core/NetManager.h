#ifndef _IMEVENT_H_
#define _IMEVENT_H_

#include <time.h>
#include <NetLibBase.h>
#include <list>

class NetManager {
public:
	static NetManager* getInstance();
	Connection* connect(const char* ip,int port,int type);
	Connection* listen(const char* ip, int port, int type);
	int addConnection(Connection* conn);
	int closeConnection(int fd);
	
	int sendSock(msg_t & msg, int fd);
	int sendNode(msg_t& msg, uint32_t nid);
	int sendDb(msg_t& msg);
	
	void addFileEvent(int fd, int mask);
	void delFileEvent(int fd, int mask);
	int sendState(msg_t& msg);
	void addTimer(timer_handler cb, int sec);

	void  loop();
	void stop();

private:
	NetManager();
	
private:
	bool                           m_stop;
	std::list<Connection*>         m_conns;
	NetLibBase*                    m_pNet;
	CRWLock                        m_lock;


};
#endif
