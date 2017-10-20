#include "IMServer.h"
#include "Log.h"
#include "Config.h"
#include <Service.h>
#include <ApplicationBase.h>
#include <Common.h>
#include <TcpServer.h>
#include <Timer.h>
#define THROUGHPUT
 uint32_t recv_chat_pkts =0;
 uint32_t send_success_pkts=0;
 uint32_t send_fail_pkts=0;
 uint32_t last_send_pkts = 0;

 void counter() {
	 
//	 LogMsg(LOG_DEBUG, "recv  msg %u, send success pkts:%u,send fail pkt:%u, send rate :%d packet every sec", recv_chat_pkts, send_pkts, send_fail_pkts,send_pkts- last_send_pkts);
	// printf("recv  msg %u, send success pkts:%u,send fail pkt:%u, send rate :%d packet every sec\n", recv_chat_pkts, send_success_pkts, send_fail_pkts, send_success_pkts - last_send_pkts);
	 last_send_pkts = send_success_pkts;
 }

char IMServer::get_sys_()
{
	char percent = m_srv_status.get_percent();
	LogMsg(LOG_DEBUG, "server status cpu_used:%d  mem_used:%d\n", m_srv_status.get_cpu());
	LogMsg(LOG_DEBUG, "server status mem_total:%d  mem_used:%d\n", m_srv_status.get_mem_tatol(), m_srv_status.get_mem_used());
	return percent;
}

IMServer::IMServer()
{
	m_bHttp = false;
	m_pNodeMgr = NodeMgr::getInstance();
}

IMServer::~IMServer()
{
}

int IMServer::init()
{
	char* tcpListen_ip = CConfigFileReader::getInstance()->getConfigName("ListenIP");
	char* tcpListen_port = CConfigFileReader::getInstance()->getConfigName("ListenPort");
	
	if (tcpListen_ip == NULL || tcpListen_port ==NULL ) {
		LogMsg(LOG_ERROR, "read config fail,must config tcp IP,Port");
		return IM_ERR;
	}
	char* http= CConfigFileReader::getInstance()->getConfigName("http");
	if (http) {
		if (atoi(http)) {
			m_bHttp = true;
			char* HttpListen_ip = CConfigFileReader::getInstance()->getConfigName("HttpIP");
			char* HttpListen_port = CConfigFileReader::getInstance()->getConfigName("HttpPort");
			if (HttpListen_ip == NULL || HttpListen_port == NULL) {
				LogMsg(LOG_ERROR, "read config fail,must config http  IP,Port");
				return IM_ERR;
			}
			strcpy(m_htttpIP, HttpListen_ip);
			m_httpPort = atoi(HttpListen_port);
		}
	}
	strcpy(m_tcpIP, tcpListen_ip);
	m_tcpPort = atoi(tcpListen_port);
	
	if (m_tcp.init(m_tcpPort, m_tcpIP) == -1) {
		return IM_ERR;
	}
	if (m_bHttp) {
		if (m_http.init(m_httpPort, m_htttpIP) == -1) {
			return IM_ERR;
		}
	}
#ifndef WIN32
#include <unistd.h>
	char* pid_file = CConfigFileReader::getInstance()->getConfigName("PidFile");
	if (pid_file == NULL) {
		LogMsg(LOG_ERROR, "can't find pid file config");
		return IM_ERR;
	}
	int fd = open(pid_file, O_WRONLY | O_CREAT, 0666);
	if (fd == -1) {
		LogMsg(LOG_ERROR, "open file %s fail", pid_file);
		return IM_ERR;
	}
	pid_t pid = getpid();
	write(fd, &pid, sizeof(pid));
	close(fd);
#endif
	ApplicationBase::getInstance()->init();
	m_tcp.prepare();
	if (m_bHttp) {
		m_http.prepare();
	}
#ifdef WIN32
	Sleep(200);
#else 
	usleep(1000 * 200);
#endif
	m_pNodeMgr->init(&m_tcp);
	return IM_OK ;
}

int IMServer::start()
{		
	Timer::getInstance()->startTimer();
#ifdef THROUGHPUT
	Timer::getInstance()->addTimer(counter,1);
#endif
	m_tcp.start();
	if (m_bHttp) {
		m_http.start();
	}
	return IM_ERR;
}

IMServer* IMServer::get_instance()
{
	static IMServer	instance ;
	return &instance;
}

void IMServer::set_type(IM::BaseDefine::ServiceID type)
{
	//m_type = type;
}
