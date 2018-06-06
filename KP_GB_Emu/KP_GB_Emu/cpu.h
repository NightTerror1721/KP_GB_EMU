#pragma once

#include "basics.h"

typedef uint8 reg8;
typedef uint16 reg16;

#define bireg(hi_reg, lo_reg) union { \
	reg16 hi_reg##lo_reg; \
	struct { \
		reg8 lo_reg; \
		reg8 hi_reg; \
	}; \
}

typedef struct {
	bireg(B, C);
	bireg(D, E);
	bireg(H, L);
	bireg(A, F);
	reg16 PC;
	reg16 SP;
	reg16 M;

	FLAG IME;
	FLAG I;
	FLAG R;
} Registers;

#undef bireg


class CPU
{
public:
	Registers reg;
	struct {
		reg16 M;
	} clock;

	CPU();
	~CPU();
};

