#include "address.h"

unsigned long long operator "" _kB(unsigned long long value)
{
	return value * 0x400;
}

unsigned long long operator "" _MB(unsigned long long value)
{
	return value * 0x10000;
}

AddressSide::AddressSide(const address_t& offset, const address_t& length) :
	_offset(offset),
	_length(length)
{}

void AddressSide::writeByte(const address_t& offset, const byte_t& value)
{
	if (offset >= _offset && offset - _offset < _length)
		_writeByte(offset - _offset, value);
}

void AddressSide::writeWord(const address_t& offset, const word_t& value)
{
	if (offset >= _offset && (offset + 1) - _offset < _length)
		_writeWord(offset - _offset, value);
}

byte_t AddressSide::readByte(const address_t& offset) const
{
	return offset >= _offset && offset - _offset < _length
		? _readByte(offset - _offset)
		: 0;
}

word_t AddressSide::readWord(const address_t& offset) const
{
	return offset >= _offset && (offset + 1) - _offset < _length
		? _readWord(offset - _offset)
		: 0;
}

bool AddressSide::validAddress(const address_t& offset) { return offset >= _offset && offset - _offset < _length; }
