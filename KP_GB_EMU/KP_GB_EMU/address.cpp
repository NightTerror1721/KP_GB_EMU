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

AddressSide::AddressSide(const address_t& offset, const address_t& length) :
	_offset(offset),
	_length(length)
{}

void AddressSide::writeByte(const address_t& offset, const byte_t& value)
{
	if (offset >= _offset && offset - _offset < _length)
	{
		_writeByte(offset - _offset, value);
	}
}

void AddressSide::writeWord(const address_t& offset, const word_t& value)
{
	if (offset >= _offset && (offset + 1) - _offset < _length)
	{
		_writeWord(offset - _offset, value);
	}
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

bool AddressSide::validAddress(const address_t& offset) const { return offset >= _offset && offset - _offset < _length; }
const address_t AddressSide::adaptOffset(const address_t& offset) const { return offset - _offset; }



RAM::RAM(const address_t& offset, const address_t& length) :
	AddressSide(offset, length),
	_mem(new byte_t[_length]),
	_shadow(false)
{}
RAM::RAM(const address_t& offset, const RAM& base) :
	AddressSide(offset, base._length),
	_mem(base._mem),
	_shadow(true)
{}


RAM::~RAM()
{
	if (!_shadow)
		delete[] _mem;
}

void RAM::_writeByte(const address_t& offset, const byte_t& value)
{
	_mem[offset] = value;
}

void RAM::_writeWord(const address_t& offset, const word_t& value)
{
	*reinterpret_cast<word_t*>(_mem + offset) = value;
}

byte_t RAM::_readByte(const address_t& offset) const
{
	return _mem[offset];
}

word_t RAM::_readWord(const address_t& offset) const
{
	return *reinterpret_cast<word_t*>(_mem + offset);
}

byte_t& RAM::operator[] (const address_t& offset)
{
	return _mem[offset];
}
const byte_t& RAM::operator[] (const address_t& offset) const
{
	return _mem[offset];
}

byte_t* RAM::ptr() { return _mem; }

void RAM::clear()
{
	std::memset(_mem, 0, sizeof(byte_t) * _length);
}

void RAM::dump(const unsigned int& columns)
{
	byte_t* p = _mem;

	std::cout << std::hex;
	for (address_t i = 0; i < _length; p++, i++)
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
