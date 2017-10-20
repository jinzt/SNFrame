#ifndef _HTTPSERVER_H
#define _HTTPSERVER_H
#include "HttpParser.h"
#include <NetServer.h>
#include <PbBuf.h>
#include "ServerStatus.h"
#include <Log.h>
#include <Lock.h>

typedef struct http_socket
{
	short m_peer_port;
	int m_fd;
	http_parser m_parser;                          //http½âÎöÆ÷
	http_parser_settings m_settings;
	std::string m_path;
	std::string m_peer_ip;
}SOCKINFO;

class MsgHandler;
class HttpServer : public NetServer
{
public:
	HttpServer();
	~HttpServer();
	static int http_on_url(http_parser *parser, const char *buf, size_t len);
	static int http_on_headers_complete(http_parser *parser);
	static int http_on_body(http_parser *parser, const char *buf, size_t len);
	static int http_on_message_complete(http_parser *parser);

public:
	void initContext(Connection* conn);
	void write(Connection* conn);
	void read(Connection* conn);
	void parse(Connection* conn);
	void handler(msg_t& msg,int id);
private:
	unsigned int                 m_type;

};
#endif
