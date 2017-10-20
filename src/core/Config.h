#pragma once
#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#include <Wininet.h>
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "Wininet.lib")
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#else
#include<netinet/in.h>
#include<sys/socket.h>
#include<unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/vfs.h>
#endif
#include <sys/types.h>
#include <event2/event-config.h>
#include <sys/stat.h>
#include <time.h>
#ifdef _EVENT_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

#define CONFIG_PATH "../config/IMFrame.conf"
