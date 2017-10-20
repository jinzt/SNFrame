#ifndef _STRUCT_BASE_H_
#define _STRUCT_BASE_H_
#include <time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <event.h>
//#include <hash_set>
#include <IMString.h>
#include <IM.BaseDefine.pb.h>
#include <Define.h>
#define BUFSIZE 128//初始化内存块大小
#ifdef WIN32
#pragma warning(disable: 4309)
#endif

/*typedef struct msg_s
{
    char*m_data;
	int m_len;
	int m_alloc;
}msg_t;*/
#define  msg_t  IMString
typedef struct context_s {
	msg_t      msg;
	void*      arg;
}context_t;

 bool  init(msg_t &msg);
 bool  bufalloc(msg_t &msg,int len);
 bool  deep_copy(msg_t& dest, const msg_t& msg);
 bool  resize(msg_t &msg, int new_lenth);
 void  buffree(msg_t &msg);
 bool  check(msg_t &msg);

/**
* @brief  设置头部标志 找到用户连着那个链接服务器
* @param  srv_id 服务节点
* @param  sock_id该用户的链接标识符
*/
 void   set_tag(msg_t &msg, uint32_t srv_id, uint32_t sock_id);

/**
* @brief src头部信息拷贝到dst
* @param  cid 设置新头部cmd id
* @param  len  pb长度
*/

 bool  new_head(msg_t& dst, msg_t& src,uint16_t cid, uint32_t len);

  void  set_body_head(msg_t &msg, uint16_t number, uint16_t sid, uint16_t cid, char verssion = 1);

  bool  set_data(msg_t &msg, const char* buf, uint16_t len);

  /**
  * @brief 将data数据序列化到msg中
  */
  bool  set_pb(msg_t &msg, const google::protobuf::MessageLite* data);

  bool set_checksum(msg_t& msg);

 char*  get_data(msg_t &msg);

 uint32_t   get_company_id(msg_t &msg);

 uint16_t get_packge_number(msg_t &msg);

 /**
 * @brief 从msg二进制数据中计算pb body长度 用于parse packet 
 */
 uint16_t get_data_len(msg_t &msg);

 /**
 * @brief 从完整的数据包中获取pb长度
 */
 uint32_t data_len(msg_t& msg);

 /**
 * @brief 获取协议id  sid<<16 | cid
 */
 uint32_t get_protocl(msg_t &msg);

 uint16_t get_sid(msg_t& msg);

 uint16_t get_cmd(msg_t& msg);

 uint16_t  get_msg_id(msg_t& msg);
 char get_version(msg_t &msg);

 uint32_t get_srv_id(msg_t &msg);
 uint32_t get_socket_id(msg_t& msg);

#endif
