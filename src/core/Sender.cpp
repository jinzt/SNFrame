#include "Sender.h"
#include <NetManager.h>
#include <NetManager.h>
#include <Log.h>
Sender::Sender()
{
	m_pNetMgr = NetManager::getInstance();
}
Sender::~Sender()
{
	
}
int Sender::sendBlock(msg_t & msg, int fd)
{
	int ret;
	uint16_t  port;
	int sendLen = msg.m_len;
	int pos = 0;
	while (sendLen) {
		ret = send(fd, msg.m_data + pos, sendLen, 0);
		if (ret == 0) {
			break;
		}
		else if (ret == -1)
		{
			if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) {
				continue;
			}
			else {
				break;
			}
		}
		else
		{
			pos += ret;
			sendLen -= ret;
		}
	}
	if (sendLen == 0) {
		return 0;
	}
	else {
		return -1;
	}
}
int Sender::sendNode(msg_t & msg, uint32_t nid)
{
	//LogMsg(LOG_DEBUG, "send msg node(%u)", nid);
	return m_pNetMgr->sendNode(msg, nid);
	
}

int Sender::sendSock(msg_t & msg, int fd)
{
	LogMsg(LOG_DEBUG, "send msg fd(%d)", fd);
	return m_pNetMgr->sendSock(msg, fd);
}

int Sender::sendDb(msg_t & msg)
{
	return m_pNetMgr->sendDb(msg);
}
int Sender::sendState(msg_t& msg){
	return m_pNetMgr->sendState(msg);
}
