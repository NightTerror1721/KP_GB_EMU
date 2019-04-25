#include "emu.h"
#include "debug.h"

#include <iostream>

int main(int argc, char** argv)
{

	Registers regs;
	MMU mmu;

	mmu.writeByte(0xc000, 17);
	std::cout << static_cast<int>(mmu.readByte(0xc000)) << std::endl;

	std::cout << sizeof(Registers) << std::endl;
	std::cout << sizeof(Inputs) << std::endl;

	//mmu.dumpInternalRam();

	GBState state;

	/*for (int i = 0; i < 400; i++) {
		state.runStep();
		db_info("PC = %u\n", state.cpu.regs.PC);
	}

	db_statement(state.cpu.regs.dump())*/

	Bios::gb()->dumpToFile("gb_bios.txt");

	std::system("pause");

	return 0;
}
