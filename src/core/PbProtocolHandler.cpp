#include "PbProtocolHandler.h"
#include <Service.h>
#include <Log.h>
PbProtocolHandler::PbProtocolHandler()
{
}

PbProtocolHandler * PbProtocolHandler::getInstance()
{
	static PbProtocolHandler instance;
	return &instance;
}

int PbProtocolHandler::regist(uint16_t sid, uint16_t cid, protocolHandler * cb)
{
	uint32_t id = (uint32_t)sid << 16 | cid;
	handler_t* handler= (handler_t*)calloc(1, sizeof(handler_t));
	if (NULL == handler) {
		LogMsg(LOG_ERROR,"alloc memroy fail£¬regist fail");
		return -1;
	}
	handler->sid = sid;
	handler->cid = cid;
	handler->cb = cb;
	m_handler[id] = handler;
	return 0;
}

bool PbProtocolHandler::handler(context_t& context)
{
	/*uint32_t nid = (uint32_t)reinterpret_cast<intptr_t>(context.arg);
	sendNode(context.msg, 66);
	return 0;*/
        uint32_t id=get_protocl(context.msg);
	std::map<uint32_t, handler_t*>::iterator it=m_handler.find(id);
	if(it!=m_handler.end()){
		if(it->second->cb){
			it->second->cb(context);
		}
	}
	else {
		LogMsg(LOG_DEBUG, "PbProtocolHandler fail,not find sid(%d),cimd(%d)", get_sid(context.msg), get_cmd(context.msg));
	}
	return false;
}
