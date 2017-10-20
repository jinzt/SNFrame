#ifndef _NETLIBBASE_H
#define _NETLIBBASE_H
#include <Connection.h>
#define AE_OK 0
#define AE_ERR -1

#define AE_NONE 0
#define AE_READABLE 1
#define AE_WRITABLE 2
#define AE_ACCEPTABLE 4
#define AE_ERROR    8

typedef void(*timer_handler)(void);
class NetLibBase {
public:
	virtual int createClient(const char* ip, int port, int type) = 0;
	virtual int createServer(const char * ip, int port, int type) = 0;
	virtual int addFileEvent(Connection * conn,int mask) = 0;
	virtual int delFileEvent(Connection * conn, int mask) = 0;
	virtual void addTimer(timer_handler cb, int sec) = 0;
	virtual int loop(const timeval * tv) = 0;
	virtual int stop() = 0;
	
};
#endif