#include "RouterNode.h"
#include <ConfigFileReader.h>
#include <Common.h>
#include <NodeMgr.h>
#include <Service.h>

void notifyNodeState(void* arg,int state) {
	LogMsg(LOG_DEBUG,"node state change notify");
	Node* pNode = static_cast<Node*>(arg);
	if (pNode) {
		if (state == STATE_ERROR) {
			pNode->setState(NODE_ERROR);
		}
		else if (state == STATE_CLOSED) {
			pNode->setState(NODE_CLOSED);
		}
		if (state == STATE_ERROR || state == STATE_CLOSED) {
			msg_t msg;
			set_tag(msg, pNode->getNodeId(), 0);//srv_id
			set_body_head(msg, 0, pNode->getSid(), 0);//sid
			if (!bufalloc(msg, 27)) {
				LogMsg(LOG_ERROR, "alloc memory fail");
				return ;
			}
			NodeMgr::getInstance()->notifyApp(msg,pNode->getNodeId());
		}
		
	}
}
void  RouterNode::parse (msg_t& msg) {
	
	uint16_t cid = get_cmd(msg);
	switch (cid) {
	case CID_ROUTER_CLIENT_REGISTER_RSP:
		registRsp(msg);
		break;
	case CID_ROUTER_PUSH_SERVER_STATUS:
		srvinfoBroadcast(msg);
		break;
	default:
		break;
	}
}

RouterNode::RouterNode(){
	m_sid=IM::BaseDefine::ServiceID::SID_ROUTER;
}

int RouterNode::init()
{
	char* service_type = CConfigFileReader::getInstance()->getConfigName("cur_node_type");
	char* router_ip = CConfigFileReader::getInstance()->getConfigName("RouteServerIP1");
	char* router_port = CConfigFileReader::getInstance()->getConfigName("RouteServerPort1");
	if ( !router_ip || !router_port || !service_type) {
		LogMsg(LOG_ERROR, "read config fail,must config ListenIP,ListenPort,RouteServerIP1,RouteServerPort1,ConnectionService,DbProxyService");
		return -1;
	}
	m_routerIP = router_ip;
	m_routerPort = atoi(router_port);
	m_type = (IM::BaseDefine::ServiceID)atoi(service_type);
	return registReq();
}

int RouterNode::registReq()
{
	char* tcpListen_ip = CConfigFileReader::getInstance()->getConfigName("ListenIP");
	char* tcpListen_port = CConfigFileReader::getInstance()->getConfigName("ListenPort");

	if (tcpListen_ip == NULL || tcpListen_port == NULL) {
		LogMsg(LOG_ERROR, "read config fail,must config tcp IP,Port");
		return IM_ERR;
	}
	Connection* conn = NULL;
	int i;
	for (i = 0; i < 4; i++) {
		conn = connect(m_routerIP, m_routerPort, 0);
		if (conn == NULL) {
			if (i == 3) {
				return IM_ERR;
			}
			sleep(1<<(i+1));
			LogMsg(LOG_ERROR, "connect route(ip: %s,port: %d)  fail %d times", m_routerIP, m_routerPort,i+1);
			continue;
		}
		break;
	}
	
	conn->setNodeInfo(IM::BaseDefine::ServiceID::SID_ROUTER, 0);
	conn->setAddrs(m_routerIP, m_routerPort);
	conn->setState(STATE_CONNECTED);

	Node* node = m_pNodeMgr->addNode(IM::BaseDefine::ServiceID::SID_ROUTER, conn->nid);
	node->setAddrs(m_routerIP, m_routerPort);
	node->setContext(conn);
	node->setState(NODE_ABLE);
	//route 断开需要重新注册，不能简单重连
	node->setPersistConnect(false);

	int fd = conn->fd;
	//注册服务器
	struct sockaddr_in addr;
	IM::Server::RegisterServerReq register_server;
	IM::Server::ServerStatus  *msg_server = register_server.mutable_current_server();// .add_current_server();
	msg_server->set_is_enable(true);
	msg_server->set_server_ip(tcpListen_ip, strlen(tcpListen_ip));
	msg_server->set_server_port(atoi(tcpListen_port));
	msg_server->add_service_type(m_type);
	msg_server->set_node_id(0);
	msg_t register_msg;
	if (!bufalloc(register_msg, register_server.ByteSize()+27)) {
		LogMsg(LOG_ERROR, "alloc memory fail");
		return IM_ERR;
	}
	set_tag(register_msg, 0, 0);
	set_body_head(register_msg, 1, IM::BaseDefine::ServiceID::SID_ROUTER, IM::BaseDefine::CID_ROUTER_CLIENT_REGISTER_REQ);
	set_pb(register_msg, &register_server);
	sendSock(register_msg,fd);
	return IM_OK;
}

int RouterNode::registRsp(msg_t& msg)
{
	IM::Server::RegisterServerRsp rsp;
	CHECK_PB_PARSE_MSG(rsp.ParseFromArray(get_data(msg), data_len(msg)));
	int result = rsp.result_code();
	LogMsg(LOG_DEBUG, "regist fail code(%d)", result);
	/*if (result != SERVER_REG_SUCCESSED) {
		
		m_pNodeMgr->exit();
		return -1;
	}*/
	LogMsg(LOG_DEBUG, "regist success");
	set_id(rsp.node_id());
	m_pNodeMgr->start();
	return true;
}

bool RouterNode::srvinfoBroadcast(msg_t & msg)
{
	::IM::BaseDefine::ServiceID type ;
	IM::Server::ServerStatusPush broadcast;
	CHECK_PB_PARSE_MSG(broadcast.ParseFromArray(get_data(msg), data_len(msg)));
	LogMsg(LOG_DEBUG, "srvinfo_broadcast server_list = %d ", broadcast.server_list_size());
	for (int i = 0; i < broadcast.server_list_size(); i++)
	{
		IM::Server::ServerStatus srv_info;
		srv_info = broadcast.server_list(i);
		if (srv_info.is_enable())//注册
		{
			uint32_t node_id = srv_info.node_id();
				
			for (int j = 0; j < srv_info.service_type_size(); j++)//遍历是否有我关心的服务器
			{
				type = srv_info.service_type(j);
				if (!m_pNodeMgr->findNode(type,node_id))//查找是否存在
				{
					if (m_pNodeMgr->getServiceNode(type))
					{
						int port = srv_info.server_port();
						Connection* conn = connect(srv_info.server_ip().c_str(), port, NULL);
						if (conn)
						{
							Node* node = m_pNodeMgr->addNode(type, node_id);
							if(!node){
								continue;
							}
							LogMsg(LOG_DEBUG, "add node[sid:%u,srv_id:%u,fd=%d", type, node_id, conn->fd);

							conn->setNodeInfo(type, node_id);
							conn->setAddrs(srv_info.server_ip().c_str(), port);
							conn->setState(STATE_AUTH);
							conn->setCb(notifyNodeState, node);
							
							node->setAddrs(srv_info.server_ip().c_str(), port);
							node->setState(NODE_AUTH);
							node->setContext(conn);
							AuthenticationReq(conn->fd, type, node_id);
							
						}
						else {
							LogMsg(LOG_ERROR, "connect node_id=%d srv_id:%u fail\n", node_id, type);
							return false;
						}

					}
				}
			}
			
		}
	}
	return true;
}

void RouterNode::set_id(uint32_t nid)
{
	m_id = nid;
	char id[10] = { 0 };
	sprintf(id, "%d", nid);
	CConfigFileReader::getInstance()->setConfigValue("node_id", id);
}

