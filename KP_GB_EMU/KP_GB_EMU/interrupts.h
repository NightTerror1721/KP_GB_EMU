#pragma once

#include <cinttypes>

class CPU;
class MMU;

#ifndef ENABLED_FLAG
#define ENABLED_FLAG 1
#endif

#ifndef DISABLED_FLAG
#define DISABLED_FLAG 0
#endif

struct Interrupts
{
	bool master;
	union
	{
		struct
		{
			uint8_t enabled_vblank : 1;
			uint8_t enabled_lcdstat : 1;
			uint8_t enabled_timer : 1;
			uint8_t enabled_serial : 1;
			uint8_t enabled_joypad : 1;
			uint8_t : 3;
		};
		uint8_t enabled;
	};
	union
	{
		struct
		{
			uint8_t int_vblank : 1;
			uint8_t int_lcdstat : 1;
			uint8_t int_timer : 1;
			uint8_t int_serial : 1;
			uint8_t int_joypad : 1;
			uint8_t : 3;
		};
		uint8_t flags;
	};

public:
	Interrupts();

	void step(MMU* const& mmu, CPU* const& cpu);

	void vblank(MMU* const& mmu, CPU* const& cpu);
	void lcdstat(MMU* const& mmu, CPU* const& cpu);
	void timer(MMU* const& mmu, CPU* const& cpu);
	void serial(MMU* const& mmu, CPU* const& cpu);
	void joypad(MMU* const& mmu, CPU* const& cpu);

	void returnFromInterrupt(MMU* const& mmu, CPU* const& cpu);
};