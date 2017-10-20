#include "HttpServer.h"

HttpServer::HttpServer():NetServer(CONN_HTTP)
{
}

HttpServer::~HttpServer()
{
}

int HttpServer::http_on_url(http_parser *parser, const char *buf, size_t len)
{
	SOCKINFO *conn = static_cast<SOCKINFO *>(parser->data);
	return 0;
}
int HttpServer::http_on_headers_complete(http_parser *parser)
{
	SOCKINFO *conn = static_cast<SOCKINFO *>(parser->data);

	return 0;
}
int HttpServer::http_on_body(http_parser *parser, const char *buf, size_t len)
{
	SOCKINFO *conn = static_cast<SOCKINFO *>(parser->data);
	send(conn->m_fd, buf, len, 0);
	return 0;
}
int HttpServer::http_on_message_complete(http_parser *parser)
{
	SOCKINFO *conn = static_cast<SOCKINFO *>(parser->data);
	return 0;
}
void HttpServer::initContext(Connection * conn)
{
	if (conn)
	{
		/*³õÊ¼»¯http½âÎöÆ÷*/
		SOCKINFO* info = new SOCKINFO();
		if (!info)
		{
			return ;
		}
		http_parser_settings_init(&info->m_settings);
		info->m_settings.on_url = http_on_url;
		info->m_settings.on_headers_complete = http_on_headers_complete;
		info->m_settings.on_body = http_on_body;
		info->m_settings.on_message_complete = http_on_message_complete;
		info->m_fd = conn->fd;
		info->m_parser.data = static_cast<void *>(info);
		http_parser_init(&info->m_parser, HTTP_BOTH);
		conn->context = info;
		conn->type = CONN_HTTP;
	}
	return ;
}

void HttpServer::write(Connection * conn)
{
	return;
}

void HttpServer::parse(Connection * conn)
{
	context_t  context;
	m_recver.addData(context);
}

void HttpServer::read(Connection * conn)
{
	return ;
}

void HttpServer::handler(msg_t & msg,int id)
{
	return;
}
