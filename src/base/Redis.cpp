#include "Redis.h"
#include<Log.h>
Redis::Redis()
{
	m_context = NULL;
	init();
}
Redis::~Redis()
{
	if (m_context) {
		redisFree(m_context);
	}
}
void Redis::init()
{
#ifdef WIN32  
	WSADATA t_wsa;
	WORD wVers = MAKEWORD(2, 2); // Set the version number to 2.2
	int iError = WSAStartup(wVers, &t_wsa);

	if (iError != NO_ERROR || LOBYTE(t_wsa.wVersion) != 2 || HIBYTE(t_wsa.wVersion) != 2) {
		LogMsg(LOG_ERROR, "Winsock2 init error");
		return ;
	}
#endif
}

int  Redis::connect(const char * ip, short port)
{
	if (ip == NULL) {
		return -1;
	}
	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	redisContext* context = redisConnectWithTimeout(ip, port, tv);
	if (context && !context->err)
	{
		m_context = context;
		return 0;
	}
	else {
		LogMsg(LOG_ERROR, "connect redis error %s", context->errstr);
		return -1;
	}
}

int Redis::del(std::string& key)
{
	bool bret = false;
	const char *command[2];
	size_t vlen[2];
	command[0] = "del";
	vlen[0] = 3;
	command[1] = key.c_str();
	vlen[1] = key.length();
	redisReply* result = query(command, sizeof(command) / sizeof(command[0]), vlen);
	if (result)
	{
		if (result->type == REDIS_REPLY_STRING)
		{
			bret = true;
		}
		freeReplyObject(result);
	}
	return bret;
}

redisReply * Redis::query(const char * command[], int num, size_t * vlen)
{
	if (m_context == NULL) {
		return NULL;
	}
	redisReply* result = NULL;
	result = (redisReply*)redisCommandArgv(m_context, num, command, vlen);
	if (NULL == result)//出现严重的错误  需要重连
	{
		LogMsg(LOG_ERROR, "insert redisCommandArgv error need reconnect");
		redisFree(m_context);
		m_context = NULL;
		return NULL;
	}
	return result;
}
