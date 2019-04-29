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

void AddressSide::writeByte(const size_t& offset, const byte_t& value)
{
	(*this)[offset] = value;
}

void AddressSide::writeWord(const size_t& offset, const word_t& value)
{
	(*this)[offset].as_word() = value;
}

byte_t AddressSide::readByte(const size_t& offset) const
{
	return (*this)[offset];
}

word_t AddressSide::readWord(const size_t& offset) const
{
	return (*this)[offset].as_word();
}

void AddressSide::onMmuWrite(const address_t& address, const byte_t& value) {}
void AddressSide::onMmuWrite(const address_t& address, const word_t& value) {}

ByteAddressAccessor AddressSide::operator* () { return operator[](0); }
ConstByteAddressAccessor AddressSide::operator* () const { return operator[](0); }



RAM::RAM(const size_t& size) :
	AddressSide(),
	_size(size),
	_mem(new byte_t[_size]),
	_shadow(false)
{}
RAM::RAM(const RAM& base) :
	AddressSide(),
	_mem(base._mem),
	_size(base._size),
	_shadow(true)
{}


RAM::~RAM()
{
	if (!_shadow)
		delete[] _mem;
}

ByteAddressAccessor RAM::operator[] (const size_t& offset)
{
	return { _mem + offset };
}
const ConstByteAddressAccessor RAM::operator[] (const size_t& offset) const
{
	return { _mem + offset };
}

void RAM::clear()
{
	std::memset(_mem, 0, sizeof(byte_t) * _size);
}

void RAM::dump(const unsigned int& columns)
{
	byte_t* p = _mem;

	std::cout << std::hex;
	for (address_t i = 0; i < _size; p++, i++)
	{
		if (!(i % columns) && i)
			std::cout << std::endl;
		std::cout << static_cast<unsigned int>(*p);
	}
	std::cout << std::dec << std::endl;
}

void RAM::reset()
{
	if (!_shadow)
		clear();
}
