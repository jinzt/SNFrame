#ifndef _APPLICATION_H
#define _APPLICATION_H

#include <PbProtocolHandler.h>
class ApplicationBase{
public:
	~ApplicationBase();
	static ApplicationBase* getInstance();
	void regist(uint16_t sid,uint16_t cid, protocolHandler* handler);
    void super(ApplicationBase* base);
	virtual int init();
public:
	ApplicationBase();
private:
	ProtocolHandler* m_pHandler;
	static ApplicationBase*  m_app;
};
        
#endif
