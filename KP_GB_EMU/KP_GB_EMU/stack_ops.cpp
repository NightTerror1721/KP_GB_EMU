#include "stack_ops.h"

void Stack::writeWord(MMU* const& mmu, CPU* const& cpu, const word_t& value)
{
	cpu->regs.SP -= 2;
	mmu->writeWord(cpu->regs.SP, value);
}

word_t Stack::readWord(MMU* const& mmu, CPU* const& cpu)
{
	word_t value = mmu->readWord(cpu->regs.SP);
	cpu->regs.SP += 2;
	return value;
}
