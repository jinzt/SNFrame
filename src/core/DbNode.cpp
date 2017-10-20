#include "DbNode.h"

DbNode::DbNode()
{
	m_sid = IM::BaseDefine::ServiceID::SID_DB_PROXY;
}

DbNode::~DbNode()
{
}

int DbNode::init()
{
	return 0;
}

void DbNode::parse(msg_t & msg)
{
}
