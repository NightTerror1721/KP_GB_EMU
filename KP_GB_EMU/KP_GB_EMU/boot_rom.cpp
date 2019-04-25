#include "boot_rom.h"

#include "cpu.h"

Bios::Bios(const bool& gbc) :
	AddressSide(0, gbc ? sizeof(GBC_BIOS) : sizeof(GB_BIOS)),
	_rom(gbc ? GBC_BIOS : GB_BIOS),
	_gbc(gbc)
{}

void Bios::reset() {}

bool Bios::isGBC() { return _gbc; }

void Bios::dumpToFile(const char* filename)
{
	Opcode::dumpCodeToFile(filename, _rom, _length);
}


void Bios::_writeByte(const address_t& offset, const byte_t& value) {}
void Bios::_writeWord(const address_t& offset, const word_t& value) {};

byte_t Bios::_readByte(const address_t& offset) const
{
	return _rom[offset];
}

word_t Bios::_readWord(const address_t& offset) const
{
	return *reinterpret_cast<const word_t*>(_rom + offset);
}

Bios Bios::_GB(false);
Bios Bios::_GBC(true);

Bios* const Bios::gb() { return &_GB; }
Bios* const Bios::gbc() { return &_GBC; }
