#include "address.h"

#include <iostream>

unsigned long long operator "" _kB(unsigned long long value)
{
	return value * 0x400;
}

unsigned long long operator "" _MB(unsigned long long value)
{
	return value * 0x10000;
}

bool addr_range(const address_t& min, const address_t& current, const address_t& max)
{
	return min <= current && current <= max;
}



RAM::RAM(const size_t& size) :
	DefaultMemory(ByteWordListener(), size, true)
{}
RAM::RAM(const RAM& base) :
	DefaultMemory(ByteWordListener(), base._mem, base._size, false)
{}
