#include "Recver.h"
 
extern  uint32_t recv_chat_pkts;
Recver::Recver(int type)
{
	m_num = 0;
	m_pHandler = getProtocolHandler(type);
}

void Recver::addData(context_t & context)
{
	if (m_num) {
		m_pool.addJob(context);
	}
	else {
		handler(context);
	}
}

bool Recver::handler(context_t & context)
{
	++recv_chat_pkts;
	m_pHandler->handler(context);
	return false;
}

void Recver::getData(context_t & context)
{
	handler(context);
}

int Recver::start()
{
	char* process_thread = CConfigFileReader::getInstance()->getConfigName("ProcessThread");
	if (process_thread != NULL) {
		m_num = atoi(process_thread);
		if (m_num) {
			m_pool.init(this, m_num);
			m_pool.start(false);
		}
	}
	return 0;
}


