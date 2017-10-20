#ifndef _PROTOCOLHANDLER_H
#define _PROTOCOLHANDLER_H
#include <PbBuf.h>
typedef bool protocolHandler(context_t& context);
class ProtocolHandler {
public:
	ProtocolHandler(int type);
	ProtocolHandler();
	virtual int regist(uint16_t sid,uint16_t cid, protocolHandler* handler);
	virtual bool handler(context_t& context)=0;

};

ProtocolHandler* getProtocolHandler(int type);
#endif
