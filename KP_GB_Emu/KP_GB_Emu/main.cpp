#include "emu.h"
#include "debug.h"

#include <iostream>

int main(int argc, char** argv)
{
	std::cout << sizeof(Registers) << std::endl;
	std::cout << sizeof(Inputs) << std::endl;

	//mmu.dumpInternalRam();

	GBEmulator state;

	/*for (int i = 0; i < 400; i++) {
		state.runStep();
		db_info("PC = %u\n", state.cpu.regs.PC);
	}

	db_statement(state.cpu.regs.dump())*/

	//Bios::gb()->dumpToFile("gb_bios.txt");

	state.start(sf::VideoMode(640, 480));

	//std::system("pause");

	return 0;
}
