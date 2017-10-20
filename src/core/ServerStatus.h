/*================================================================
*   Copyright (C) 2016 All rights reserved.
*
*   filename：ServerStatus.h
*   author：Jiang Canjun
*   create date：2016-7-5
================================================================*/
#pragma  once
#include <string>
#ifdef WIN32
#include <Windows.h>
#endif
class ServerStatus
{
	typedef struct          
	{
		char name[20];
		unsigned int user;
		unsigned int nice;
		unsigned int system;
		unsigned int idle;
		unsigned int irq;
		unsigned int sorftirq;
	}CPU_OCCUPY;
public:
	/***************************************************************
	*函数名：   get_host_ip
	*返回值：	string
	*函数功能： 获取系统ip
	*****************************************************************/
	std::string get_host_ip();
	/***************************************************************
	*函数名：   get_percent
	*返回值：	char (%)
	*函数功能： 获取系统当前信息 （cpu mem disk 综合百分比）
	*****************************************************************/
	char get_percent();
	ServerStatus(); 
	~ServerStatus();
	/***************************************************************
	*函数名：   get_sys_info
	*返回值：
	*函数功能： 获取系统当前信息 （cpu mem disk）
	*****************************************************************/
	void get_sys_info();
	/***************************************************************
	*函数名：   get_disk_tatol
	*返回值：	long long （kb）
	*函数功能： 返回磁盘总量（注意程序所在的磁盘）
	*****************************************************************/
	long long get_disk_tatol();
	/***************************************************************
	*函数名：   get_disk_tatol
	*返回值：	long long（kb）
	*函数功能： 返回磁盘使用量（注意程序所在的磁盘）
	*****************************************************************/
	long long get_disk_used();
	/***************************************************************
	*函数名：   get_disk_free
	*返回值：	long long（kb）
	*函数功能： 返回磁盘空余量（注意程序所在的磁盘）
	*****************************************************************/
	long long get_disk_free();
	/***************************************************************
	*函数名：   get_cpu
	*返回值：	double
	*函数功能： 返回cpu使用率
	*****************************************************************/
	int get_cpu();
	/***************************************************************
	*函数名：   get_mem_tatol
	*返回值：	long long（kb）
	*函数功能： 返回内存总量
	*****************************************************************/
	long long get_mem_tatol();
	/***************************************************************
	*函数名：   get_mem_used
	*返回值：	long long（kb）
	*函数功能： 返回内存使用量
	*****************************************************************/
	long long get_mem_used();
	/***************************************************************
	*函数名：   get_mem_free
	*返回值：	long long（kb）
	*函数功能： 返回内存空余量
	*****************************************************************/
	long long get_mem_free();
private:
#ifdef WIN32
	long long CompareFileTime(FILETIME time1, FILETIME time2);
#endif
	void get_cpu_info(CPU_OCCUPY *cpu);
	int get_cpu_used(CPU_OCCUPY *cpu_old, CPU_OCCUPY *cpu_new);
	
	int m_cpu_used;// %
	long long m_disk_tatol;//kb
	long long m_disk_free;//kb
	long long m_mem_tatol;//kb
	long long m_mem_free;//kb
};
