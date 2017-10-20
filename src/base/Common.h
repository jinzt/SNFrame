
#ifndef _COMMON_H
#define _COMMON_H
#include <stdio.h>
#include <iostream>
#define  _PROTOCOL_EXPORTS
#ifdef WIN32
#else
#include <unistd.h>
#endif

#include <IM.BaseDefine.pb.h>
#include <IM.Router.pb.h>
#include <IM.Server.pb.h>

#include <ConfigFileReader.h>
#include <Log.h>
#include <Define.h>

#include <Service.h>
using namespace IM::BaseDefine;
using namespace std;

#pragma  pack(push,1)

struct CommonHeader {
	/*server header*/
	uint16_t    flags;
	uint32_t    connServerId;
	uint32_t    socketId;
	uint8_t     IsRouterMsg;

	/*client header*/
	uint16_t    sign;
	uint8_t     ver;
	uint16_t    msgId;
	uint16_t    serviceId;
	uint16_t    cmdId;
	uint32_t    companyId;
	uint16_t    len;
	uint8_t     check;
	char        data[];
};
#pragma pack(pop)

#define CHECK_PB_PARSE_MSG(ret) {            \
    if (ret == false)                            \
    {                                            \
        LogMsg(LOG_DEBUG,"parse pb msg failed.");   \
        return false;                            \
    }                                            \
}

//#define  new  new(std::nothrow)


#endif
