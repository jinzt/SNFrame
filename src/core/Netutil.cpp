#include <Netutil.h>
#include <Log.h>
#include <Define.h>
#include <event.h>
#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#endif

int setTcpNodelay(int fd)
{
	int on = 1;
#ifdef WIN32
	if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char*)&on, sizeof(on)) == -1)
#else
	if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on)) == -1)
#endif
	{
		LogMsg(LOG_DEBUG, "setsockopt(TCP_NODELAY)");
		return IM_ERR;
	}
	return IM_OK;
}

int setSendBuffer(int fd, int buffsize)
{
#ifdef WIN32
	if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const char*)&buffsize, sizeof(buffsize)) == -1)
#else
	if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &buffsize, sizeof(buffsize)) == -1)
#endif
	{
		LogMsg(LOG_DEBUG, "setsockopt SO_SNDBUF fail");
		return IM_ERR;
	}
	return IM_OK;
}
