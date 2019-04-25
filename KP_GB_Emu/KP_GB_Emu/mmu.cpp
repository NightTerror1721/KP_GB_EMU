#include "mmu.h"

#include <iostream>


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

void RAM::reset()
{
	if (!_shadow)
		clear();
}





MMU::MMU(const bool& gbc_bios) :
	_invalidAddress(),
	_ram(0xC000, static_cast<address_t>(8_kB)),
	_shadowRam(0xE000, _ram),
	_bios(gbc_bios ? Bios::gbc() : Bios::gb()),
	_biosMode(true)
{}

void MMU::dumpInternalRam(const unsigned int& columns) { _ram.dump(columns); }

AddressSide& MMU::findSide(const address_t& offset)
{
	switch (offset & 0xF000)
	{
		/* ROM bank #0 */
		case 0x0000:
			if (_biosMode)
			{
				if (offset < 0x100)
					return *_bios;
				else if (offset < 0x800 && _bios->isGBC())
					return *_bios;
			}
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

void MMU::reset()
{
	_ram.reset();


	writeByte(0xFF05, 0);
	writeByte(0xFF06, 0);
	writeByte(0xFF07, 0);
	writeByte(0xFF10, 0x80);
	writeByte(0xFF11, 0xBF);
	writeByte(0xFF12, 0xF3);
	writeByte(0xFF14, 0xBF);
	writeByte(0xFF16, 0x3F);
	writeByte(0xFF17, 0x00);
	writeByte(0xFF19, 0xBF);
	writeByte(0xFF1A, 0x7A);
	writeByte(0xFF1B, 0xFF);
	writeByte(0xFF1C, 0x9F);
	writeByte(0xFF1E, 0xBF);
	writeByte(0xFF20, 0xFF);
	writeByte(0xFF21, 0x00);
	writeByte(0xFF22, 0x00);
	writeByte(0xFF23, 0xBF);
	writeByte(0xFF24, 0x77);
	writeByte(0xFF25, 0xF3);
	writeByte(0xFF26, 0xF1);
	writeByte(0xFF40, 0x91);
	writeByte(0xFF42, 0x00);
	writeByte(0xFF43, 0x00);
	writeByte(0xFF45, 0x00);
	writeByte(0xFF47, 0xFC);
	writeByte(0xFF48, 0xFF);
	writeByte(0xFF49, 0xFF);
	writeByte(0xFF4A, 0x00);
	writeByte(0xFF4B, 0x00);
	writeByte(0xFFFF, 0x00);
}

