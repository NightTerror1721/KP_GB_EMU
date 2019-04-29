#include "boot_rom.h"

#include "cpu.h"

Bios::Bios(const bool& gbc) :
	AddressSide(),
	_rom(gbc ? GBC_BIOS : GB_BIOS),
	_gbc(gbc)
{}

void Bios::reset() {}

bool Bios::isGBC() { return _gbc; }

void Bios::dumpToFile(const char* filename)
{
	Opcode::dumpCodeToFile(filename, _rom, _gbc ? sizeof(GBC_BIOS) : sizeof(GB_BIOS));
}

ByteAddressAccessor Bios::operator[] (const size_t& offset)
{
	return {};
}

ConstByteAddressAccessor Bios::operator[] (const size_t& offset) const
{
	return { _rom + offset };
}

Bios Bios::_GB(false);
Bios Bios::_GBC(true);

Bios* const Bios::gb() { return &_GB; }
Bios* const Bios::gbc() { return &_GBC; }
