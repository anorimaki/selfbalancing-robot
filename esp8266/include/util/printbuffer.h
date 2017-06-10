#ifndef INCLUDE_UTIL_PRINTBUFFER_H_
#define INCLUDE_UTIL_PRINTBUFFER_H_

#include "Print.h"

namespace io
{

template <size_t SIZE>
class PrintBuffer: public Print {
public:
	static const size_t Size = SIZE;

public:
	PrintBuffer( Print* impl ): m_impl(impl), m_pos(0) {
		m_buf = new byte[Size];
	}

	~PrintBuffer() {
		delete[] m_buf;
	}

	void flush() {
		m_impl->write( m_buf, m_pos );
		m_pos = 0;
	}
/*
	virtual size_t write(const uint8_t *buffer, size_t size) {
		while( )
		size_t toCopy = std::min( Size-m_pos, size );
		memcpy( &buf[m_pos], buffer, toCopy );
		m_pos += toCopy;
		flush();

		memcpy( &buf[m_pos], buffer, toCopy );
	}
*/
	virtual size_t write( uint8_t c )  {
		m_buf[m_pos++] = c;
		if ( m_pos==Size ) {
			flush();
		}
		return 1;
	}


private:
	Print* m_impl;
	//uint8_t m_buf[Size];
	uint8_t* m_buf;
	size_t m_pos;
};

}

#endif /* INCLUDE_UTIL_PRINTBUFFER_H_ */
