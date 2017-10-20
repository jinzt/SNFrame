#include "TcpServer.h"
#include "Config.h"
#include <time.h>
#include <cstring>
#include <Service.h>
#include <IM.BaseDefine.pb.h>


#pragma  pack(push,1)
struct ServerHeader {
	uint16_t    flags;
	uint32_t    connServerID;
	uint32_t    socketId;
	uint8_t     IsRouterMsg;
};

struct ClientHeader {
	uint16_t    sign;
	uint8_t     ver;
	uint16_t    msgID;
	uint16_t    serviceID;
	uint16_t    cmdID;
	uint32_t    companyID;
	uint16_t    len;
	uint8_t     check;
};

struct CommonHeader {
	ServerHeader    sHeader;
	ClientHeader    cHeader;
	char            data[];
};
#pragma pack(pop)

extern void parseCommonProto(msg_t& msg, Connection* conn);
static void  msg_info(msg_t& msg) {
	CommonHeader* head = (CommonHeader*)msg.m_data;
    LogMsg(LOG_DEBUG, "recv  msg(%u)  serviceid(%d),cmdid(%d) len(%d)", get_msg_id(msg),ntohs(head->cHeader.serviceID), ntohs(head->cHeader.cmdID), ntohs(head->cHeader.len));
	
}

static void getsockInfo (int fd,uint16_t& port,char* ip) {
#ifdef WIN32
	int
#else 
	socklen_t
#endif	
		len;
	struct sockaddr_in addr;
	len = sizeof(addr);
	getsockname(fd, (struct sockaddr *)&addr, &len);
	ip = inet_ntoa(addr.sin_addr);
	port = ntohs(addr.sin_port);
}
TcpServer::TcpServer(): NetServer(CONN_TCP)
{
	
}
TcpServer::~TcpServer()
{
}

void TcpServer::write(Connection * conn)
{
	if (!conn) {
		return;
	}
	int ret = conn->write();
	if (IO_ERROR == ret) {
		conn->setState(STATE_ERROR);
		LogMsg(LOG_ERROR, "fd=%d[type:%d,port: %hd ] send error", conn->fd, conn->sid, conn->port);

	}
	else if (IO_CLOSED == ret) {
		conn->setState(STATE_CLOSED);
		LogMsg(LOG_ERROR, "fd=%d[type:%d, port: %hd ] close", conn->fd, conn->sid, conn->port);
	}
	else {
		if (conn->empty()) {
			m_pNetMgr->delFileEvent(conn->fd, AE_WRITABLE);
		}
		return;
	}

	// write error
	m_pNetMgr->delFileEvent(conn->fd, AE_READABLE | AE_WRITABLE);
	if (conn->flag != conn_client) {
		m_pNetMgr->closeConnection(conn->fd);
	}
}

void TcpServer::read(Connection * conn)
{
	if (!conn) {
		return;
	}
	int ret = conn->read();
	if (IO_ERROR == ret) {
		LogMsg(LOG_ERROR, "read err,will drop the packet");
		conn->setState(STATE_ERROR);
	}
	else if (IO_CLOSED == ret) {
		LogMsg(LOG_ERROR, "client close");
		conn->setState(STATE_CLOSED);
	}
	else {
		parse(conn);
		return;
	}

	//read fail
	m_pNetMgr->delFileEvent(conn->fd, AE_READABLE | AE_WRITABLE);
	//被动连接直接关闭
	if (conn->flag != conn_client) {
		m_pNetMgr->closeConnection(conn->fd);
	}
}

void TcpServer::parse(Connection* conn) {
	int fd = conn->fd;
	char* data = conn->buf;
	while (conn->buf_len >= HEAD_LEN) {
		//没有不完整的包
		if (!conn->less_pkt_len) {
			if ((uint8_t)data[0] == TAG_HEAD_1 && (uint8_t)data[1] == TAG_HEAD_2)//正常情况下 先接收到包头
			{
				if (conn->buf_len >= HEAD_LEN) {
					int data_len = ntohs(*(uint16_t*)(data + 24));
					int pkt_len = data_len + HEAD_LEN;
					if (conn->buf_len >= pkt_len) {
						msg_t msg;

						if (bufalloc(msg, pkt_len)) {
							memcpy(msg.m_data, data, pkt_len);
							msg.m_len = pkt_len;
							conn->buf_len -= pkt_len;
							data += pkt_len;
							if (!check(msg)) {
								buffree(msg);
								continue;
							}
							msg_info(msg);
							uint16_t sid = get_sid(msg);
							if (sid == IM::BaseDefine::ServiceID::SID_ROUTER || sid == IM::BaseDefine::ServiceID::SID_OTHER) {
								parseCommonProto(msg, conn);
								buffree(msg);
							}
							else {
								handler(msg, conn->nid);
							}
						}
					}
					//not enough a pkt ;
					else {
						if (bufalloc(conn->pkt, pkt_len)) {
							memcpy(conn->pkt.m_data, data, conn->buf_len);
							conn->pkt.m_len = conn->buf_len;
							conn->less_pkt_len = pkt_len - conn->buf_len;
							conn->buf_len = 0;
							break;
						}
					}
				}
			}
			else {//找到包头
				int i;
				for (i = 0; i < conn->buf_len - 1; ++i) {
					if ((uint8_t)data[i] == TAG_HEAD_1 && (uint8_t)data[i + 1] == TAG_HEAD_2) {
						data += i;
						conn->buf_len -= i;
						break;
					}
				}
				if (i == conn->buf_len - 1) {
					conn->clear();
					break;
				}
			}
		}
		else {
			int less_pkt_len = conn->less_pkt_len;
			if (conn->buf_len >= less_pkt_len) {
				memcpy(conn->pkt.m_data + conn->pkt.m_len, data, less_pkt_len);
				conn->pkt.m_len += less_pkt_len;
				conn->buf_len -= less_pkt_len;
				msg_t msg;
				msg = conn->pkt;

				data += less_pkt_len;

				//清空记录
				memset(&conn->pkt, 0, sizeof(conn->pkt));
				conn->less_pkt_len = 0;
				if (!check(msg)) {
					buffree(msg);
					continue;
				}
				msg_info(msg);
				uint16_t sid = get_sid(msg);
				if (sid == IM::BaseDefine::ServiceID::SID_ROUTER || sid == IM::BaseDefine::ServiceID::SID_OTHER) {
					parseCommonProto(msg, conn);
					buffree(msg);
				}
				else {
					handler(msg, conn->nid);
				}
			}
			else {
				memcpy(conn->pkt.m_data + conn->pkt.m_len, data, conn->buf_len);
				conn->pkt.m_len += conn->buf_len;
				conn->less_pkt_len -= conn->buf_len;
				conn->buf_len = 0;
			}
		}
	}

	//move not enough header data to buf start
	if (conn->buf_len && data != conn->buf) {
		memmove(conn->buf, data, conn->buf_len);
	}
}

void TcpServer::handler(msg_t & msg, int id)
{
	context_t context;
	context.msg = msg;
	context.arg = (int*)(id);
	m_recver.addData(context);
}

bool TcpServer::signal(evutil_socket_t sig, short events, void *arg)
{
	return true;
}

void TcpServer::initContext(Connection * conn)
{
	if (conn)
	{
		conn->type = CONN_TCP;
	}
	return ;
}


