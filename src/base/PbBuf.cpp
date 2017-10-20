#include "PbBuf.h"
#include "Define.h"
#include <Log.h>
#include <Common.h>

enum Headoff {
	off_FLAGS = offsetof(CommonHeader, flags),
	off_CONNSERVERID = offsetof(CommonHeader, connServerId),
	off_SOCKETID = offsetof(CommonHeader, socketId),
	off_ISROUTERMSG = offsetof(CommonHeader, IsRouterMsg),
	off_SIGN = offsetof(CommonHeader, sign),
	off_VER = offsetof(CommonHeader, ver),
	off_MSGID = offsetof(CommonHeader, msgId),
	off_SERVICEID = offsetof(CommonHeader, serviceId),
	off_CMDID = offsetof(CommonHeader, cmdId),
	off_COMPANYID = offsetof(CommonHeader, companyId),
	off_LEN = offsetof(CommonHeader, len),
	off_CHECK = offsetof(CommonHeader, check),
};

 void  buffree(msg_t& msg)
{
	 return;
	if (msg.m_data != NULL)
	{
		free(msg.m_data);
		msg.m_data = NULL;
	}
}

bool  init(msg_t& msg)
{
	msg.m_data = NULL;
	msg.m_len = 0;
	msg.m_alloc = 0;
	msg.m_data = (char*)malloc(BUFSIZE);
	if (msg.m_data)
	{
		msg.m_alloc = BUFSIZE;
        memset(msg.m_data,0,msg.m_alloc);
		return true;
	}
	return false;
}

 bool bufalloc(msg_t & msg, int len)
{
	msg.m_data = NULL;
	msg.m_len = 0;
	msg.m_alloc = 0;
	msg.m_data = (char*)malloc(len);
	if (msg.m_data)
	{
		msg.m_alloc = len;
		memset(msg.m_data, 0, len);
		return true;
	}
	return false;
}
 
bool  resize(msg_t& msg, int new_lenth)
{
	if (msg.m_alloc  >= new_lenth)
	{
		return true;
	}
	char *temp = (char*)realloc(msg.m_data, new_lenth);
	if (temp != NULL)
	{
		msg.m_data = temp;
		msg.m_alloc = new_lenth;
		return true;
	}
	return false;
}

void set_tag(msg_t& msg, uint32_t srv_id, uint32_t sock_id)
{
	if (msg.m_data)
	{
		msg.m_data[0] = TAG_HEAD_1;
		msg.m_data[1] = TAG_HEAD_2;
		*(uint32_t *)(msg.m_data + off_CONNSERVERID) = htonl(srv_id);
		*(uint32_t *)(msg.m_data + off_SOCKETID) = htonl(sock_id);
	}
}

bool new_head(msg_t & dst, msg_t & src, uint16_t cid, uint32_t body_len)
{
	if (!bufalloc(dst, body_len + HEAD_LEN)) {
		LogMsg(LOG_ERROR, "init msg fail,alloc memory fail");
		return false;
	}
	memcpy(dst.m_data, src.m_data, HEAD_LEN);
	*(uint16_t *)(dst.m_data + off_CMDID) = htons(cid);
	return true;
}
void  set_body_head(msg_t& msg, uint16_t  number, uint16_t sid, uint16_t cid, char verssion)
{
	if (msg.m_data)
	{
		msg.m_data[11] = 102;
		msg.m_data[12] = 35;
		if (verssion != 0)
		{
			msg.m_data[off_VER] = verssion;
		}
		if (number != 0)
		{
			*(uint16_t *)(msg.m_data + off_MSGID) = htons(number);
		}
		if (sid != 0)
		{
			*(uint16_t *)(msg.m_data + off_SERVICEID) = htons(sid);
		}
		if (cid != 0)
		{
			*(uint16_t *)(msg.m_data + off_CMDID) = htons(cid);
		}
	}
}

bool  set_data(msg_t& msg, const char* buf, uint16_t len)
{
	bool ret = false;
	uint16_t  total = len + HEAD_LEN;
	if (msg.m_alloc  < total)
	{
		ret = resize(msg, total);
	}
	if (!msg.m_data)
	{
		return ret;
	}
	*(uint16_t *)(msg.m_data + off_LEN) = htons(len);
	if (buf)
	{
		memcpy(msg.m_data + HEAD_LEN, buf, len);
	}
	char c = msg.m_data[24] ^ msg.m_data[25];
	c ^= msg.m_data[HEAD_LEN];

	for (int i =  1; i < len; ++i)
	{
		 c ^= msg.m_data[HEAD_LEN + i];
	}
	msg.m_data[off_CHECK] = c;
	msg.m_len = total;
	ret = true;
	return ret;
}

