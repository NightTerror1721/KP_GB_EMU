#pragma once

#include <iostream>

#include "address.h"
#include "boot_rom.h"
#include "interrupts.h"

class GPU;
struct Interrupts;

class MMU
{
private:
	InvalidAddressSide _invalidAddress;
	RAM _ram;
	RAM _shadowRam;
	RAM _hram;

	GPU* _gpu;

	InterruptsAddressSide _int;

	Bios* const _bios;
	bool _biosMode;

public:
	MMU(const bool& gbc_bios = false);

	void setGPU(GPU* const& gpu);
	void setInterrupts(Interrupts* const& ints);
	
	void writeByte(const address_t& offset, const byte_t& value);
	void writeWord(const address_t& offset, const word_t& value);

	byte_t readByte(const address_t& offset) const;
	word_t readWord(const address_t& offset) const;

	void reset();

	void dumpInternalRam(const unsigned int& columns = 0x10);

private:
	struct Accessor
	{
		ByteAddressAccessor data;
		AddressSide& location;

		Accessor(AddressSide& location, const address_t& address);
		Accessor(AddressSide& location, const int& address);
	};

	Accessor findSide(const address_t& offset);
};
