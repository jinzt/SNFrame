#include "ConnNode.h"

ConnNode::ConnNode()
{
	m_sid = IM::BaseDefine::ServiceID::SID_CONN;
}

ConnNode::~ConnNode()
{
}

int ConnNode::init()
{
	return 0;
}

void ConnNode::parse(msg_t & msg)
{
}
