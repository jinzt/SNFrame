#ifndef  _MSG_PROCESS_H
#define   _MSG_PROCESS_H
#include <ThreadBase.h>
#define CHECKTIME 10
#include <PbBuf.h>
#include <queue>
#include <Lock.h>
class Recver;
class ThreadPool:public ThreadBase
{
	struct thread_info
	{
		bool 	 m_work;
		ThreadPool* m_thread_pool;
		thread_t m_thread_id;
		std::queue<context_t>* m_buf;
		mutex_t	m_mutex;
		cond_t	m_cond;
	};

public:
	#ifdef WIN32
	static DWORD WINAPI
#else
	static void*
#endif
		threadProc(void* arg);
	~ThreadPool();
	ThreadPool();
	int init(Recver* recver, int max_threads, int min_threads = 10,
		int value = 4, bool single = false);
	int addJob(context_t& context);
	void start(bool automatic = true);
	void stop();
private:
	void create_threads(unsigned  num);
	void run();
	void run(void* arg);
	bool                      m_stop;
	unsigned int              m_max_threads;
	Recver*                   m_recver;
	unsigned int	m_index;
	mutex_t  	  m_threads_mutex;
	cond_t  	  m_threads_cond;
	unsigned int  m_working_num;//wake up num
	std::vector<thread_info*>  m_threads_vct;
	unsigned int m_min_threads;
	unsigned int m_gradient;
	bool m_single;
};
#endif
