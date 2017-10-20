#pragma once
#include <ProtocolHandler.h>
#include <PbBuf.h>
#include <ThreadPool.h>
#include <ConfigFileReader.h>
class Recver {
public:
	Recver(int type);
	void addData(context_t&  context);
	bool handler(context_t& context);
	void getData(context_t& context);
	int start();
private:
	int                 m_num;//thread num
	int                 m_type;
	ThreadPool          m_pool;
	ProtocolHandler*    m_pHandler;

};
