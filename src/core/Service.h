#ifndef _SERVICE_H
#define _SERVICE_H
#include<PbBuf.h>

int sendBlock(msg_t& msg, int fd);
int sendNode(msg_t& msg, uint32_t nid);
int sendSock(msg_t& msg, int fd);
int sendDb(msg_t& msg);
int sendConn(msg_t& msg);
void addTimer(void (*cb)(), int sec);
int sendState(msg_t& msg);
#endif