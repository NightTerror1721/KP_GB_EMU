#include "basics.h"

RAW_DATA::RAW_DATA(const size_t length) : length(length)
{
	this->data = new RAW_VALUE[length];
}

RAW_DATA::~RAW_DATA()
{
	delete[] data;
}
