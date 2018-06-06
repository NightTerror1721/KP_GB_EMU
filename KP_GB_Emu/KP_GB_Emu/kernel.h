#pragma once

#include <list>
#include <functional>

#include "basics.h"
#include "cpu.h"
#include "mmu.h"
#include "cartridge.h"

class CPU;
class Memory;

class Kernel
{
private:
	std::list<std::string> _errs;

public:
	const CPU* cpu;
	const Memory* mem;

	const Cartridge* cartridge;

	Kernel();
	~Kernel();

	
	/* Error control */
	void push_error(const std::string error);
	void foreachError(void(*consumer)(std::string&)) const;

	__forceinline bool hasErrors() const { return _errs.empty(); }
	__forceinline void clearErrors() { _errs.clear(); }
	__forceinline void cpush_error(const std::string error) const { const_cast<Kernel*>(this)->push_error(error); }
	
};

