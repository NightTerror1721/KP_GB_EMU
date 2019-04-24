
#include "registers.h"
#include "mmu.h"
#include "cpu.h"
#include "inputs.h"

#include <iostream>

int main(int argc, char** argv)
{

	Registers regs;
	MMU mmu;

	mmu.writeByte(0xc000, 17);
	std::cout << static_cast<int>(mmu.readByte(0xc000)) << std::endl;

	std::cout << sizeof(Inputs) << std::endl;

	//mmu.dumpInternalRam();

	std::system("pause");

	return 0;
}
