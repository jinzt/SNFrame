#pragma once
#include <Lock.h>
class IMString {
public:
	IMString();
	~IMString();
	IMString(const IMString& rhs);
	IMString& operator=(IMString& rhs);
public:
	char* m_data;
	int m_alloc;
	int m_len;
};
