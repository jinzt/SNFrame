#ifndef _CONNNODE_H_
#define _CONNNODE_H_
#include <ServiceNode.h>

class ConnNode : public ServiceNode {
public:
	ConnNode();
	~ConnNode();
	int init();
	void parse(msg_t& msg);
};
#endif