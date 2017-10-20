#include "IMString.h"
#include <stdlib.h>

IMString::IMString()
{
	m_data =NULL;
	m_len = 0;
	m_alloc = 0;
}

IMString::~IMString()
{
	if (m_data) {
		free(m_data);
		m_data=NULL;
	}
}

IMString::IMString( const IMString & rhs)
{
	if (this != &rhs) {
		m_data = rhs.m_data;
		m_len = rhs.m_len;
		m_alloc = rhs.m_alloc;
		char** tmp=const_cast<char**>(&rhs.m_data) ;
		*tmp=NULL;
	}
}

IMString& IMString::operator=(IMString & rhs)
{
	if (this != &rhs) {
		if (m_data) {
			free(m_data);
		}
		m_data = rhs.m_data;
		m_len = rhs.m_len;
		m_alloc = rhs.m_alloc;
		rhs.m_data = NULL;
	}
	return *this;
}

