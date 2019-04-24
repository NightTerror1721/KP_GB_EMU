#include "registers.h"

#include <iostream>

#define UINT_REG(x) static_cast<unsigned int>(x)

Registers::Registers() :
	AF(0),
	BC(0),
	DE(0),
	HL(0),
	SP(0),
	PC(0)
{}

void Registers::dump()
{
	std::cout << std::hex;
	std::cout << "AF[" << UINT_REG(AF) << "] A[" << UINT_REG(A) << "] F[" << UINT_REG(F) << "]" << std::endl;
	std::cout << "BC[" << UINT_REG(BC) << "] B[" << UINT_REG(B) << "] C[" << UINT_REG(C) << "]" << std::endl;
	std::cout << "DE[" << UINT_REG(DE) << "] D[" << UINT_REG(D) << "] E[" << UINT_REG(E) << "]" << std::endl;
	std::cout << "HL[" << UINT_REG(HL) << "] H[" << UINT_REG(H) << "] L[" << UINT_REG(L) << "]" << std::endl;
	std::cout << "SP[" << UINT_REG(SP) << "]" << std::endl;
	std::cout << "PC[" << UINT_REG(PC) << "]" << std::endl;
	std::cout << std::dec;
}
