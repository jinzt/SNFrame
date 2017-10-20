#ifndef _SERVICENODE_H
#define _SERVICENODE_H
#include <Define.h>
#include <Node.h>
#include <PbBuf.h>
#include <Log.h>
#include <NetManager.h>
class NodeMgr;
class NetServer;
class ServiceNode {
public:
	ServiceNode();
	virtual ~ServiceNode();
	int init(NodeMgr* manager,NetServer* pNet);
	
    Node* addNode(uint32_t sid, uint32_t nid);
	Connection* connect(const char* ip, uint16_t port, int type);
	bool available(uint32_t nid);
	Node* findNode(uint32_t nid);
	uint16_t  getServiceId();

	bool AuthenticationReq(int fd, IM::BaseDefine::ServiceID sid, uint32_t nid);


	void checkState();

	void heartBeat();


	virtual void parse(msg_t& msg)=0 ;
	virtual int init() = 0;

private:
	int reinit();

protected:

	IM::BaseDefine::ServiceID      m_sid;
	NodeMgr*                       m_pNodeMgr;
private:
	NetServer*         m_pNet;
	
	std::list<Node*> m_nodes;
	bool             m_reinit;

};

ServiceNode* CreateServiceNode(uint32_t sid);
#endif
