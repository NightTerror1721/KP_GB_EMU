#pragma once

#include <iostream>

#include "address.h"
#include "boot_rom.h"

class GPU;

class MMU
{
private:
	InvalidAddressSide _invalidAddress;
	RAM _ram;
	RAM _shadowRam;
	RAM _hram;

	GPU* _gpu;

	Bios* const _bios;
	bool _biosMode;

public:
	MMU(const bool& gbc_bios = false);

	void setGPU(GPU* const& gpu);
	
	void writeByte(const address_t& offset, const byte_t& value);
	void writeWord(const address_t& offset, const word_t& value);

	byte_t readByte(const address_t& offset) const;
	word_t readWord(const address_t& offset) const;

	void reset();

	void dumpInternalRam(const unsigned int& columns = 0x10);

private:
	AddressSide& findSide(const address_t& offset);

};
