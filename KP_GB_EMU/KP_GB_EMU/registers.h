#pragma once

#include <cinttypes>

using reg8_t  = uint8_t;
using reg16_t = uint16_t;

#define ENABLED_FLAG 1
#define DISABLED_FLAG 0

struct Registers
{
	union
	{
		struct
		{
			union
			{
				struct
				{
					reg8_t : 4;
					reg8_t CarryFlag : 1;
					reg8_t HalfCarryFlag : 1;
					reg8_t SubtractFlag : 1;
					reg8_t ZeroFlag : 1;
				};
				reg8_t F;
			};
			
			reg8_t A;
		};
		reg16_t AF;
	};
	union
	{
		struct { reg8_t C; reg8_t B; };
		reg16_t BC;
	};
	union
	{
		struct { reg8_t E; reg8_t D; };
		reg16_t DE;
	};
	union
	{
		struct { reg8_t L; reg8_t H; };
		reg16_t HL;
	};

	reg16_t SP;

	reg16_t PC;


public:
	Registers();

	void dump();

	__forceinline void setZeroFlag() { ZeroFlag = ENABLED_FLAG; }
	__forceinline void setSubtractFlag() { SubtractFlag = ENABLED_FLAG; }
	__forceinline void setHalfCarryFlag() { HalfCarryFlag = ENABLED_FLAG; }
	__forceinline void setCarryFlag() { CarryFlag = ENABLED_FLAG; }

	__forceinline void clearZeroFlag() { ZeroFlag = DISABLED_FLAG; }
	__forceinline void clearSubtractFlag() { SubtractFlag = DISABLED_FLAG; }
	__forceinline void clearHalfCarryFlag() { HalfCarryFlag = DISABLED_FLAG; }
	__forceinline void clearCarryFlag() { CarryFlag = DISABLED_FLAG; }
};
