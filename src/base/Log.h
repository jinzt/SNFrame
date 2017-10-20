#ifndef _IM_LOG_H
#define _IM_LOG_H

#define ZONE     __FILE__,__LINE__
#define MAX_DEBUG_LEN            2048

//日志类型
typedef enum
{	
	LOG_DEBUG =0,
	LOG_WARN ,
	LOG_ERROR,
	LOG_CRIT,
	LOG_FATAL 
}LOGLEVEL;

int initLog(const char* filename);
void logDebug(LOGLEVEL level, const char* filename,int line,const char* format, ...);//最大长度2k 超出不打印
#define LogMsg( level, format,...)  logDebug(level,ZONE,format,##__VA_ARGS__)

#endif
