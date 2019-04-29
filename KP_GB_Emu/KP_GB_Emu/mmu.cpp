#include "mmu.h"
#include "gpu.h"
#include "interrupts.h"


MMU::MMU(const bool& gbc_bios) :
	_invalidAddress(),
	_ram(0x2000),
	_shadowRam(_ram),
	_hram(0x7F),
	_gpu(nullptr),
	_int(),
	_bios(gbc_bios ? Bios::gbc() : Bios::gb()),
	_biosMode(true)
{}

void MMU::setGPU(GPU* const& gpu) { _gpu = gpu; }

void MMU::setInterrupts(Interrupts* const& ints) { _int = { ints }; }

void MMU::dumpInternalRam(const unsigned int& columns) { _ram.dump(columns); }

MMU::Accessor MMU::findSide(const address_t& offset)
{
	switch (offset & 0xF000)
	{
		/* ROM bank #0 */
		case 0x0000:
			if (_biosMode)
			{
				if (offset < 0x100)
					return { *_bios, offset };
				else if (offset < 0x800 && _bios->isGBC())
					return { *_bios, offset };
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
			return { _gpu->vram, offset - 0x8000 };

		/* switchable RAM bank */
		case 0xA000:
		case 0xB000:
			break;

		/* Internal RAM */
		case 0xC000:
		case 0xD000:
			return { _ram, offset - 0xC000 };

		/* Echo of Internal RAM (0xE000 to 0xFDFF) */
		case 0xE000:
		case 0xF000:
			if (offset < 0xFE00)
				return { _shadowRam, offset - 0xE000 };
			else if (offset < 0xFEA0)
				return { _gpu->oam, offset - 0xFE00 };
			else if (offset < 0xFF80);

			else return { _int, 1 };
			break;
	}

	return { _invalidAddress, 0 };
}

#define WRITE_SIDE(offset) this->findSide((offset))
#define READ_SIDE(offset) const_cast<MMU*>(this)->findSide((offset))

void MMU::writeByte(const address_t& offset, const byte_t& value)
{
	MMU::Accessor accessor = WRITE_SIDE(offset);
	accessor.data = value;
	accessor.location.onMmuWrite(offset, value);
}

void MMU::writeWord(const address_t& offset, const word_t& value)
{
	MMU::Accessor accessor = WRITE_SIDE(offset);
	accessor.data.as_word() = value;
	accessor.location.onMmuWrite(offset, value);
}

byte_t MMU::readByte(const address_t& offset) const
{
	return READ_SIDE(offset).data;
}

word_t MMU::readWord(const address_t& offset) const
{
	return READ_SIDE(offset).data.as_word();
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



MMU::Accessor::Accessor(AddressSide& location, const address_t& address) :
	location(location),
	data(location[address])
{}

MMU::Accessor::Accessor(AddressSide& location, const int& address) :
	location(location),
	data(location[static_cast<address_t>(address & 0xFFFF)])
{}
