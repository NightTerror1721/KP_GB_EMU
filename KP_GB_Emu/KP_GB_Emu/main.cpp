#include <iostream>
#include <cstdint>
#include <cstring>

typedef unsigned char reg8;
typedef unsigned short reg16;

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
} Registers;


int main(int argc, char** argv) {
	std::cout << "Hello world" << std::endl;

	Registers regs;
	std::cout << sizeof(Registers) << std::endl;

	std::memset(&regs, 0, sizeof(Registers));

	//regs.B = 1;
	regs.C = 1;
	

	std::cout << regs.BC << std::endl;

	std::system("pause");
}


