#ifndef ERROR_H
#define ERROR_H

#include <stack>
#include <string>

namespace error {

class StackItem {
	public:
		StackItem( const char* file, unsigned int line, const std::string& msg ): m_file(file), m_line(line), m_message(msg) {}

		const char* file() const { return m_file; }
		unsigned int line() const { return m_line; }
		const std::string& message() const { return m_message; }

	private:
		const char* m_file;
		unsigned int m_line;
		std::string m_message;
};

typedef std::stack<StackItem> Stack;

Stack& globalStack();

}

#endif
