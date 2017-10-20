#include "ServiceNode.h"
#include <NetServer.h>
#include <RouterNode.h>
#include <ConnNode.h>
#include <DbNode.h>
#include <StateNode.h>
#include <NodeMgr.h>
#include <Common.h>
#include <Service.h>
ServiceNode * CreateServiceNode(uint32_t sid)
{
	switch (sid) {
	case IM::BaseDefine::ServiceID::SID_ROUTER:
		return new RouterNode;
	case IM::BaseDefine::ServiceID::SID_CONN:
		return new ConnNode;
	case IM::BaseDefine::ServiceID::SID_DB_PROXY:
		return new DbNode;
	case IM::BaseDefine::ServiceID::SID_USER_STATE:
		return new StateNode;
	default:
		break;
	}
	return NULL;
}

ServiceNode::ServiceNode()
{
}

ServiceNode::~ServiceNode()
{
}
int ServiceNode::init(NodeMgr * manager, NetServer* pNet)
{
	m_pNodeMgr = manager;
	m_pNet = pNet;
	m_reinit = false;
	this->init();
	return 0;
}

int ServiceNode::reinit()
{
	m_reinit = true;
	this->init();
	return 0;
}

Node* ServiceNode::addNode(uint32_t sid, uint32_t nid)
{
	if (sid != m_sid) {
		return NULL;
	}
	else {
		
		Node* node = new Node(sid,nid);
		if ( node) {
			m_nodes.push_back(node);
			return node;
		}
	}
	return NULL;
}

Connection* ServiceNode::connect(const char * ip, uint16_t port, int type)
{
	return m_pNet->connect(ip, port);
}

void ServiceNode::parse(msg_t & msg)
{
	return ;
}

bool ServiceNode::available(uint32_t nid)
{
	Node * node = findNode(nid);
	if (node) {
		int state = node->getState();
		if (NODE_ABLE == state || state==NODE_AUTH) {
			return true;
		}
		else {
			LogMsg(LOG_WARN, "sid(%u),node(%u) unavailable", m_sid, nid);
		}
	}
	return false;
}

Node * ServiceNode::findNode(uint32_t nid)
{
	std::list<Node*>::iterator it = m_nodes.begin();
	while (it != m_nodes.end()) {
		if ((*it)->getNodeId() == nid) {
			return *it;
		}
		++it;
	}
	return NULL;
}

uint16_t ServiceNode::getServiceId()
{
	return m_sid;
}

bool ServiceNode::AuthenticationReq(int fd, IM::BaseDefine::ServiceID sid, uint32_t nid)
{
	::IM::Server::IMAuthenticationReq req;
	req.set_type(sid);
	req.set_node_id(nid);
	msg_t auth_msg;
	if (!bufalloc(auth_msg, req.ByteSize() + 27)) {
		LogMsg(LOG_ERROR, "alloc memory fail");
		return IM_ERR;
	}
	set_tag(auth_msg, 0, 0);
	set_body_head(auth_msg, 1, IM::BaseDefine::ServiceID::SID_OTHER, IM::BaseDefine::CID_OTHER_AUTHENTICATION_REQ);
	set_pb(auth_msg, &req);
	sendBlock(auth_msg, fd);
	return true;
}

void ServiceNode::checkState()
{
	std::list<Node*>::iterator it = m_nodes.begin();
	while (it != m_nodes.end()) {
		Node* pNode = *it;
		int state = (*it)->getState();
		if (state== NODE_ERROR || state == NODE_CLOSED) {
			
			LogMsg(LOG_DEBUG,"node(sid:%u,nid(%u)",m_sid,pNode->getNodeId());
			if (pNode->needReconnect()) {
				Connection* conn = connect(pNode->getIp(), pNode->getPort(), 0);
				if (conn)
				{
					
					AuthenticationReq(conn->fd, m_sid, pNode->getNodeId());

					Connection* old_conn = static_cast<Connection*>(pNode->getContext());
					if (old_conn) {
						conn->setCache(old_conn->out_buffer);
						old_conn->cache = false;

						conn->setCb(old_conn->cb, old_conn->arg);
						m_pNet->close(old_conn->fd);
					}
					conn->setNodeInfo(m_sid, pNode->getNodeId());
					conn->setAddrs(pNode->getIp(), pNode->getPort());
					conn->setState(STATE_AUTH);

					pNode->setState(NODE_AUTH);
					pNode->setContext(conn);
					//conn->setCache(old_conn->)
				}
				else {

					LogMsg(LOG_WARN, "connect node[service(%u),node(%u)] fail", m_sid, pNode->getNodeId());
				}
			}
			else {
				LogMsg(LOG_WARN, "node[type:%u,node_id:%u) is unavailable", m_sid, pNode->getNodeId());
			
				//删除node
				it = m_nodes.erase(it);
				Connection* old_conn = static_cast<Connection*>(pNode->getContext());
				if (old_conn) {
					m_pNet->close(old_conn->fd);
				}
				delete pNode;
				//断开连接重新初始化router service
				if (m_sid == IM::BaseDefine::ServiceID::SID_ROUTER) {
					reinit();
				}
				continue;
			}
		}
		++it;
	}
} 
void ServiceNode:: heartBeat() {
	static uint32_t     loop = 0;
	if (loop % 300) {
		return;
	}
	std::list<Node*>::iterator it = m_nodes.begin();
	while (it != m_nodes.end()) {
		if ((*it)->getState() == NODE_ABLE) {
			Node* pNode = *it;
			Connection* conn = static_cast<Connection*>(pNode->getContext());
			if (conn) {
				msg_t msg;
				if (::init(msg)) {
					set_body_head(msg, 1, IM::BaseDefine::SID_OTHER, IM::BaseDefine::CID_OTHER_HEARTBEAT);
					set_tag(msg, conn->nid, 0);
					msg.m_len = 27;
					sendSock(msg, conn->fd);
				}
			}
		}
		++it;
	}
	++loop;
}
