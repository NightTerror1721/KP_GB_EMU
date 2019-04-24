#include "mmu.h"

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
	if(!_shadow)
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





MMU::MMU() :
	_invalidAddress(),
	_ram(0xC000, static_cast<address_t>(8_kB)),
	_shadowRam(0xE000, _ram)
{}

void MMU::dumpInternalRam(const unsigned int& columns) { _ram.dump(columns); }

AddressSide& MMU::findSide(const address_t& offset)
{
	switch (offset & 0xF000)
	{
		/* ROM bank #0 */
		case 0x0000:
		case 0x1000:
		case 0x2000:
		case 0x3000:
			break;

		/* switchable ROM bank */
		case 0x4000:
		case 0x5000:
		case 0x6000:
		case 0x7000:
			break;

		/* Video RAM */
		case 0x8000:
		case 0x9000:
			break;

		/* switchable RAM bank */
		case 0xA000:
		case 0xB000:
			break;

		/* Internal RAM */
		case 0xC000:
		case 0xD000:
			return _ram;

		/* Echo of Internal RAM (0xE000 to 0xFDFF) */
		case 0xE000:
		case 0xF000:
			if (offset < 0xFE00)
				return _shadowRam;
			else
			{

			}
			break;
			break;
	}

	return _invalidAddress;
}

#define SIDE_REF(offset) this->findSide((offset))
#define CSIDE_REF(offset) const_cast<MMU*>(this)->findSide((offset))

void MMU::writeByte(const address_t& offset, const byte_t& value)
{
	SIDE_REF(offset).writeByte(offset, value);
}

void MMU::writeWord(const address_t& offset, const word_t& value)
{
	SIDE_REF(offset).writeWord(offset, value);
}

byte_t MMU::readByte(const address_t& offset) const
{
	return CSIDE_REF(offset).readByte(offset);
}

word_t MMU::readWord(const address_t& offset) const
{
	return CSIDE_REF(offset).readWord(offset);
}

