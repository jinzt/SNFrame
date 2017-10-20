#include "StateNode.h"

StateNode::StateNode()
{
	m_sid = IM::BaseDefine::ServiceID::SID_USER_STATE;
}

StateNode::~StateNode()
{
}

int StateNode::init()
{
	return 0;
}

void StateNode::parse(msg_t & msg)
{
}
