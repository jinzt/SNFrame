
#pragma once
#include <PbBuf.h>
#include <Connection.h>
#include <Recver.h>
#include <NetManager.h>
#define RECV_BUF_LEN          16*1024

typedef void(*timer_handler)(void);
class  NetServer
{
public:
	virtual ~NetServer();
	NetServer(int type);

	/**
	* @brief 初始化监听服务器
	* @param port 监听端口
	* @param ip 监听地址
	*/
	int  init(int port, const char* ip = NULL);
	
	/**
	* 启动线程池
	*/
	int prepare();

	/**
	* @brief 开始事件循环
	*/
	void start();

	/**
	* @brief 停止事件循环
	*/
	void stop();
	

	Connection* connect( const char* ip, int port);
	void accept(int listener);
	void close(int fd);
	
	/**
	* @brief 取得端口
	*/
	int get_port();

	/**
	* @brief 取得ip
	*/
	std::string get_ip();

public:
	virtual void initContext(Connection* conn)=0;
	/**
	*@brief 写事件回调
	*/
	virtual void write(Connection * conn)=0;
	/**
	*@brief 读事件回调
	*/
	virtual void read(Connection* conn)=0;

	/**
	* @brief 解包
	*/
	virtual void parse(Connection* conn) = 0;

	virtual void handler(msg_t& msg,int id) = 0;

	/**
	* @brief 写事件回调的函数--（子类继承之后实现此接口 则会被调用）
	* @param    sig 触信号读事件的链接
	* @param    events 事件类型
	* @return   true or false
	*/
	//virtual bool signal(evutil_socket_t sig, short events, void *arg) = 0;

public:

	int  m_port;
	char m_ip[64];
	
	Recver           m_recver;  
	NetManager*      m_pNetMgr;
	int              m_type;
protected:
	
};
