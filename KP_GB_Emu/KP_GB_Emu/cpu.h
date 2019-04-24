#pragma once

#include "mmu.h"
#include "registers.h"
#include "interrupts.h"

#include <string>

using opfunc_w = void(*)(MMU* const&, CPU* const&, const word_t&);
using opfunc_b = void(*)(MMU* const&, CPU* const&, const byte_t&);
using opfunc_v = void(*)(MMU* const&, CPU* const&);

class CPU;


class Opcode
{
private:
	using opfunc = void*;

	const std::string _name;
	const uint8_t _length;
	const opfunc _func;

public:
	Opcode(const std::string& name, uint8_t length, opfunc_w func);
	Opcode(const std::string& name, uint8_t length, opfunc_b func);
	Opcode(const std::string& name, uint8_t length, opfunc_v func);
	Opcode();

	Opcode(const Opcode&) = delete;
	Opcode(Opcode&&) = delete;

	Opcode& operator= (const Opcode&) = delete;
	Opcode& operator= (Opcode&&) = delete;

	std::string name() const;
	uint8_t length() const;

	void operator() (MMU* const& mmu, CPU* const& cpu) const;
	void operator() (MMU* const& mmu, CPU* const& cpu, const byte_t& operand) const;
	void operator() (MMU* const& mmu, CPU* const& cpu, const word_t& operand) const;

private:
	void undefined();
};
extern const Opcode OPCODES[256];

class CPU
{
private:
	bool _stopped;

public:
	CPU();

	void step(MMU* const& mmu);
	void reset();

	void stop();
	bool isStopped();

public:
	Registers regs;
	Interrupts ints;
	uint64_t ticks;
};
