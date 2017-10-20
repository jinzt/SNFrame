#ifndef _DEFINE_H__
#define _DEFINE_H__
#include <stdint.h>

#define   IM_ERR       -1
#define   IM_OK        0


#define           TAG_HEAD_1    0x7e 
#define           TAG_HEAD_2    0xaa
#define           BODY_HEAD_1   102
#define           BODY_HEAD_2   35
#define           HEAD_LEN      27

#ifdef WIN32
#define snprintf  _snprintf
#define sleep(sec)     Sleep(1000*(sec))
#endif
#endif
