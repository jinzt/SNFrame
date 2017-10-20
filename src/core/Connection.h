#ifndef _CONNECTION_H
#define _CONNECTION_H

#include <PbBuf.h>
#include <IMQueue.h>

#define RECV_BUF_LEN          16*1024

#define    conn_client         1
#define    conn_server         2

typedef void notify(void* arg,int state);
enum ConnState {
	STATE_AUTH,
	STATE_CONNECTED, 
	STATE_ERROR,
	STATE_CLOSED,
	STATE_NONE
};

enum ConnType {
	CONN_TCP,
	CONN_UDP,
	CONN_HTTP,
	CONN_UNIX
};

#define  IO_ERROR           1
#define  IO_CLOSED          2
#define  IO_FINISH          3
#define  IO_AGAIN           4
class  Connection {
public:
	Connection();
	~Connection();
	bool empty();
	bool nextPkt();
	void clear();
	void push(msg_t& msg);
	void setAddrs(const char* ip, uint16_t port);
	void setNodeInfo(uint16_t sid, uint32_t nid);
	void setState(uint32_t state);
	void setCb(notify* cb,void* arg);
	void setCache(IMQueue<msg_t>* cache);

	int  write();
	int  read();

public:
	//addr info
	uint16_t                 port;
	char                    ip[16];
	//connection  attribute
	int                 flag;           //client or server
	uint16_t            sid;            //service type
	int                 nid;         //node id
	int                 fd;
	bool                multi;          //whether broast connection
	struct event*       revent;
	struct event*       wevent;
	short               evs;
	void*               server;
	int                 state;          //conn state
	int                 type;           //conn type

	//recv pkt
	char                      buf[RECV_BUF_LEN];
	msg_t			          pkt;
	int                       buf_len;
	int                       less_pkt_len;

	//for send
	IMQueue<msg_t>*           out_buffer;//store send data
	bool                      cache;     

	msg_t                     out_pkt;   //store a pkt
	bool                      finish;   //a pkt whether send finish
	int                       send_len; //send pos
	
	//use http
	void*                   context;

	//use notify node
	notify*                 cb;
	void*                   arg;
};

#endif
