#include "interrupts.h"

#include "cpu.h"
#include "mmu.h"
#include "stack_ops.h"

Interrupts::Interrupts() :
	master(false),
	enabled(false),
	flags(0)
{}

#define CHECK_INT(interrupt) if(enabled_##interrupt) { int_##interrupt = DISABLED_FLAG; interrupt(mmu, cpu); }

void Interrupts::step(MMU* const& mmu, CPU* const& cpu)
{
	if (master && enabled && flags)
	{
		CHECK_INT(vblank)
		CHECK_INT(lcdstat)
		CHECK_INT(timer)
		CHECK_INT(serial)
		CHECK_INT(joypad)
	}
}

void Interrupts::vblank(MMU* const& mmu, CPU* const& cpu)
{
	// TODO: drawFrameBuffer();

	master = false;
	Stack::writeWord(mmu, cpu, cpu->regs.PC);
	cpu->regs.PC = 0x40;
	cpu->ticks += 12;
}

void Interrupts::lcdstat(MMU* const& mmu, CPU* const& cpu)
{
	master = false;
	Stack::writeWord(mmu, cpu, cpu->regs.PC);
	cpu->regs.PC = 0x48;
	cpu->ticks += 12;
}

void Interrupts::timer(MMU* const& mmu, CPU* const& cpu)
{
	master = false;
	Stack::writeWord(mmu, cpu, cpu->regs.PC);
	cpu->regs.PC = 0x50;
	cpu->ticks += 12;
}

void Interrupts::serial(MMU* const& mmu, CPU* const& cpu)
{
	master = false;
	Stack::writeWord(mmu, cpu, cpu->regs.PC);
	cpu->regs.PC = 0x58;
	cpu->ticks += 12;
}

void Interrupts::joypad(MMU* const& mmu, CPU* const& cpu)
{
	master = false;
	Stack::writeWord(mmu, cpu, cpu->regs.PC);
	cpu->regs.PC = 0x60;
	cpu->ticks += 12;
}

void Interrupts::returnFromInterrupt(MMU* const& mmu, CPU* const& cpu)
{
	master = true;
	cpu->regs.PC = Stack::readWord(mmu, cpu);
}
