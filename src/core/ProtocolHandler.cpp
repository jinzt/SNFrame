#include "ProtocolHandler.h"
#include <PbProtocolHandler.h>
#include <Connection.h>
ProtocolHandler::ProtocolHandler(int type)
{
}

ProtocolHandler * getProtocolHandler(int type)
{
	if (type == CONN_TCP) {
	    return PbProtocolHandler::getInstance();
	}
}

ProtocolHandler::ProtocolHandler()
{
}

int ProtocolHandler::regist(uint16_t sid, uint16_t cid, protocolHandler * handler)
{
	return 0;
}

