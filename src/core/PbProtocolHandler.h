#ifndef _PBPROTOCOLHANDLER_H
#define _PBPROTOCOLHANDLER_H
#include <ProtocolHandler.h>
#include <PbBuf.h>
#include <map>

struct handler_t {
	uint16_t sid;
	uint16_t cid;
	protocolHandler*  cb;
	uint64_t       nums;  //recv num;
};
class PbProtocolHandler : public ProtocolHandler {
public:
	static PbProtocolHandler* getInstance();
	int regist(uint16_t sid,uint16_t cid, protocolHandler* cb);
	bool handler(context_t& context);
private:
	PbProtocolHandler();
	std::map<uint32_t, handler_t*>  m_handler;
};
#endif
