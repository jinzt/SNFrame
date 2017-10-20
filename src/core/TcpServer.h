/*================================================================
*   Copyright (C) 2016 All rights reserved.
*
*   filename：net_work.h
*   author：Jiang Canjun
*   create date：2016-6-28
================================================================*/
#pragma once
#include <NetServer.h>
#include <PbBuf.h>
#include "ServerStatus.h"
#include  <Log.h>
#include <Lock.h>

enum {
	send_SUCCESS,
	send_FAIL,
	send_AGAIN
};
class MsgHandler;
class TcpServer :public NetServer
{
public:
	TcpServer();
	 ~TcpServer();

	void initContext(Connection * conn);
	void write(Connection * conn);
	void read(Connection* conn);
	void parse(Connection* conn);
	void handler(msg_t& msg, int id);
	bool signal(evutil_socket_t sig, short events, void * arg);

private:
	unsigned int                 m_type;
	
};
