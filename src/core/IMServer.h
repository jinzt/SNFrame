
#ifndef _IMSERVER_H
#define _IMSERVER_H

#include <HttpServer.h>
#include "TcpServer.h"

#include <ConfigFileReader.h>
#include <list>
#include <NodeMgr.h>

class IMServer
{
public:
	static IMServer*  get_instance();
	
	~IMServer();

	int init();
	int start();

	/**
	* @brief  设置服务器类型
	* @param type 服务器类型
	*/
	void set_type(IM::BaseDefine::ServiceID type);

	/**
	* @brief 取得系统状态百分比
	*/
	char get_sys_();

private:
	
	IMServer();

private:
	char                     m_tcpIP[16];
	uint16_t                 m_tcpPort;

	bool                     m_bHttp;     //是否支持http
	char                     m_htttpIP[16];
	uint16_t                 m_httpPort;

	TcpServer                m_tcp;
	HttpServer               m_http;

	ServerStatus             m_srv_status;
	NodeMgr*                 m_pNodeMgr;
};
#endif
