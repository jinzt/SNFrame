#include "Node.h"
#include <cstring>

Node::Node(uint16_t sid,uint32_t nid)
{
	m_num = 0;
	m_type = sid;
	m_id = nid;
	memset(m_ip, 0, sizeof(m_ip));
	m_state = NODE_UNABLE;
	m_context = NULL;
	m_persist = true;
}

uint32_t Node::getNodeId()
{
	return m_id;
}

int Node::getState()
{
	return m_state;
}
uint16_t Node::getSid()
{
	return m_type;
}
void Node::setState(int state)
{
	if (m_state == NODE_AUTH) {
		m_num = 0;
	}

	m_state = state;
}

void Node::setType(uint16_t type)
{
	type = m_type;
	
}

uint16_t Node::getType()
{
	return m_type;
}

int Node::setAddrs(const char * ip, uint16_t port)
{
	m_port = port;
	strcpy(m_ip, ip);
	return 0;
}

void Node::setContext(void * context)
{
	m_context = context;
}

void * Node::getContext()
{
	return m_context;
}

char * Node::getIp()
{
	return m_ip;
}

uint16_t Node::getPort()
{
	return m_port;
}

bool Node::needReconnect()
{
	return m_persist && m_num++ < 4;
}

void Node::setPersistConnect(bool persist)
{
	m_persist = persist;
}

void Node::clear()
{
	m_num = 0;
	m_state = NODE_UNABLE;
	m_context = NULL;
	memset(m_ip, 0, sizeof(m_ip));
}
