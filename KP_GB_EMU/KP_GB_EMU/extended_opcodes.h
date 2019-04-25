#pragma once

#include <cinttypes>
#include <string>

class MMU;
class CPU;

namespace OpCB
{
	void cb_op(MMU* const& mmu, CPU* const& cpu, const uint8_t& opcode);
}
