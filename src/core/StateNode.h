#ifndef _STATENODE_H_
#define _STATENODE_H_
#include <ServiceNode.h>

class StateNode : public ServiceNode {
public:
	StateNode();
	~StateNode();
	int init();
	void parse(msg_t& msg);
};
#endif