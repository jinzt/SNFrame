#include "ServerStatus.h"
#include "Config.h"
#include <Log.h>
#include <string>
#ifdef WIN32
#pragma warning(disable: 4996)
#else
#include <netdb.h>
#endif
#define  MAX(a, b, c) a > b ? (a > c ? a : c) : (b > c ? b : c)
ServerStatus::ServerStatus()
{

}
ServerStatus::~ServerStatus()
{
}
std::string ServerStatus::get_host_ip()
{
	char host_name[256];
	int WSA_return;
	static int i = 0;
	struct hostent *host_entry;
	const char *pszAddr = NULL;
#ifdef WIN32
	WSADATA WSAData;
	WORD wVersionRequested;
	wVersionRequested = MAKEWORD(2, 0);
	WSA_return = WSAStartup(wVersionRequested, &WSAData);
#else
	WSA_return = 0;
#endif
	if (WSA_return == 0)
	{
		gethostname(host_name, sizeof(host_name));
		host_entry = gethostbyname(host_name);
		if (host_entry != NULL && host_entry->h_addr_list[i] != NULL)
		{
			pszAddr = inet_ntoa(*(struct in_addr *)host_entry->h_addr_list[i++]);
			//printf("[IP]%s\n[Name]%s\n", pszAddr, host_name);
		}
		else if (host_entry != NULL && host_entry->h_addr_list[i] == NULL)
		{
			i = 0;
		}
	}
	return pszAddr;
}
char ServerStatus::get_percent()
{
	get_sys_info();
	return  MAX(m_cpu_used, ((m_disk_tatol - m_disk_free) * 100 / m_disk_tatol),
		((m_mem_tatol - m_mem_free) * 100 / m_mem_tatol));
}
void ServerStatus::get_sys_info()
{
	m_mem_tatol = 0;
	m_mem_free = 0;
	m_disk_tatol = 0;
	m_disk_free = 0;
	m_cpu_used = 0;
#ifdef WIN32
	//cpu
	BOOL res;
	FILETIME preidleTime;
	FILETIME prekernelTime;
	FILETIME preuserTime;
	FILETIME idleTime;
	FILETIME kernelTime;
	FILETIME userTime;
	res = GetSystemTimes(&idleTime, &kernelTime, &userTime);
	preidleTime = idleTime;
	prekernelTime = kernelTime;
	preuserTime = userTime;
	Sleep(100);
	res = GetSystemTimes(&idleTime, &kernelTime, &userTime);
	__int64 idle = CompareFileTime(preidleTime, idleTime);
	__int64 kernel = CompareFileTime(prekernelTime, kernelTime);
	__int64 user = CompareFileTime(preuserTime, userTime);
	m_cpu_used = char((kernel + user - idle) * 100 / (kernel + user));//单位%
	//disk
	UINT64				m_i64TotalBytes;
	UINT64				m_i64FreeBytes;
	UINT64				m_i64FreeBytesToCaller;
	BOOL fResult = GetDiskFreeSpaceEx(
		NULL,
		(PULARGE_INTEGER)&m_i64FreeBytesToCaller,
		(PULARGE_INTEGER)&m_i64TotalBytes,
		(PULARGE_INTEGER)&m_i64FreeBytes);
	m_disk_tatol = m_i64TotalBytes / 1024;
	m_disk_free = m_i64FreeBytes / 1024;
	//mem
	MEMORYSTATUSEX		statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	m_mem_tatol = statex.ullTotalPhys/1024;
	m_mem_free =  statex.ullAvailPhys/1024;
#else
	//mem
	FILE *fd;          
	char buff[256];
	char name[24];
	fd = fopen("/proc/meminfo", "r");
	fgets(buff, sizeof(buff), fd);
	sscanf(buff, "%s %u %s", name, &m_mem_tatol, name);
	fgets(buff, sizeof(buff), fd); 
	sscanf(buff, "%s %u", name, &m_mem_free, name);
	fclose(fd);
	//cpu
	CPU_OCCUPY  cpu_o, cpu_n;
	get_cpu_info(&cpu_o);
	usleep(100);
	get_cpu_info(&cpu_n);
	m_cpu_used = get_cpu_used(&cpu_o, &cpu_n);
	//disk
	struct statfs diskInfo;
	statfs("/", &diskInfo);
	m_disk_tatol = (diskInfo.f_bsize * diskInfo.f_blocks)/1024;
	m_disk_free = (diskInfo.f_bfree*diskInfo.f_bsize)/1024;
#endif
}
#ifdef WIN32
long long ServerStatus::CompareFileTime(FILETIME time1, FILETIME time2)
{
	long long a = (unsigned __int64)time1.dwHighDateTime << 32 | time1.dwLowDateTime;
	long long b = (unsigned __int64)time2.dwHighDateTime << 32 | time2.dwLowDateTime;
	return   (b - a);
}
#endif
void ServerStatus:: get_cpu_info(CPU_OCCUPY *cpu)
{
	FILE *fd;
	char buff[256];
	fd = fopen("/proc/stat", "r");
	fgets(buff, sizeof(buff), fd);
	sscanf(buff, "%s %u %u %u %u", cpu->name, &cpu->user, &cpu->nice, &cpu->system, &cpu->idle,&cpu->irq,&cpu->sorftirq);
	fclose(fd);
}
int ServerStatus::get_cpu_used(CPU_OCCUPY *cpu_old, CPU_OCCUPY *cpu_new)
{
	unsigned long od, nd;
	unsigned long idle;
	int cpu_use = 0;
	od = (unsigned long)(cpu_old->user + cpu_old->nice + cpu_old->system + cpu_old->idle);
	nd = (unsigned long)(cpu_new->user + cpu_new->nice + cpu_new->system + cpu_new->idle);
	idle = (unsigned long)(cpu_new->idle - cpu_old->idle);
	if ((nd - od) != 0)
		cpu_use = 100 - (int)( idle * 100) / (nd - od);
	else cpu_use = 0;
	return cpu_use;
}
long long ServerStatus::get_disk_tatol()
{
	return m_disk_tatol;
}
long long ServerStatus::get_disk_used()
{
	return m_disk_tatol - m_disk_free;
}
long long ServerStatus::get_disk_free()
{
	return m_disk_free;
}

int ServerStatus::get_cpu()
{
	return m_cpu_used;
}
long long ServerStatus::get_mem_tatol()
{
	return m_mem_tatol;
}
long long ServerStatus::get_mem_used()
{
	return m_mem_tatol - m_mem_free;
}
long long ServerStatus::get_mem_free()
{
	return m_mem_free;
}
