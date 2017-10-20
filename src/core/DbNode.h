#ifndef _DBNODE_H
#define _DBNODE_H
#include <ServiceNode.h>
class DbNode:public ServiceNode {
public:
	DbNode();
	~DbNode();
	int init();
	void parse(msg_t& msg);
};

#endif
