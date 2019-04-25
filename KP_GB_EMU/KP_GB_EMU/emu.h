#pragma once

#include "mmu.h"
#include "cpu.h"
#include "inputs.h"

struct GBState
{
	MMU mmu;
	CPU cpu;
	Inputs inputs;

	void runStep();

	void reset();
};
