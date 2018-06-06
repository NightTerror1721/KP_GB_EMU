#include "mmu.h"

typedef DWORD ext_address;


Memory::Memory(const Kernel* core) : core(core)
{
	this->wram = new BYTE[WRAM_PAGE_LEN * (core->cartridge->isColorGB() ? 8 : 2)];
	this->vram = new BYTE[VRAM_PAGE_LEN * (core->cartridge->isColorGB() ? 2 : 1)];
}


Memory::~Memory()
{
	delete[] wram;
	delete[] vram;
}


void Memory::setValue(address h_addr, BYTE value)
{
	ext_address addr = h_addr;
	switch(addr & 0xF000)
	{
		case 0x0000:
		case 0x1000:
		case 0x2000:
		case 0x3000:
		case 0x4000:
		case 0x5000:
		case 0x6000:
		case 0x7000:
			// handled by external hardware, if any
			break;
		case 0x8000:
		case 0x9000:
			vram[vramPageStart + addr - 0x8000] = value;
			break;
		case 0xA000:
		case 0xB000:
			// handled by external hardware, if any
			break;
		case 0xC000:
			wram[addr - 0xC000] = value;
			break;
		case 0xD000:
			wram[wramPageStart + addr - 0xD000] = value;
			break;
		case 0xE000:
		case 0xF000:
			// FEA0-FEFF is not usable
			if(0xFEA0 <= addr && addr <= 0xFEFF)
				break;
			if (addr < 0xFE00) // 7.5kb echo
				setValue(h_addr - 0xE000, value);
			else if (addr < 0xFF00)
				oam[addr - 0xFE00] = value;
			else setIO(h_addr - 0xFF00, value);
			break;
	}
}

BYTE Memory::getValue(address h_addr)
{
	ext_address addr = h_addr;
	switch(addr & 0xF000)
	{
		case 0x0000:
		case 0x1000:
		case 0x2000:
		case 0x3000:
			return core->cartridge->romdata(addr);
		case 0x4000:
		case 0x5000:
		case 0x6000:
		case 0x7000:
			return core->cartridge->romdata(romPageStart + addr - 0x4000);
		case 0x8000:
		case 0x9000:
			return vram[vramPageStart + addr - 0x8000];
		case 0xA000:
		case 0xB000:
			return 0;
		case 0xC000:
			return wram[addr - 0xC000];
		case 0xD000:
			return wram[wramPageStart + addr - 0xD000];
		case 0xE000:
		case 0xF000:
			// FEA0-FEFF is not usable
			if (0xFEA0 <= addr && addr <= 0xFEFF)
				return 0xFF;
			else if (addr < 0xFE00)
				// E000-FE00 echoes the main ram
				// But wait, E000-FE00 contains just 7.5kb and hence
				// does not echo the entire 8kb internal ram
				return getValue(addr - 0xE000);
			else if (addr < 0xFF00)
				return oam[addr - 0xFF00];
			else return getIO(addr - 0xFF00);
	}
	return 0xFF;
}
