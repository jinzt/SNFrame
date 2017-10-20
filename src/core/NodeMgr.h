#pragma once
#include <ServiceNode.h>
class NodeMgr {
public:
	NodeMgr();
	static NodeMgr* getInstance();
	//根据关心的服务类型，创建服务类
	int init(NetServer* net);
	bool parse(msg_t& msg,Connection* conn);
	bool AuthenticationReq(msg_t& msg,Connection* conn);
	bool AuthenticationRsp(msg_t& msg, Connection* conn);

	void notifyApp(msg_t& msg,int id);
	Node* addNode(uint32_t sid, uint32_t nid);
	Node* findNode(uint32_t sid, uint32_t nid);
	bool available(uint32_t sid, uint32_t nid);
	ServiceNode* getServiceNode(uint32_t sid);

	void start();
	void exit();
	

private:
	static void startCron();
     void cron();
	static void timer();

private:
	ServiceNode**   m_serviceNode;
	int             m_num;   
	NetServer*      m_pNet;
};