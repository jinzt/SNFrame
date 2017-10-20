#include <Connection.h>
#include <Log.h>
#ifndef WIN32
#include <unistd.h>
#endif

extern  uint32_t send_success_pkts;
extern uint32_t send_fail_pkts;
Connection::Connection()
{
	sid = 0;
	nid = 0;
	fd = -1;
	memset(buf, 0, sizeof(buf));
	memset(&pkt, 0, sizeof(msg_t));
	memset(ip, 0, sizeof(ip));
	buf_len = 0;
	less_pkt_len = 0;
	multi = false;
	state = STATE_NONE;

	//out_buffer = new IMQueue<msg_t>;
	send_len = 0;
	context = NULL;
	evs = 0;
	finish = true;
	revent = (struct event*)malloc(sizeof(struct event));
	wevent = (struct event*)malloc(sizeof(struct event));
	cb = NULL;
	arg = NULL;

	out_buffer = new IMQueue<msg_t>;
	cache = true;
}

Connection::~Connection()
{
	if (cache && out_buffer) {
		delete out_buffer;
	}

	buf_len = 0;
	less_pkt_len = 0;
	free(revent);
	free(wevent);
	
	if (fd > -1) {
#ifdef WIN32

		closesocket(fd);
#else

		close(fd);
#endif
	}
}

bool Connection::empty()
{
	return finish && out_buffer->empty();
}

bool Connection::nextPkt()
{
	if (!finish) {
		//msg = conn->out_pkt;
		return true;
	}
	else if (!out_buffer->empty()) {
		out_buffer->pop(out_pkt);
		//		msg = conn->out_pkt;
		finish = false;
		return true;
	}
	return false;
}

void Connection::clear()
{
	memset(buf, 0, sizeof(buf));
	buffree(pkt);
	memset(&pkt, 0, sizeof(msg_t));
	buf_len = 0;
	less_pkt_len = 0;
}

void Connection::push(msg_t & msg)
{
	out_buffer->push(msg);
}

void Connection::setAddrs(const char * ip, uint16_t port)
{
	this->port = port;
	strcpy(this->ip, ip);
}

void Connection::setNodeInfo(uint16_t sid, uint32_t nid)
{
	this->sid = sid;
	this->nid = nid;
}

void Connection::setState(uint32_t state)
{
	this->state = state;
	if (state == STATE_CLOSED || state == STATE_ERROR) {
		if (cb) {
			cb(arg, state);
		}
	}
}

void Connection::setCb(notify * cb,void* arg)
{
	this->cb = cb;
	this->arg = arg;
}

void Connection::setCache(IMQueue<msg_t>* cache)
{
	if (out_buffer) {
		delete out_buffer;
		out_buffer = cache;
	}
}

int Connection::write()
{
	size_t nwritten = 0;
	size_t data_len;

	while (nextPkt()) {
		msg_t& data = out_pkt;
		data_len = data.m_len;
		nwritten = send(fd, data.m_data + send_len, data_len - send_len, 0);
		if (nwritten == -1) {
			if (errno == EAGAIN || errno==EWOULDBLOCK) {
				return IO_AGAIN;
			}
			else {
				++send_fail_pkts;
				return IO_ERROR;
			}
		}
		else if (nwritten == 0) {
			return IO_CLOSED;
		}
		send_len += nwritten;

		if (send_len == data_len) {
			LogMsg(LOG_DEBUG,"send fd(%d) port(%d) sid(%d) %d bytes",fd,port,sid,send_len);
			++send_success_pkts;
			send_len = 0;
			finish = true;
		}
	}
	return IO_FINISH;
}

int Connection::read()
{

	int recv_len = RECV_BUF_LEN - buf_len;
	int nRcv = 0;
	int res = 0;
	while (recv_len) {
		nRcv = recv(fd, buf + buf_len, recv_len, 0);//每次先取包头长度 
		if (nRcv < 0)
		{
			if (errno == EINTR) {
				continue;
			}
			else if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return IO_AGAIN;
			}
			else {
				return IO_ERROR;
			}
		}
		else if (nRcv == 0)
		{
			return IO_CLOSED;
		}
		else {
			buf_len += nRcv;
			recv_len -= nRcv;
		}
	}
	
	return IO_FINISH;
}






