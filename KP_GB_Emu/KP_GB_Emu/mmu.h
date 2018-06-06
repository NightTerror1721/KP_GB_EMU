#pragma once
#include "kernel.h"

class Kernel;

#define MEM_BASE_SIZE 0x10000
#define ROM_PAGE_LEN 0x4000
#define VRAM_PAGE_LEN 0x2000
#define ERAM_PAGE_LEN 0x2000
#define WRAM_PAGE_LEN 0x2000

typedef WORD address;


class Memory
{
private:
	const Kernel* core;

public:
	BYTE registers[0x0100];
	BYTE oam[0x00A0];

	BYTE* vram;
	BYTE* wram;

protected:
	uint32 vramPageStart;
	uint32 wramPageStart;
	uint32 romPageStart;

public:
	Memory(const Kernel* core);
	~Memory();

	void setValue(address addr, BYTE value);
	BYTE getValue(address addr);

	void setIO(address addr, BYTE value);
	BYTE getIO(address addr);
};

