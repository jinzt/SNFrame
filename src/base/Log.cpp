#include<log4cpp/Category.hh>
#include<log4cpp/OstreamAppender.hh>
#include<log4cpp/Priority.hh>
#include<log4cpp/PatternLayout.hh>
#include<log4cpp/FileAppender.hh>
#include<log4cpp/Category.hh>
#include<log4cpp/PropertyConfigurator.hh>
#include "Log.h"
#include <stdarg.h>
#include <Config.h>
#include <string>
#include <cstring>
#include<Define.h>

static log4cpp::Category* s_log=NULL;

int initLog(const char* filename)
{
	try
	{
		log4cpp::PropertyConfigurator::configure(filename);//导入配置文件
		log4cpp::Category& log = log4cpp::Category::getInstance(std::string("error"));
		s_log = &log;
		if (!s_log)
		{
			fprintf(stderr, "Configure error\n");
			return IM_ERR;
		}
	}
	catch (log4cpp::ConfigureFailure& f)
	{
		fprintf(stderr, "Configure Problem: %s\n", f.what());
		return IM_ERR;
	}
	return IM_OK;
}

void  logDebug(LOGLEVEL level, const char* filename, int line, const char* format, ...)
{
	if(!s_log){
		return;
	}
	char buf[MAX_DEBUG_LEN + 1] = { 0 };
    int num=0;
#ifdef WIN32
	const char* p = strrchr(filename, '\\');
	if (NULL == p) {
		return;
	}
	num = sprintf(buf, "%s:%d ", p+1, line);
#else
	const char* p = strrchr(filename, '/');
	if (NULL == p) {
		return;
	}
    num=sprintf(buf, "%s:%d ", p+1, line);
#endif
	va_list vl;
	va_start(vl, format); 
	vsnprintf(buf+num, MAX_DEBUG_LEN-num,format,vl);
	va_end(vl);
	printf("%s\n", buf);
	std::string msg(buf);
	switch (level)
	{
	case LOG_FATAL:
		s_log->fatal(msg);
		break;
	case LOG_CRIT:
		s_log->crit(msg);
		break;
	case LOG_ERROR:
		s_log->error(msg);
		break;
	case LOG_WARN:
		s_log->warn(msg);
		break;
	case LOG_DEBUG:
		s_log->debug(msg);
		break;
	default:
		s_log->debug(msg);
		break;
	}
}
