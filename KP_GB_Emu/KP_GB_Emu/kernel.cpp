#include "kernel.h"



Kernel::Kernel()
{
}


Kernel::~Kernel()
{
}






/* Error control */
void Kernel::push_error(const std::string error)
{
	_errs.push_back(error);
}

void Kernel::foreachError(void(*consumer)(std::string&)) const
{
	for (std::string err : _errs)
		consumer(err);
}
