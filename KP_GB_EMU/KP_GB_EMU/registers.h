#pragma once

#include <cinttypes>

using reg8_t  = uint8_t;
using reg16_t = uint16_t;

#define ENABLED_FLAG 1
#define DISABLED_FLAG 0

#define BYTE_OFFSET_REG_A 0
#define BYTE_OFFSET_REG_F 1
#define BYTE_OFFSET_REG_B 2
#define BYTE_OFFSET_REG_C 3
#define BYTE_OFFSET_REG_D 4
#define BYTE_OFFSET_REG_E 5
#define BYTE_OFFSET_REG_H 6
#define BYTE_OFFSET_REG_L 7

#define BYTE_OFFSET_REG_AF 0
#define BYTE_OFFSET_REG_BC 2
#define BYTE_OFFSET_REG_DE 4
#define BYTE_OFFSET_REG_HL 6
#define BYTE_OFFSET_REG_SP 8
#define BYTE_OFFSET_REG_PC 10

#define WORD_OFFSET_REG_AF 0
#define WORD_OFFSET_REG_BC 1
#define WORD_OFFSET_REG_DE 2
#define WORD_OFFSET_REG_HL 3
#define WORD_OFFSET_REG_SP 4
#define WORD_OFFSET_REG_PC 5

union Registers
{
	struct
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
	};
	reg8_t data8[12];
	reg16_t data16[6];


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
