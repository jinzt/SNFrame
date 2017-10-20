#pragma once
#include <ServiceNode.h>

class RouterNode : public ServiceNode {
public:
	RouterNode();
	void parse(msg_t & msg);
	int init();
	int registReq();
	int registRsp(msg_t & msg);
	bool srvinfoBroadcast(msg_t & msg);
	
	void set_id(uint32_t nid);

private:
	int                            m_id;         //cur server node id;
	char*                          m_routerIP;
	uint16_t                       m_routerPort;
	IM::BaseDefine::ServiceID      m_type;

};