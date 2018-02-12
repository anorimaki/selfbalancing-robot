#include "error.h"

namespace error
{

Stack& globalStack()
{
	static Stack ret;
	return ret;
}

}
