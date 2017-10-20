#ifndef _SENDER_H
#define _SENDER_H
#include <PbBuf.h>

class NetManager;
class Sender {
public:
	Sender();
	~Sender();
	int sendBlock(msg_t& msg, int fd);
	int sendNode(msg_t& msg, uint32_t nid);
	int sendSock(msg_t& msg,int fd);
	int sendDb(msg_t& msg);
	int sendState(msg_t& msg);
private:
	NetManager*       m_pNetMgr;
	
	
};
#endif
