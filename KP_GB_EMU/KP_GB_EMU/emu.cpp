#include "emu.h"

void GBState::runStep()
{
	cpu.step(&mmu);
}

void GBState::reset()
{
	mmu.reset();
	cpu.reset();
	inputs.reset();
}
