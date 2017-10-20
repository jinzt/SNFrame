#include "Service.h"
#include <Sender.h>
#include <Timer.h>
Sender sender;

int sendBlock(msg_t & msg, int fd)
{
	return sender.sendBlock(msg,fd);
}

int sendNode(msg_t & msg, uint32_t nid)
{
	return sender.sendNode(msg, nid);
}

int sendSock(msg_t & msg, int fd)
{
	return sender.sendSock(msg, fd);
}

int sendDb(msg_t & msg)
{
	return sender.sendDb(msg);
}

int sendState(msg_t& msg){
	return sender.sendState(msg);
}
int sendConn(msg_t & msg)
{
	uint32_t nid = get_srv_id(msg);
	return sendNode(msg, nid);
}

void addTimer(void(*cb)(), int sec)
{
	Timer::getInstance()->addTimer(cb, sec);
}

