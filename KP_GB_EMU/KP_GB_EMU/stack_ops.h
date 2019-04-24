#pragma once

#include "cpu.h"
#include "mmu.h"

namespace Stack
{
	void writeWord(MMU* const& mmu, CPU* const& cpu, const word_t& value);
	word_t readWord(MMU* const& mmu, CPU* const& cpu);
}