bool  set_pb(msg_t& msg, const google::protobuf::MessageLite* data)
{
	if (!msg.m_data) {
		return false;
	}
	uint32_t len = data->ByteSize();
	uint16_t  total = len + HEAD_LEN;
	*(uint16_t *)(msg.m_data + off_LEN) = htons(len);
	if (!data->SerializeToArray(msg.m_data + HEAD_LEN, len)) {
		LogMsg(LOG_DEBUG, "serialize pb fail");
		return false;
	}
	char c = msg.m_data[24] ^ msg.m_data[25];
	c ^= msg.m_data[HEAD_LEN];

	for (int i = 1; i < len; ++i)
	{
		c ^= msg.m_data[HEAD_LEN + i];
	}
	msg.m_data[off_CHECK] = c;
	msg.m_len = total;
	return  true;
}

bool set_checksum(msg_t& msg){
	int len=msg.m_len-HEAD_LEN;
	if(len<0){
		return false;
	}
	*(uint16_t *)(msg.m_data + off_LEN) = htons(len);
	char c = msg.m_data[24] ^ msg.m_data[25];
	c ^= msg.m_data[HEAD_LEN];

	for (int i = 1; i < len; ++i)
	{
		c ^= msg.m_data[HEAD_LEN + i];
	}
	msg.m_data[off_CHECK] = c;
	return  true;

}

char*  get_data(msg_t& msg)
{
	if (!msg.m_data)
	{
		return NULL;
	}
	return msg.m_data + HEAD_LEN;
}

uint32_t data_len(msg_t& msg) {
	return msg.m_len - HEAD_LEN;
}

uint32_t  get_company_id(msg_t& msg)
{
	if (msg.m_data)
	{
		return  ntohl(*(uint32_t*)(msg.m_data + off_COMPANYID));
	}
	return 0;
}
uint16_t  get_packge_number(msg_t& msg)
{
	if (msg.m_data)
	{
		return ntohs( *(uint32_t*)(msg.m_data + off_MSGID));
	}
	return 0;
}
 
uint16_t  get_data_len(msg_t& msg)
{
	if (msg.m_data)
	{
		return  ntohs(*(uint16_t*)(msg.m_data + off_LEN));
	}
	return 0;
}
  
uint32_t get_protocl(msg_t& msg)
{
	if (msg.m_data)
	{
		return  ntohl(*(uint32_t*)(msg.m_data + off_SERVICEID));
	}
	return 0;
}
uint16_t get_sid(msg_t & msg)
{
	if (msg.m_data)
	{
		return  ntohs(*(uint16_t*)(msg.m_data + off_SERVICEID));
	}
	return 0;
}
uint16_t get_cmd(msg_t & msg)
{
	if (msg.m_data)
	{
		return  ntohs(*(uint16_t*)(msg.m_data + off_CMDID));
	}
	return 0;
}
 
uint16_t get_msg_id(msg_t & msg)
{
	if (msg.m_data)
	{
		return  ntohs(*(uint16_t*)(msg.m_data + off_MSGID));
	}
	return 0;
}

char get_version(msg_t& msg)
{
	if (msg.m_data)
	{
		return msg.m_data[off_VER];
	}
	return 0;
}
 
bool check(msg_t& msg)
{
	if (msg.m_data)
	{
		char c = msg.m_data[24] ^ msg.m_data[25];
		c ^= msg.m_data[HEAD_LEN];
		for (int i = 1; i < msg.m_len - HEAD_LEN; ++i)
		{
			c ^= msg.m_data[HEAD_LEN + i];
		}
		return msg.m_data[off_CHECK] == c;
	}
	LogMsg(LOG_ERROR, "check sum err");
	return false;
}

bool deep_copy (msg_t& dest,const msg_t& msg)
{
	if (resize(dest, msg.m_alloc))
	{
		memcpy(dest.m_data, msg.m_data, msg.m_len);
		dest.m_len = msg.m_len;
		return true;
	}
	return false;
}
uint32_t  get_srv_id(msg_t &msg)
{
	uint32_t srv_id = 0;
	if (msg.m_data)
	{
		srv_id = ntohl(*((uint32_t*)(msg.m_data + off_CONNSERVERID)));
	}
	return srv_id;
}

uint32_t get_socket_id(msg_t &msg)
{
	uint32_t socket_id = 0;
	if (msg.m_data)
	{
		socket_id = ntohl(*((uint32_t*)(msg.m_data + off_SOCKETID)));
	}
	return socket_id;
}




