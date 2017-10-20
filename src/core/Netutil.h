#ifndef _NETUTIL_H
#define _NETUTIL_H

int setTcpNodelay(int fd);
int setSendBuffer(int fd, int buffsize);
#endif
