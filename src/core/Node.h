#ifndef _NODE_H
#define _NODE_H
#include <Define.h>
enum NodeState {
	NODE_UNABLE,
	NODE_AUTH,
	NODE_ABLE,
	NODE_ERROR,
	NODE_CLOSED,
};
class Node {
public:
	Node(uint16_t sid, uint32_t nid);
	uint32_t getNodeId();
	int getState();
	uint16_t getSid();
	void setState(int state);
	void setType(uint16_t type);
	uint16_t getType();
	int  setAddrs(const char* ip, uint16_t port);
	void setContext(void* context);
	void* getContext();
	char* getIp();
	uint16_t getPort();
	bool  needReconnect();
	void  setPersistConnect(bool persist);
	void clear();
private:
	int        m_num;  //connect fail num
	uint32_t   m_id;
	char       m_ip[16];
	uint16_t   m_port;
	int        m_state;
	void*      m_context;
	uint16_t   m_type;  //service type
	bool       m_persist;//是否持续重连

};
#endif
