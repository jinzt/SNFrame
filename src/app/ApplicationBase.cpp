#include <ApplicationBase.h>
#include <Log.h>
ApplicationBase* ApplicationBase::m_app = NULL;
ApplicationBase::ApplicationBase(){
	m_pHandler = PbProtocolHandler::getInstance();
	m_app = NULL;
}
ApplicationBase::~ApplicationBase(){
}
ApplicationBase* ApplicationBase::getInstance(){
	if (!m_app) {
		 m_app=new ApplicationBase;
	}
	return m_app;
}

void ApplicationBase::regist(uint16_t sid, uint16_t cid, protocolHandler * handler)
{
	m_pHandler->regist(sid,cid, handler);
}

void ApplicationBase::super(ApplicationBase * base)
{
	m_app = base;
}

int ApplicationBase::init(){
	LogMsg(LOG_WARN, "you maybe not regist protocol");
	return -1;
}
