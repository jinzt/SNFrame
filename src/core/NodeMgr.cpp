#include "NodeMgr.h"
#include <NetServer.h>
#include <ConfigFileReader.h>
#include <Common.h>
#include <Service.h>
#include <Timer.h>
#define  MAX_SERVICE_TYPE        100

void parseCommonProto(msg_t& msg,Connection* conn) {
	LogMsg(LOG_DEBUG, "recv common proto msg");
	uint16_t sid = get_sid(msg);
	NodeMgr* pNodeMgr= NodeMgr::getInstance();
	if (sid == IM::BaseDefine::ServiceID::SID_OTHER) {
		pNodeMgr->parse(msg,conn);
	}
	else {
		ServiceNode* snode = pNodeMgr->getServiceNode(sid);
		if (snode) {
			snode->parse(msg);
		}
		else {
			LogMsg(LOG_ERROR, "service type(%u) not regist", sid);
		}
	}
}

NodeMgr::NodeMgr()
{
	m_pNet = NULL;
	m_num = 0;
}
NodeMgr * NodeMgr::getInstance()
{
	static NodeMgr instance;
	return &instance;
}

int NodeMgr::init(NetServer* net)
{
	m_pNet = net;
	char* services = CConfigFileReader::getInstance()->getConfigName("service_type");
	if (!services) {
		LogMsg(LOG_ERROR, "no service node ");
		return -1;
	}
	char* p = services;
	int i = 0;
	char* type[MAX_SERVICE_TYPE] = { NULL };
	type[i++] = services;
	while (*p && i<MAX_SERVICE_TYPE) {
		if (*p == ' ') {
			*p = '\0';
			while (*(++p) == ' ');
			type[i++] = p;
		}
		++p;
	}
	m_num=i;
	m_serviceNode=(ServiceNode**)malloc(m_num*sizeof(ServiceNode*));	
	ServiceNode* snode = NULL;
	for (int i = 0; i < m_num; ++i) {
		snode = CreateServiceNode(atoi(type[i]));
		if(snode){
	            m_serviceNode[i]=snode;
		    snode->init(this, net);
		}
	}
	return 0;
}

bool NodeMgr::parse(msg_t & msg,Connection* conn)
{
	uint16_t cid = get_cmd(msg);
	switch (cid) {
	case IM::BaseDefine::CID_OTHER_AUTHENTICATION_REQ:
		AuthenticationReq(msg,conn);
		break;
	case IM::BaseDefine::CID_OTHER_AUTHENTICATION_RSP:
		AuthenticationRsp(msg,conn);
		break;
	case IM::BaseDefine::CID_OTHER_HEARTBEAT:
		break;
	default:
		LogMsg(LOG_ERROR, "unknow cmd id(%u)", cid);
		break;
	}
	return false;
}

bool NodeMgr::AuthenticationReq(msg_t & msg,Connection* conn)
{
	::IM::Server::IMAuthenticationReq req;
	CHECK_PB_PARSE_MSG(req.ParseFromArray(get_data(msg), data_len(msg)));
	uint16_t sid = req.type();
	uint32_t nid = req.node_id();
	conn->setNodeInfo(sid, nid);
	conn->setState(STATE_CONNECTED);
		
	::IM::Server::IMAuthenticationRsp rsp;
	rsp.set_node_id(nid);
	rsp.set_type(req.type());
	rsp.set_result(1);

	msg_t auth_msg;
	if (!bufalloc(auth_msg, rsp.ByteSize() + 27)) {
		LogMsg(LOG_ERROR, "alloc memory fail");
		return IM_ERR;
	}
	set_tag(auth_msg, 0, 0);
	set_body_head(auth_msg, 1, IM::BaseDefine::ServiceID::SID_OTHER, IM::BaseDefine::CID_OTHER_AUTHENTICATION_RSP);
	set_pb(auth_msg, &rsp);
	sendSock(auth_msg, conn->fd);
	return false;
}

bool NodeMgr::AuthenticationRsp(msg_t & msg,Connection* conn)
{
	::IM::Server::IMAuthenticationRsp rsp;
	CHECK_PB_PARSE_MSG(rsp.ParseFromArray(get_data(msg), data_len(msg)));
	uint16_t sid = rsp.type();
	uint32_t nid = rsp.node_id();
	Node* pNode = findNode(sid, nid);
	if (pNode) {
		pNode->setState(NODE_ABLE);
		conn->setState(STATE_CONNECTED);
	}
	return false;
}

void NodeMgr::notifyApp(msg_t & msg,int id)
{
	m_pNet->handler(msg,id);
}

Node* NodeMgr::addNode(uint32_t sid, uint32_t nid)
{
	ServiceNode* snode;
	if (NULL == (snode=getServiceNode(sid))) {
		LogMsg(LOG_WARN, "sid(%u) not regist", sid);
		return NULL;
	}
	return snode->addNode(sid, nid);
}

ServiceNode * NodeMgr::getServiceNode(uint32_t sid)
{
	for (int i = 0; i < m_num; i++) {
		if (m_serviceNode[i] && m_serviceNode[i]->getServiceId() == sid) {
			return m_serviceNode[i];
		}
	}
	LogMsg(LOG_DEBUG, "not regist sid(%u)", sid);
	return NULL;
}

void NodeMgr::start()
{
	Timer::getInstance()->addTimer(timer, 1);
}

void NodeMgr::exit()
{
	m_pNet->stop();
}

void NodeMgr::cron()
{
	for (int i = 0; i < m_num; i++) {
		if (m_serviceNode[i] ) {
			m_serviceNode[i]->checkState();
			m_serviceNode[i]->heartBeat();
		}
	}
}

void NodeMgr::timer()
{
	NodeMgr::getInstance()->cron();
}


Node* NodeMgr::findNode(uint32_t sid, uint32_t nid)
{
	ServiceNode * snode = getServiceNode(sid);
	if (snode) {
		return snode->findNode(nid);
	}
	return NULL;
}

bool NodeMgr::available(uint32_t sid, uint32_t nid)
{
	ServiceNode * snode = getServiceNode(sid);
	if (snode) {
		return snode->available(nid);
	}
	return false;
}
