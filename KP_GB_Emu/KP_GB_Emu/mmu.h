#pragma once

#include <iostream>

#include "address.h"
#include "boot_rom.h"



class RAM : public AddressSide
{
private:
	byte_t * const _mem;
	const bool _shadow;

public:
	RAM(const address_t& offset, const address_t& length);
	RAM(const address_t& offset, const RAM& base); //shadow ram
	~RAM();

	void clear();

	void dump(const unsigned int& columns = 0x10);

	void reset();

protected:
	void _writeByte(const address_t& offset, const byte_t& value);
	void _writeWord(const address_t& offset, const word_t& value);

	byte_t _readByte(const address_t& offset) const;
	word_t _readWord(const address_t& offset) const;
};

class MMU
{
private:
	InvalidAddressSide _invalidAddress;
	RAM _ram;
	RAM _shadowRam;

	Bios* const _bios;
	bool _biosMode;

public:
	MMU(const bool& gbc_bios = false);
	
	void writeByte(const address_t& offset, const byte_t& value);
	void writeWord(const address_t& offset, const word_t& value);

	byte_t readByte(const address_t& offset) const;
	word_t readWord(const address_t& offset) const;

	void reset();

	void dumpInternalRam(const unsigned int& columns = 0x10);

private:
	AddressSide& findSide(const address_t& offset);

};
