#include <iostream>
#include <cstdint>
#include <cstring>

#include "cpu.h"



int main(int argc, char** argv) {
	std::cout << "Hello world" << std::endl;

	CPU cpu;
	std::cout << sizeof(CPU) << std::endl;

	std::memset(&cpu, 0, sizeof(CPU));

	//regs.B = 1;
	cpu.reg.C = 1;
	

	std::cout << regs.BC << std::endl;

	std::system("pause");
}


