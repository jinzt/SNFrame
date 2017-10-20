#include "ThreadPool.h"
#include <Log.h>
#include <ConfigFileReader.h>
#ifndef  WIN32
#include <unistd.h>
#endif
#include <Recver.h>
ThreadPool::~ThreadPool()
{
	for (unsigned int i = 0; i < m_threads_vct.size(); i++)
	{
		destroy_cond(m_threads_vct[i]->m_cond);
		destroy_mutex(m_threads_vct[i]->m_mutex);
		delete m_threads_vct[i]->m_buf;
		delete m_threads_vct[i];
	}
	destroy_cond(m_threads_cond);
	destroy_mutex(m_threads_mutex);
}

ThreadPool::ThreadPool() 
{
	m_single = false;
	m_stop = false;
	init_mutex(m_threads_mutex);
	init_cond(m_threads_cond);
	m_recver = NULL;
	m_working_num = 0;//wake up num 
	m_threads_vct.clear();
	m_max_threads = 0;
	m_min_threads = 0;
	m_gradient = 0;
	m_index = 0;
}

int ThreadPool::addJob(context_t & context)
{
	if (m_single && m_recver)
	{
		m_recver->getData(context);
		return 0;
	}

	lock_mutex(m_threads_mutex);
	if (++m_index >= m_working_num)
	{
		m_index = 0;
	}
	thread_info *info = m_threads_vct[m_index];
	unlock_mutex(m_threads_mutex);


	lock_mutex(info->m_mutex);
	info->m_buf->push(context);
	signal_cond(info->m_cond);
	unlock_mutex(info->m_mutex);
	return 0;
}

int ThreadPool::init(Recver* recver, int max_threads, int min_threads
	/* = 10 */, int value /* = 4 */, bool single /* = false */)
{
	m_recver = recver;
	m_max_threads = max_threads;
	m_min_threads = min_threads;
	m_gradient = value;
	m_single = single;
	if (max_threads < min_threads)
	{
		min_threads = max_threads;
		m_gradient = 1;
	}
	return 1;
}
void ThreadPool::stop()
{
	m_stop = true;
	//stop all threads
}
void ThreadPool::start(bool automatic)
{
	//create no more than 32 threads
	if (automatic)
	{
		create_threads(m_max_threads > m_min_threads ? m_min_threads : m_max_threads);
		startThread();
	}
	else
	{
		create_threads(m_max_threads);
	}
}
void ThreadPool::create_threads(unsigned  num)
{
	lock_mutex(m_threads_mutex);

	for (unsigned int i = 0; i < num; i++)
	{
		thread_info *info = new thread_info();
		info->m_buf = new std::queue<context_t>();
		if(info != NULL)
		{
			info->m_thread_pool = this;
			info->m_work = (m_threads_vct.size() < m_min_threads);
			if(info->m_work)
				m_working_num++;
			create_thread(info->m_thread_id, NULL, threadProc, info);	
			init_mutex(info->m_mutex);
			init_cond(info->m_cond);
			m_threads_vct.push_back(info);
		}
	}
	unlock_mutex(m_threads_mutex);

}

void ThreadPool::run(void *arg)
{
	thread_info* pThread = (thread_info*)arg;
	if (pThread == NULL)
	{
		return ;
	}
	while (!m_stop)
	{
		//check should sleep?
		if(!pThread->m_work && pThread->m_buf->empty())
		{
			lock_mutex(m_threads_mutex);
			while(!pThread->m_work)
			{
				wait_cond(m_threads_cond,m_threads_mutex);
			}
			if (m_stop) {
				unlock_mutex(m_threads_mutex);
				exit_thread(pThread->m_thread_id);
			}
			unlock_mutex(m_threads_mutex);
		}

		
		lock_mutex(pThread->m_mutex);
		while (pThread->m_buf->empty()) {
			wait_cond(pThread->m_cond, pThread->m_mutex);
		}

		context_t context;
		context = pThread->m_buf->front();
		pThread->m_buf->pop();
		unlock_mutex(pThread->m_mutex);
		m_recver->getData(context);
	}
}
void ThreadPool::run()
{
	while(!m_stop)
	{
		unsigned int size = 0;
		for (unsigned int i = 0; i < m_threads_vct.size(); i++)
		{
			size += m_threads_vct[i]->m_buf->size();
			//printf("%d %d\n", m_threads_vct[i]->m_thread_id, m_threads_vct[i]->m_data->size());
		}	
		static unsigned int temp = size;
		if (m_max_threads > m_threads_vct.size())
		{
			create_threads((m_max_threads - m_threads_vct.size()) > m_min_threads
				? m_min_threads : (m_max_threads - m_threads_vct.size()));
		}
		else
		{
			//Tested  Can capture the rising or falling trend
			bool trend = (size - temp) % 100 > 3;
			//size > 3000 In order to reduce the level
			if ((trend  && (size > temp))|| size > 50000)
			{
				if (m_threads_vct.size() > m_working_num)
				{
					lock_mutex(m_threads_mutex);
					unsigned int temp_num = m_working_num;
					for (unsigned int i = temp_num; i < ((m_threads_vct.size() - temp_num) > m_gradient ?
						temp_num + m_gradient : m_threads_vct.size()); i++)
					{
						if (m_threads_vct[i]->m_work == false)
						{
							m_threads_vct[i]->m_work = true;

							m_working_num++;
						}
					}

					
					unlock_mutex(m_threads_mutex);
					broadcast_cond(m_threads_cond);

				}
			}
			// down
			if (trend && size < 10000 && size < temp && m_working_num > m_min_threads)
			{
			
				lock_mutex(m_threads_mutex);
				int temp_num = m_working_num;
				for (unsigned int i = temp_num; i < ((m_threads_vct.size() - temp_num) > m_gradient ?
					temp_num + m_gradient : m_threads_vct.size()); i++)
				{
					m_threads_vct[i]->m_work = false;
					m_working_num--;
				}
				unlock_mutex(m_threads_mutex);
			}
			temp = size;
		}

#ifdef WIN32	
		Sleep(CHECKTIME * 1000);
#else	
		sleep(CHECKTIME);
#endif
	}
	for (unsigned int i = 0; i < m_threads_vct.size(); i++)
	{
		if (m_threads_vct[i]->m_work == false)
		{
			m_threads_vct[i]->m_work = true;
		}
	}

	broadcast_cond(m_threads_cond);

}

#ifdef WIN32
DWORD WINAPI
#else
void*
#endif 
ThreadPool::threadProc(void* arg)
{
	thread_info* pThread = (thread_info*)arg;
	if (pThread != NULL &&
		pThread->m_thread_pool != NULL)
		pThread->m_thread_pool->run(arg);
	return NULL;
}

