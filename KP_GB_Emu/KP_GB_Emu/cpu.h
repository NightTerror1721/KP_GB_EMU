#pragma once

#include "baiscs.h"

typedef uint8 reg8;
typedef uint16 reg16;

#define bireg(reg0, reg1) union { \
	reg16 reg0##reg1; \
	struct { \
		reg8 reg0; \
		reg8 reg1; \
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

