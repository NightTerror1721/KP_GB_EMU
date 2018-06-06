#pragma once

#include "basics.h"
#include "cpu.h"
#include "mmu.h"
#include "cartridge.h"

class CPU;
class Memory;

class Kernel
{

public:
	const CPU* cpu;
	const Memory* mem;

	const Cartridge* cartridge;

	Kernel();
	~Kernel();
};

