#include <iostream>
#include <cstdint>
#include <cstring>

#include <GL/glut.h>


#include "kernel.h"



int main(int argc, char** argv) {
	std::cout << "Hello world" << std::endl;

	/*CPU cpu;
	std::cout << sizeof(CPU) << std::endl;

	std::memset(&cpu, 0, sizeof(CPU));

	//regs.B = 1;
	cpu.reg.C = 1;

	//Memory m;

	Kernel k;
	const CPU* c = k.cpu;*/
	
	

	std::cout << sizeof(Memory) << std::endl;

	std::system("pause");
}


