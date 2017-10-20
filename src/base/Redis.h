#ifndef _REDIS_CACHE_H
#define _REDIS_CACHE_H

#include <hiredis.h>
#include<string>
class Redis {
public:
	Redis();
	~Redis();
	void init();
	int  connect(const char* ip, short port);
	redisReply* query(const char* command[], int num, size_t* vlen);
	int  del(std::string& key);
private:
	redisContext*               m_context;
};
#endif
