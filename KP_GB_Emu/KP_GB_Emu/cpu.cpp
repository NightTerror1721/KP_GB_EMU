#include "cpu.h"
#include "stack_ops.h"
#include "extended_opcodes.h"
#include "debug.h"

#include <fstream>

void invalid_opcode(MMU* const& mmu, CPU* const& cpu) {}


Opcode::Opcode(const std::string& name, uint8_t length, opfunc func) :
	_name(name),
	_length(length),
	_func(func)
{}

Opcode::Opcode() :
	_name("<unknown_opcode>"),
	_length(0),
	_func(static_cast<opfunc>(invalid_opcode))
{}


std::string Opcode::name() const { return _name; }
uint8_t Opcode::length() const { return _length; }

void Opcode::operator() (MMU* const& mmu, CPU* const& cpu) const
{
	if (!_length)
		static_cast<opfunc_v>(_func)(mmu, cpu);
}

void Opcode::operator() (MMU* const& mmu, CPU* const& cpu, const byte_t& operand) const
{
	if (_length == 1)
		static_cast<opfunc_b>(_func)(mmu, cpu, operand);
}

void Opcode::operator() (MMU* const& mmu, CPU* const& cpu, const word_t& operand) const
{
	if (_length == 2)
		static_cast<opfunc_w>(_func)(mmu, cpu, operand);
}

byte_t Opcode::ticks(const byte_t& opcode_id) { return OPCODE_TICKS[opcode_id]; }
const Opcode* Opcode::opcode(const byte_t& opcode_id) { return &OPCODES[opcode_id]; }
void Opcode::execute(MMU* const& mmu, CPU* const& cpu, const byte_t& opcode_id)
{
	const Opcode& op = OPCODES[opcode_id];

	switch (op._length)
	{
		case 0: {
			op(mmu, cpu);
			db_info("OPCODE %s", op.name().c_str())
		} break;
		case 1: {
			byte_t operand = mmu->readByte(cpu->regs.PC++);
			op(mmu, cpu, operand);
			db_info("OPCODE %s, %u", op.name().c_str(), operand)
		} break;
		case 2: {
			word_t operand = mmu->readWord(cpu->regs.PC);
			cpu->regs.PC += 2;
			op(mmu, cpu, operand);
			db_info("OPCODE %s, %u", op.name().c_str(), operand)
		} break;
	}
	cpu->ticks += OPCODE_TICKS[opcode_id];
}

void Opcode::dumpCodeToFile(const char* filename, const byte_t* code, size_t size)
{
	std::fstream file(filename, std::fstream::out);

	const size_t csize = size;
	while (size > 0)
	{
		size--;
		const unsigned int opid = *(code++);
		const Opcode& op = OPCODES[opid];
		

		switch (op.length())
		{
			case 0: file << "[" << (csize - size - 1) << "](0x" << std::hex << opid << std::dec << ") " << op.name() << std::endl; break;
			case 1:
				if (size > 0)
				{
					size--;
					int operand = static_cast<int>(*(code++));
					file << "[" << (csize - size - 2) << "](0x" << std::hex << opid << std::dec << ") " << op.name() << "\t" << operand << std::endl;
				}
				break;
			case 2:
				if (size > 1)
				{
					size -= 2;
					int operand = static_cast<int>(*reinterpret_cast<const word_t*>(code));
					code += 2;
					file << "[" << (csize - size - 3) << "](0x" << std::hex << opid << std::dec << ") " << op.name() << "\t" << operand << std::endl;
				}
				break;
		}
	}

	file.close();
}


CPU::CPU() :
	regs(),
	ints(),
	ticks(0),
	_stopped(false)
{}

void CPU::step(MMU* const& mmu)
{
	if (_stopped)
		return;

	byte_t instruction = mmu->readByte(regs.PC++);
	Opcode::execute(mmu, this, instruction);
}

void CPU::reset()
{
	regs.A = 0x01;
	regs.F = 0xb0;
	regs.B = 0x00;
	regs.C = 0x13;
	regs.D = 0x00;
	regs.E = 0xd8;
	regs.H = 0x01;
	regs.L = 0x4d;
	regs.SP = 0xfffe;
	regs.PC = 0x0100;

	ints.master = true;
	ints.enabled = 0;
	ints.flags = 0;

	ticks = 0;
	_stopped = false;
}

void CPU::stop() { _stopped = true; }
bool CPU::isStopped() { return _stopped; }









/* opcode functions */
#define BYTE byte_t
#define WORD word_t
#define __CPU cpu
#define __MMU mmu
#define __REG __CPU->regs
#define __INT __CPU->ints
#define __ARGS __MMU, __CPU
#define BASE_ARGS MMU* const& __MMU, CPU* const& __CPU
#define BYTE_ARG const BYTE&
#define WORD_ARG const WORD&

#define A __REG.A
#define B __REG.B
#define C __REG.C
#define D __REG.D
#define E __REG.E
#define F __REG.F
#define H __REG.H
#define L __REG.L
#define AF __REG.AF
#define BC __REG.BC
#define DE __REG.DE
#define HL __REG.HL
#define SP __REG.SP
#define PC __REG.PC

#define WriteByte(offset, value) __MMU->writeByte((offset), (value))
#define WriteWord(offset, value) __MMU->writeWord((offset), (value))
#define ReadByte(offset) __MMU->readByte((offset))
#define ReadWord(offset) __MMU->readWord((offset))

#define ZERO_FLAG __REG.ZeroFlag
#define SUBTRACT_FLAG __REG.SubtractFlag
#define HALFCARRY_FLAG __REG.HalfCarryFlag
#define CARRY_FLAG __REG.CarryFlag

#define SET_FLAG(flag) (flag) = ENABLED_FLAG
#define CLEAR_FLAG(flag) (flag) = DISABLED_FLAG

#define SET_IF_ELSE_CLEAR(flag, if_statement) if(if_statement) SET_FLAG(flag); else CLEAR_FLAG(flag)
#define CLEAR_IF_ELSE_SET(flag, if_statement) if(if_statement) CLEAR_FLAG(flag); else SET_FLAG(flag)


BYTE __inc(BASE_ARGS, BYTE value)
{
	SET_IF_ELSE_CLEAR(HALFCARRY_FLAG, (value & 0x0F) == 0x0F);

	value++;
	CLEAR_IF_ELSE_SET(ZERO_FLAG, (value + 1));
	CLEAR_FLAG(SUBTRACT_FLAG);
	return value;
}
#define INC(value) __inc(__ARGS, (value))

BYTE __dec(BASE_ARGS, BYTE value)
{
	CLEAR_IF_ELSE_SET(HALFCARRY_FLAG, (value & 0x0F) == 0x0F);

	value--;
	CLEAR_IF_ELSE_SET(ZERO_FLAG, (value + 1));
	CLEAR_FLAG(SUBTRACT_FLAG);
	return value;
}
#define DEC(value) __dec(__ARGS, (value))

void __add(BASE_ARGS, BYTE& dest, BYTE_ARG value)
{
	unsigned int result = dest + value;

	SET_IF_ELSE_CLEAR(CARRY_FLAG, result & 0xFF00);

	dest = static_cast<BYTE>(result & 0xFF);
	CLEAR_IF_ELSE_SET(ZERO_FLAG, dest);
	SET_IF_ELSE_CLEAR(HALFCARRY_FLAG, ((dest & 0x0F) + (value & 0x0F)) > 0x0F);

	CLEAR_FLAG(SUBTRACT_FLAG);
}
#define ADD(dest, value) __add(__ARGS, (dest), (value))

void __addw(BASE_ARGS, WORD& dest, WORD_ARG value)
{
	unsigned int result = dest + value;

	SET_IF_ELSE_CLEAR(CARRY_FLAG, result & 0xFFFF0000);

	dest = static_cast<BYTE>(result & 0xFFFF);
	//CLEAR_IF_ELSE_SET(ZERO_FLAG, dest); zero flag left alone
	SET_IF_ELSE_CLEAR(HALFCARRY_FLAG, ((dest & 0x0F) + (value & 0x0F)) > 0x0F);

	CLEAR_FLAG(SUBTRACT_FLAG);
}
#define ADDW(dest, value) __addw(__ARGS, (dest), (value))

void __adc(BASE_ARGS, BYTE value)
{
	value += CARRY_FLAG ? 1 : 0;

	int result = A + value;
	SET_IF_ELSE_CLEAR(CARRY_FLAG, result & 0xFF00);
	SET_IF_ELSE_CLEAR(ZERO_FLAG, value == A);
	SET_IF_ELSE_CLEAR(HALFCARRY_FLAG, ((value & 0x0F) + (A & 0x0F)) > 0x0F);

	SET_FLAG(SUBTRACT_FLAG);
	A = static_cast<BYTE>(result & 0xFF);
}
#define ADC(value) __adc(__ARGS, (value))

void __sbc(BASE_ARGS, BYTE value)
{
	value += CARRY_FLAG ? 1 : 0;

	SET_FLAG(SUBTRACT_FLAG);

	SET_IF_ELSE_CLEAR(CARRY_FLAG, value > A);
	SET_IF_ELSE_CLEAR(ZERO_FLAG, value == A);
	SET_IF_ELSE_CLEAR(HALFCARRY_FLAG, (value & 0x0F) > (A & 0x0F));

	A -= value;
}
#define SBC(value) __sbc(__ARGS, (value))

void __sub(BASE_ARGS, BYTE_ARG value)
{
	SET_FLAG(SUBTRACT_FLAG);

	SET_IF_ELSE_CLEAR(CARRY_FLAG, value > A);
	SET_IF_ELSE_CLEAR(HALFCARRY_FLAG, (value & 0x0F) > (A & 0x0F));

	A -= value;
	CLEAR_IF_ELSE_SET(ZERO_FLAG, A);
}
#define SUB(value) __sub(__ARGS, (value))

void __and(BASE_ARGS, BYTE_ARG value)
{
	A &= value;

	CLEAR_IF_ELSE_SET(ZERO_FLAG, A);

	CLEAR_FLAG(CARRY_FLAG);
	CLEAR_FLAG(SUBTRACT_FLAG);
	SET_FLAG(HALFCARRY_FLAG);
}
#define AND(value) __and(__ARGS, (value))

void __or(BASE_ARGS, BYTE_ARG value)
{
	A |= value;

	CLEAR_IF_ELSE_SET(ZERO_FLAG, A);

	CLEAR_FLAG(CARRY_FLAG);
	CLEAR_FLAG(SUBTRACT_FLAG);
	CLEAR_FLAG(HALFCARRY_FLAG);
}
#define OR(value) __or(__ARGS, (value))

void __xor(BASE_ARGS, BYTE_ARG value)
{
	A ^= value;

	CLEAR_IF_ELSE_SET(ZERO_FLAG, A);

	CLEAR_FLAG(CARRY_FLAG);
	CLEAR_FLAG(SUBTRACT_FLAG);
	CLEAR_FLAG(HALFCARRY_FLAG);
}
#define XOR(value) __xor(__ARGS, (value))

void __cp(BASE_ARGS, BYTE_ARG value)
{
	SET_IF_ELSE_CLEAR(ZERO_FLAG, A == value);
	SET_IF_ELSE_CLEAR(CARRY_FLAG, value > A);
	SET_IF_ELSE_CLEAR(HALFCARRY_FLAG, (value & 0xF) > (A & 0xF));

	SET_FLAG(SUBTRACT_FLAG);
}
#define CP(value) __cp(__ARGS, (value))







#define OPERAND operand
#define opfuncv(name) void name(BASE_ARGS)
#define opfuncb(name) void name(BASE_ARGS, BYTE_ARG OPERAND)
#define opfuncw(name) void name(BASE_ARGS, WORD_ARG OPERAND)

#define SIGNED_BYTE(value) static_cast<int8_t>(value)
#define SIGNED_WORD(value) static_cast<int16_t>(value)

#define STACK_READ_WORD() Stack::readWord(__ARGS)
#define STACK_WRITE_WORD(value) Stack::writeWord(__ARGS, (value))

#define TICKS __CPU->ticks

opfuncv(nop) {}
opfuncw(ld_bc_nn) { BC = OPERAND; }
opfuncv(ld_bcp_a) { WriteByte(BC, A); }
opfuncv(inc_bc) { BC++; }
opfuncv(inc_b) { B = INC(B); }
opfuncv(dec_b) { B = DEC(B); }
opfuncb(ld_b_n) { B = OPERAND; }
opfuncv(rlca) {
	BYTE carry = (A & 0x80) >> 7;
	SET_IF_ELSE_CLEAR(CARRY_FLAG, carry);

	A <<= 1;
	A += carry;

	CLEAR_FLAG(SUBTRACT_FLAG);
	CLEAR_FLAG(ZERO_FLAG);
	CLEAR_FLAG(HALFCARRY_FLAG);
}
opfuncw(ld_nnp_sp) { WriteWord(OPERAND, SP); }
opfuncv(add_hl_bc) { ADDW(HL, BC); }
opfuncv(ld_a_bcp) { A = ReadByte(BC); }
opfuncv(dec_bc) { BC--; }
opfuncv(inc_c) { C = INC(C); }
opfuncv(dec_c) { C = DEC(C); }
opfuncb(ld_c_n) { C = OPERAND; }
opfuncv(rrca) {
	BYTE carry = A & 0x01;
	SET_IF_ELSE_CLEAR(CARRY_FLAG, carry);

	A >>= 1;
	if (carry)
		A |= 0x80;

	CLEAR_FLAG(SUBTRACT_FLAG);
	CLEAR_FLAG(ZERO_FLAG);
	CLEAR_FLAG(HALFCARRY_FLAG);
}
opfuncb(stop) { __CPU->stop(); }
opfuncw(ld_de_nn) { DE = OPERAND; }
opfuncv(ld_dep_a) { WriteByte(DE, A); }
opfuncv(inc_de) { DE++; }
opfuncv(inc_d) { D = INC(D); }
opfuncv(dec_d) { D = DEC(D); }
opfuncb(ld_d_n) { D = OPERAND; }
opfuncv(rla) {
	int carry = CARRY_FLAG ? 1 : 0;
	SET_IF_ELSE_CLEAR(CARRY_FLAG, A & 0x80);

	A <<= 1;
	A += carry;

	CLEAR_FLAG(SUBTRACT_FLAG);
	CLEAR_FLAG(ZERO_FLAG);
	CLEAR_FLAG(HALFCARRY_FLAG);
}
opfuncb(jr_n) { PC += SIGNED_BYTE(OPERAND); }
opfuncv(add_hl_de) { ADDW(HL, DE); }
opfuncv(ld_a_dep) { A = ReadByte(DE); }
opfuncv(dec_de) { DE--; }
opfuncv(inc_e) { E = INC(E); }
opfuncv(dec_e) { E = DEC(E); }
opfuncb(ld_e_n) { E = OPERAND; }
opfuncv(rra) {
	int carry = (CARRY_FLAG ? 1 : 0) << 7;
	SET_IF_ELSE_CLEAR(CARRY_FLAG, A & 0x01);

	A >>= 1;
	A += carry;

	CLEAR_FLAG(SUBTRACT_FLAG);
	CLEAR_FLAG(ZERO_FLAG);
	CLEAR_FLAG(HALFCARRY_FLAG);
}
opfuncb(jr_nz_n) {
	if (ZERO_FLAG)
		TICKS += 8;
	else
	{
		PC += SIGNED_BYTE(OPERAND);
		TICKS += 12;
	}
}
opfuncw(ld_hl_nn) { HL = OPERAND; }
opfuncv(ldi_hlp_a) { WriteByte(HL++, A); }
opfuncv(inc_hl) { HL++; }
opfuncv(inc_h) { H = INC(H); }
opfuncv(dec_h) { H = DEC(H); }
opfuncb(ld_h_n) { H = OPERAND; }
opfuncv(daa) {
	WORD s = A;
	if (SUBTRACT_FLAG)
	{
		if (HALFCARRY_FLAG)
			s = (s - 0x06) & 0xFF;
		if (CARRY_FLAG)
			s -= 0x60;
	}
	else
	{
		if (HALFCARRY_FLAG || (s & 0x0F) > 9)
			s += 0x06;
		if (CARRY_FLAG || s > 0x9F)
			s += 0x60;
	}

	A = static_cast<BYTE>(s);
	CLEAR_FLAG(HALFCARRY_FLAG);
	CLEAR_IF_ELSE_SET(ZERO_FLAG, A);
	if (s >= 0x100)
		SET_FLAG(CARRY_FLAG);
}
opfuncb(jr_z_n) {
	if (ZERO_FLAG)
	{
		PC += SIGNED_BYTE(OPERAND);
		TICKS += 12;
	}
	else TICKS += 8;
}
opfuncv(add_hl_hl) { ADDW(HL, HL); }
opfuncv(ldi_a_hlp) { A = ReadByte(HL++); }
opfuncv(dec_hl) { HL--; }
opfuncv(inc_l) { L = INC(L); }
opfuncv(dec_l) { L = DEC(L); }
opfuncb(ld_l_n) { L = OPERAND; }
opfuncv(cpl) {
	A = ~A;
	SET_FLAG(SUBTRACT_FLAG);
	SET_FLAG(HALFCARRY_FLAG);
}
opfuncb(jr_nc_n) {
	if (CARRY_FLAG)
		TICKS += 8;
	else
	{
		PC += SIGNED_BYTE(OPERAND);
		TICKS += 12;
	}

}
opfuncw(ld_sp_nn) { SP = OPERAND; }
opfuncv(ldd_hlp_a) { WriteByte(HL--, A); }
opfuncv(inc_sp) { SP--; }
opfuncv(inc_hlp) { WriteByte(HL, INC(ReadByte(HL))); }
opfuncv(dec_hlp) { WriteByte(HL, DEC(ReadByte(HL))); }
opfuncb(ld_hlp_n) { WriteByte(HL, OPERAND); }
opfuncv(scf) {
	SET_FLAG(CARRY_FLAG);
	CLEAR_FLAG(SUBTRACT_FLAG);
	CLEAR_FLAG(HALFCARRY_FLAG);
}
opfuncb(jr_c_n) {
	if (CARRY_FLAG)
	{
		PC += SIGNED_BYTE(OPERAND);
		TICKS += 12;
	}
	else TICKS += 8;
}
opfuncv(add_hl_sp) { ADDW(HL, SP); }
opfuncv(ldd_a_hlp) { A = ReadByte(HL--); }
opfuncv(dec_sp) { SP--; }
opfuncv(inc_a) { A = INC(A); }
opfuncv(dec_a) { A = DEC(A); }
opfuncb(ld_a_n) { A = OPERAND; }
opfuncv(ccf) {
	CLEAR_IF_ELSE_SET(CARRY_FLAG, CARRY_FLAG);
	SET_FLAG(SUBTRACT_FLAG);
	SET_FLAG(HALFCARRY_FLAG);
}

opfuncv(ld_b_c) { B = C; }
opfuncv(ld_b_d) { B = D; }
opfuncv(ld_b_e) { B = E; }
opfuncv(ld_b_h) { B = H; }
opfuncv(ld_b_l) { B = L; }
opfuncv(ld_b_hlp) { B = ReadByte(HL); }
opfuncv(ld_b_a) { B = A; }

opfuncv(ld_c_b) { C = B; }
opfuncv(ld_c_d) { C = D; }
opfuncv(ld_c_e) { C = E; }
opfuncv(ld_c_h) { C = H; }
opfuncv(ld_c_l) { C = L; }
opfuncv(ld_c_hlp) { C = ReadByte(HL); }
opfuncv(ld_c_a) { C = A; }

opfuncv(ld_d_b) { D = B; }
opfuncv(ld_d_c) { D = C; }
opfuncv(ld_d_e) { D = E; }
opfuncv(ld_d_h) { D = H; }
opfuncv(ld_d_l) { D = L; }
opfuncv(ld_d_hlp) { D = ReadByte(HL); }
opfuncv(ld_d_a) { D = A; }

opfuncv(ld_e_b) { E = B; }
opfuncv(ld_e_c) { E = C; }
opfuncv(ld_e_d) { E = D; }
opfuncv(ld_e_h) { E = H; }
opfuncv(ld_e_l) { E = L; }
opfuncv(ld_e_hlp) { E = ReadByte(HL); }
opfuncv(ld_e_a) { E = A; }

opfuncv(ld_h_b) { H = B; }
opfuncv(ld_h_c) { H = C; }
opfuncv(ld_h_d) { H = D; }
opfuncv(ld_h_e) { H = E; }
opfuncv(ld_h_l) { H = L; }
opfuncv(ld_h_hlp) { H = ReadByte(HL); }
opfuncv(ld_h_a) { H = A; }

opfuncv(ld_l_b) { L = B; }
opfuncv(ld_l_c) { L = C; }
opfuncv(ld_l_d) { L = D; }
opfuncv(ld_l_e) { L = E; }
opfuncv(ld_l_h) { L = H; }
opfuncv(ld_l_hlp) { L = ReadByte(HL); }
opfuncv(ld_l_a) { L = A; }

opfuncv(ld_hlp_b) { WriteByte(HL, B); }
opfuncv(ld_hlp_c) { WriteByte(HL, C); }
opfuncv(ld_hlp_d) { WriteByte(HL, D); }
opfuncv(ld_hlp_e) { WriteByte(HL, E); }
opfuncv(ld_hlp_h) { WriteByte(HL, H); }
opfuncv(ld_hlp_l) { WriteByte(HL, L); }
opfuncv(halt) {
	if (__INT.master) {
		//HALT EXECUTION UNTIL AN INTERRUPT OCCURS
	}
	else PC++;
}
opfuncv(ld_hlp_a) { WriteByte(HL, A); }

opfuncv(ld_a_b) { A = B; }
opfuncv(ld_a_c) { A = C; }
opfuncv(ld_a_d) { A = D; }
opfuncv(ld_a_e) { A = E; }
opfuncv(ld_a_l) { A = L; }
opfuncv(ld_a_h) { A = H; }
opfuncv(ld_a_hlp) { A = ReadByte(HL); }

opfuncv(add_a_b) { ADD(A, B); }
opfuncv(add_a_c) { ADD(A, C); }
opfuncv(add_a_d) { ADD(A, D); }
opfuncv(add_a_e) { ADD(A, E); }
opfuncv(add_a_h) { ADD(A, H); }
opfuncv(add_a_l) { ADD(A, L); }
opfuncv(add_a_hlp) { ADD(A, ReadByte(HL)); }
opfuncv(add_a_a) { ADD(A, B); }

opfuncv(adc_b) { ADC(B); }
opfuncv(adc_c) { ADC(C); }
opfuncv(adc_d) { ADC(D); }
opfuncv(adc_e) { ADC(E); }
opfuncv(adc_h) { ADC(H); }
opfuncv(adc_l) { ADC(L); }
opfuncv(adc_hlp) { ADC(ReadByte(HL)); }
opfuncv(adc_a) { ADC(A); }

opfuncv(sub_b) { SUB(B); }
opfuncv(sub_c) { SUB(C); }
opfuncv(sub_d) { SUB(D); }
opfuncv(sub_e) { SUB(E); }
opfuncv(sub_h) { SUB(H); }
opfuncv(sub_l) { SUB(L); }
opfuncv(sub_hlp) { SUB(ReadByte(HL)); }
opfuncv(sub_a) { SUB(A); }

opfuncv(sbc_b) { SBC(B); }
opfuncv(sbc_c) { SBC(C); }
opfuncv(sbc_d) { SBC(D); }
opfuncv(sbc_e) { SBC(E); }
opfuncv(sbc_h) { SBC(H); }
opfuncv(sbc_l) { SBC(L); }
opfuncv(sbc_hlp) { SBC(ReadByte(HL)); }
opfuncv(sbc_a) { SBC(A); }

opfuncv(and_b) { AND(B); }
opfuncv(and_c) { AND(C); }
opfuncv(and_d) { AND(D); }
opfuncv(and_e) { AND(E); }
opfuncv(and_h) { AND(H); }
opfuncv(and_l) { AND(L); }
opfuncv(and_hlp) { AND(ReadByte(HL)); }
opfuncv(and_a) { AND(A); }

opfuncv(xor_b) { XOR(B); }
opfuncv(xor_c) { XOR(C); }
opfuncv(xor_d) { XOR(D); }
opfuncv(xor_e) { XOR(E); }
opfuncv(xor_h) { XOR(H); }
opfuncv(xor_l) { XOR(L); }
opfuncv(xor_hlp) { XOR(ReadByte(HL)); }
opfuncv(xor_a) { XOR(A); }

opfuncv(or_b) { OR(B); }
opfuncv(or_c) { OR(C); }
opfuncv(or_d) { OR(D); }
opfuncv(or_e) { OR(E); }
opfuncv(or_h) { OR(H); }
opfuncv(or_l) { OR(L); }
opfuncv(or_hlp) { OR(ReadByte(HL)); }
opfuncv(or_a) { OR(A); }

opfuncv(cp_b) { CP(B); }
opfuncv(cp_c) { CP(C); }
opfuncv(cp_d) { CP(D); }
opfuncv(cp_e) { CP(E); }
opfuncv(cp_h) { CP(H); }
opfuncv(cp_l) { CP(L); }
opfuncv(cp_hlp) { CP(ReadByte(HL)); }
opfuncv(cp_a) { CP(A); }

opfuncv(ret_nz) {
	if (ZERO_FLAG)
		TICKS += 8;
	else
	{
		PC = STACK_READ_WORD();
		TICKS += 20;
	}
}
opfuncv(pop_bc) { BC = STACK_READ_WORD(); }
opfuncw(jp_nz_nn) {
	if (ZERO_FLAG)
		TICKS += 12;
	else
	{
		PC = OPERAND;
		TICKS += 16;
	}
}
opfuncw(jp_nn) { PC = OPERAND; }
opfuncw(call_nz_nn) {
	if (ZERO_FLAG)
		TICKS += 12;
	else
	{
		STACK_WRITE_WORD(PC);
		PC = OPERAND;
		TICKS += 24;
	}
}
opfuncv(push_bc) { STACK_WRITE_WORD(BC); }
opfuncb(add_a_n) { ADD(A, OPERAND); }
opfuncv(rst_0) { STACK_WRITE_WORD(PC); PC = 0x0000; }
opfuncv(ret_z) {
	if (ZERO_FLAG)
	{
		PC = STACK_READ_WORD();
		TICKS += 20;
	}
	else TICKS += 8;
}
opfuncv(ret) { PC = STACK_READ_WORD(); }
opfuncw(jp_z_nn) {
	if (ZERO_FLAG)
	{
		PC = OPERAND;
		TICKS += 16;
	}
	else TICKS += 12;
}
opfuncb(cb_ext) { OpCB::cb_op(__ARGS, OPERAND); }
opfuncw(call_z_nn) {
	if (ZERO_FLAG)
	{
		STACK_WRITE_WORD(PC);
		PC = OPERAND;
		TICKS += 24;
	}
	else TICKS += 12;
}
opfuncw(call_nn) { STACK_WRITE_WORD(PC); PC = OPERAND; }
opfuncb(adc_n) { ADC(OPERAND); }
opfuncv(rst_08) { STACK_WRITE_WORD(PC); PC = 0x0008; }
opfuncv(ret_nc) {
	if (CARRY_FLAG)
		TICKS += 8;
	else
	{
		PC = STACK_READ_WORD();
		TICKS += 20;
	}
}
opfuncv(pop_de) { DE = STACK_READ_WORD(); }
opfuncw(jp_nc_nn) {
	if (CARRY_FLAG)
	{
		PC = OPERAND;
		TICKS += 16;
	}
	else TICKS += 12;
}
opfuncw(call_nc_nn) {
	if (CARRY_FLAG)
	{
		STACK_WRITE_WORD(PC);
		PC = OPERAND;
		TICKS += 24;
	}
	else TICKS += 12;
}
opfuncv(push_de) { STACK_WRITE_WORD(DE); }
opfuncb(sub_n) { SUB(OPERAND); }
opfuncv(rst_10) { STACK_WRITE_WORD(PC); PC = 0x0010; }
opfuncw(ret_c) {
	if (CARRY_FLAG)
	{
		PC = STACK_READ_WORD();
		TICKS += 20;
	}
	else TICKS += 8;
}
opfuncv(reti) { __INT.returnFromInterrupt(__ARGS); }
opfuncw(jp_c_nn) {
	if (CARRY_FLAG)
	{
		PC = OPERAND;
		TICKS += 16;
	}
	else TICKS += 12;
}
opfuncw(call_c_nn) {
	if (CARRY_FLAG)
	{
		STACK_WRITE_WORD(PC);
		PC = OPERAND;
		TICKS += 24;
	}
	else TICKS += 12;
}
opfuncb(sbc_n) { SBC(OPERAND); }
opfuncv(rst_18) { STACK_WRITE_WORD(PC); PC = 0x0018; }
opfuncb(ld_ff_n_ap) { WriteByte(0xFF00 + OPERAND, A); }
opfuncv(pop_hl) { HL = STACK_READ_WORD(); }
opfuncv(ld_ff_c_a) { WriteByte(0xFF00 + C, A); }
opfuncv(push_hl) { STACK_WRITE_WORD(HL); }
opfuncb(and_n) {
	A &= OPERAND;

	CLEAR_FLAG(CARRY_FLAG);
	CLEAR_FLAG(SUBTRACT_FLAG);
	SET_FLAG(HALFCARRY_FLAG);
	CLEAR_IF_ELSE_SET(ZERO_FLAG, A);
}
opfuncv(rst_20) { STACK_WRITE_WORD(PC); PC = 0x0020; }
opfuncb(add_sp_n) {
	int result = SP + SIGNED_BYTE(OPERAND);
	SET_IF_ELSE_CLEAR(CARRY_FLAG, result & 0xFFFF0000);

	SP = result & 0xFFFF;

	SET_IF_ELSE_CLEAR(HALFCARRY_FLAG, (SP & 0x0F) + (OPERAND & 0x0F) > 0x0F);
	CLEAR_FLAG(ZERO_FLAG);
	CLEAR_FLAG(SUBTRACT_FLAG);
}
opfuncv(jp_hl) { PC = HL; }
opfuncw(ld_nnp_a) { WriteByte(OPERAND, A); }
opfuncb(xor_n) { XOR(OPERAND); }
opfuncv(rst_28) { STACK_WRITE_WORD(PC); PC = 0x0028; }
opfuncb(ld_ff_ap_n) { A = ReadByte(0xFF00 + OPERAND); }
opfuncv(pop_af) { AF = STACK_READ_WORD(); }
opfuncv(ld_a_ff_c) { A = ReadByte(0xFF00 + C); }
opfuncv(di_inst) { __INT.master = false; }
opfuncv(push_af) { STACK_WRITE_WORD(AF); }
opfuncb(or_n) { OR(OPERAND); }
opfuncv(rst_30) { STACK_WRITE_WORD(PC); PC = 0x0030; }
opfuncb(ld_hl_sp_n) {
	int result = SP + SIGNED_BYTE(OPERAND);

	SET_IF_ELSE_CLEAR(CARRY_FLAG, result & 0xFFFF0000);
	SET_IF_ELSE_CLEAR(HALFCARRY_FLAG, (SP & 0x0F) + (OPERAND & 0x0F) > 0x0F);
	CLEAR_FLAG(ZERO_FLAG);
	CLEAR_FLAG(SUBTRACT_FLAG);

	HL = SIGNED_BYTE(result & 0xFFFF);
}
opfuncv(ld_sp_hl) { SP = HL; }
opfuncw(ld_a_nnp) { A = ReadByte(OPERAND); }
opfuncv(ei) { __INT.master = true; }
opfuncb(cp_n) {
	SET_FLAG(SUBTRACT_FLAG);

	SET_IF_ELSE_CLEAR(ZERO_FLAG, A == OPERAND);
	SET_IF_ELSE_CLEAR(CARRY_FLAG, OPERAND > A);
	SET_IF_ELSE_CLEAR(HALFCARRY_FLAG, (OPERAND & 0x0F) > (A & 0x0F));
}
opfuncv(rst_38) { STACK_WRITE_WORD(PC); PC = 0x0038; }















#define DEF_OPCODE(name, length, func) { (name), (length), (func) }
#define OP_VOID(func) DEF_OPCODE(#func, 0, static_cast<opfunc>(func))
#define OP_BYTE(func) DEF_OPCODE(#func, 1, static_cast<opfunc>(func))
#define OP_WORD(func) DEF_OPCODE(#func, 2, static_cast<opfunc>(func))
#define INVALID_OP {}



const Opcode OPCODES[256] = {
	/* 00 */ OP_VOID(nop),
	/* 01 */ OP_WORD(ld_bc_nn),
	/* 02 */ OP_VOID(ld_bcp_a),
	/* 03 */ OP_VOID(inc_bc),
	/* 04 */ OP_VOID(inc_b),
	/* 05 */ OP_VOID(dec_b),
	/* 06 */ OP_BYTE(ld_b_n),
	/* 07 */ OP_VOID(rlca),
	/* 08 */ OP_WORD(ld_nnp_sp),
	/* 09 */ OP_VOID(add_hl_bc),
	/* 0A */ OP_VOID(ld_a_bcp),
	/* 0B */ OP_VOID(dec_bc),
	/* 0C */ OP_VOID(inc_c),
	/* 0D */ OP_VOID(dec_c),
	/* 0E */ OP_BYTE(ld_c_n),
	/* 0F */ OP_VOID(rrca),

	/* 10 */ OP_BYTE(stop),
	/* 11 */ OP_WORD(ld_de_nn),
	/* 12 */ OP_VOID(ld_dep_a),
	/* 13 */ OP_VOID(inc_de),
	/* 14 */ OP_VOID(inc_d),
	/* 15 */ OP_VOID(dec_d),
	/* 16 */ OP_BYTE(ld_d_n),
	/* 17 */ OP_VOID(rla),
	/* 18 */ OP_BYTE(jr_n),
	/* 19 */ OP_VOID(add_hl_de),
	/* 1A */ OP_VOID(ld_a_dep),
	/* 1B */ OP_VOID(dec_de),
	/* 1C */ OP_VOID(inc_e),
	/* 1D */ OP_VOID(dec_e),
	/* 1E */ OP_BYTE(ld_e_n),
	/* 1F */ OP_VOID(rra),

	/* 20 */ OP_BYTE(jr_nz_n),
	/* 21 */ OP_WORD(ld_hl_nn),
	/* 22 */ OP_VOID(ldi_hlp_a),
	/* 23 */ OP_VOID(inc_hl),
	/* 24 */ OP_VOID(inc_h),
	/* 25 */ OP_VOID(dec_h),
	/* 26 */ OP_BYTE(ld_h_n),
	/* 27 */ OP_VOID(daa),
	/* 28 */ OP_BYTE(jr_z_n),
	/* 29 */ OP_VOID(add_hl_hl),
	/* 2A */ OP_VOID(ldi_a_hlp),
	/* 2B */ OP_VOID(dec_hl),
	/* 2C */ OP_VOID(inc_l),
	/* 2D */ OP_VOID(dec_l),
	/* 2E */ OP_BYTE(ld_l_n),
	/* 2F */ OP_VOID(cpl),

	/* 30 */ OP_BYTE(jr_nc_n),
	/* 31 */ OP_WORD(ld_sp_nn),
	/* 32 */ OP_VOID(ldd_hlp_a),
	/* 33 */ OP_VOID(inc_sp),
	/* 34 */ OP_VOID(inc_hlp),
	/* 35 */ OP_VOID(dec_hlp),
	/* 36 */ OP_BYTE(ld_hlp_n),
	/* 37 */ OP_VOID(scf),
	/* 38 */ OP_BYTE(jr_c_n),
	/* 39 */ OP_VOID(add_hl_sp),
	/* 3A */ OP_VOID(ldd_a_hlp),
	/* 3B */ OP_VOID(dec_sp),
	/* 3C */ OP_VOID(inc_a),
	/* 3D */ OP_VOID(dec_a),
	/* 3E */ OP_BYTE(ld_a_n),
	/* 3F */ OP_VOID(ccf),

	/* 40 */ OP_VOID(nop),
	/* 41 */ OP_VOID(ld_b_c),
	/* 42 */ OP_VOID(ld_b_d),
	/* 43 */ OP_VOID(ld_b_e),
	/* 44 */ OP_VOID(ld_b_h),
	/* 45 */ OP_VOID(ld_b_l),
	/* 46 */ OP_VOID(ld_b_hlp),
	/* 47 */ OP_VOID(ld_b_a),
	/* 48 */ OP_VOID(ld_c_b),
	/* 49 */ OP_VOID(nop),
	/* 4A */ OP_VOID(ld_c_d),
	/* 4B */ OP_VOID(ld_c_e),
	/* 4C */ OP_VOID(ld_c_h),
	/* 4D */ OP_VOID(ld_c_l),
	/* 4E */ OP_VOID(ld_c_hlp),
	/* 4F */ OP_VOID(ld_c_a),

	/* 50 */ OP_VOID(ld_d_b),
	/* 51 */ OP_VOID(ld_d_c),
	/* 52 */ OP_VOID(nop),
	/* 53 */ OP_VOID(ld_d_e),
	/* 54 */ OP_VOID(ld_d_h),
	/* 55 */ OP_VOID(ld_d_l),
	/* 56 */ OP_VOID(ld_d_hlp),
	/* 57 */ OP_VOID(ld_d_a),
	/* 58 */ OP_VOID(ld_e_b),
	/* 59 */ OP_VOID(ld_e_c),
	/* 5A */ OP_VOID(ld_e_d),
	/* 5B */ OP_VOID(nop),
	/* 5C */ OP_VOID(ld_e_h),
	/* 5D */ OP_VOID(ld_e_l),
	/* 5E */ OP_VOID(ld_e_hlp),
	/* 5F */ OP_VOID(ld_e_a),

	/* 60 */ OP_VOID(ld_h_b),
	/* 61 */ OP_VOID(ld_h_c),
	/* 62 */ OP_VOID(ld_h_d),
	/* 63 */ OP_VOID(ld_h_e),
	/* 64 */ OP_VOID(nop),
	/* 65 */ OP_VOID(ld_h_l),
	/* 66 */ OP_VOID(ld_h_hlp),
	/* 67 */ OP_VOID(ld_h_a),
	/* 68 */ OP_VOID(ld_l_b),
	/* 69 */ OP_VOID(ld_l_c),
	/* 6A */ OP_VOID(ld_l_d),
	/* 6B */ OP_VOID(ld_l_e),
	/* 6C */ OP_VOID(ld_l_h),
	/* 6D */ OP_VOID(nop),
	/* 6E */ OP_VOID(ld_l_hlp),
	/* 6F */ OP_VOID(ld_l_a),

	/* 70 */ OP_VOID(ld_hlp_b),
	/* 71 */ OP_VOID(ld_hlp_c),
	/* 72 */ OP_VOID(ld_hlp_d),
	/* 73 */ OP_VOID(ld_hlp_e),
	/* 74 */ OP_VOID(ld_hlp_h),
	/* 75 */ OP_VOID(ld_hlp_l),
	/* 76 */ OP_VOID(halt),
	/* 77 */ OP_VOID(ld_hlp_a),
	/* 78 */ OP_VOID(ld_a_b),
	/* 79 */ OP_VOID(ld_a_c),
	/* 7A */ OP_VOID(ld_a_d),
	/* 7B */ OP_VOID(ld_a_e),
	/* 7C */ OP_VOID(ld_a_h),
	/* 7D */ OP_VOID(ld_a_l),
	/* 7E */ OP_VOID(ld_l_hlp),
	/* 7F */ OP_VOID(nop),

	/* 80 */ OP_VOID(add_a_b),
	/* 81 */ OP_VOID(add_a_c),
	/* 82 */ OP_VOID(add_a_d),
	/* 83 */ OP_VOID(add_a_e),
	/* 84 */ OP_VOID(add_a_h),
	/* 85 */ OP_VOID(add_a_l),
	/* 86 */ OP_VOID(add_a_hlp),
	/* 87 */ OP_VOID(add_a_a),
	/* 88 */ OP_VOID(adc_b),
	/* 89 */ OP_VOID(adc_c),
	/* 8A */ OP_VOID(adc_d),
	/* 8B */ OP_VOID(adc_e),
	/* 8C */ OP_VOID(adc_h),
	/* 8D */ OP_VOID(adc_l),
	/* 8E */ OP_VOID(adc_hlp),
	/* 8F */ OP_VOID(adc_a),

	/* 90 */ OP_VOID(sub_b),
	/* 91 */ OP_VOID(sub_c),
	/* 92 */ OP_VOID(sub_d),
	/* 93 */ OP_VOID(sub_e),
	/* 94 */ OP_VOID(sub_h),
	/* 95 */ OP_VOID(sub_l),
	/* 96 */ OP_VOID(sub_hlp),
	/* 97 */ OP_VOID(sub_a),
	/* 98 */ OP_VOID(sbc_b),
	/* 99 */ OP_VOID(sbc_c),
	/* 9A */ OP_VOID(sbc_d),
	/* 9B */ OP_VOID(sbc_e),
	/* 9C */ OP_VOID(sbc_h),
	/* 9D */ OP_VOID(sbc_l),
	/* 9E */ OP_VOID(sbc_hlp),
	/* 9F */ OP_VOID(sbc_a),

	/* A0 */ OP_VOID(and_b),
	/* A1 */ OP_VOID(and_c),
	/* A2 */ OP_VOID(and_d),
	/* A3 */ OP_VOID(and_e),
	/* A4 */ OP_VOID(and_h),
	/* A5 */ OP_VOID(and_l),
	/* A6 */ OP_VOID(and_hlp),
	/* A7 */ OP_VOID(and_a),
	/* A8 */ OP_VOID(xor_b),
	/* A9 */ OP_VOID(xor_c),
	/* AA */ OP_VOID(xor_d),
	/* AB */ OP_VOID(xor_e),
	/* AC */ OP_VOID(xor_h),
	/* AD */ OP_VOID(xor_l),
	/* AE */ OP_VOID(xor_hlp),
	/* AF */ OP_VOID(xor_a),

	/* B0 */ OP_VOID(or_b),
	/* B1 */ OP_VOID(or_c),
	/* B2 */ OP_VOID(or_d),
	/* B3 */ OP_VOID(or_e),
	/* B4 */ OP_VOID(or_h),
	/* B5 */ OP_VOID(or_l),
	/* B6 */ OP_VOID(or_hlp),
	/* B7 */ OP_VOID(or_a),
	/* B8 */ OP_VOID(cp_b),
	/* B9 */ OP_VOID(cp_c),
	/* BA */ OP_VOID(cp_d),
	/* BB */ OP_VOID(cp_e),
	/* BC */ OP_VOID(cp_h),
	/* BD */ OP_VOID(cp_l),
	/* BE */ OP_VOID(cp_hlp),
	/* BF */ OP_VOID(cp_a),

	/* C0 */ OP_VOID(ret_nz),
	/* C1 */ OP_VOID(pop_bc),
	/* C2 */ OP_WORD(jp_nz_nn),
	/* C3 */ OP_WORD(jp_nn),
	/* C4 */ OP_WORD(call_nz_nn),
	/* C5 */ OP_VOID(push_bc),
	/* C6 */ OP_BYTE(add_a_n),
	/* C7 */ OP_VOID(rst_0),
	/* C8 */ OP_VOID(ret_z),
	/* C9 */ OP_VOID(ret),
	/* CA */ OP_WORD(jp_z_nn),
	/* CB */ OP_BYTE(cb_ext),
	/* CC */ OP_WORD(call_z_nn),
	/* CD */ OP_WORD(call_nn),
	/* CE */ OP_BYTE(adc_n),
	/* CF */ OP_VOID(rst_08),

	/* D0 */ OP_VOID(ret_nc),
	/* D1 */ OP_VOID(pop_de),
	/* D2 */ OP_WORD(jp_nc_nn),
	/* D3 */ INVALID_OP,
	/* D4 */ OP_WORD(call_nc_nn),
	/* D5 */ OP_VOID(push_de),
	/* D6 */ OP_BYTE(sub_n),
	/* D7 */ OP_VOID(rst_10),
	/* D8 */ OP_WORD(ret_c),
	/* D9 */ OP_VOID(reti),
	/* DA */ OP_WORD(jp_c_nn),
	/* DB */ INVALID_OP,
	/* DC */ OP_WORD(call_c_nn),
	/* DD */ INVALID_OP,
	/* DE */ OP_BYTE(sbc_n),
	/* DF */ OP_VOID(rst_18),

	/* E0 */ OP_BYTE(ld_ff_n_ap),
	/* E1 */ OP_VOID(pop_hl),
	/* E2 */ OP_VOID(ld_ff_c_a),
	/* E3 */ INVALID_OP,
	/* E4 */ INVALID_OP,
	/* E5 */ OP_VOID(push_hl),
	/* E6 */ OP_BYTE(and_n),
	/* E7 */ OP_VOID(rst_20),
	/* E8 */ OP_BYTE(add_sp_n),
	/* E9 */ OP_VOID(jp_hl),
	/* EA */ OP_WORD(ld_nnp_a),
	/* EB */ INVALID_OP,
	/* EC */ INVALID_OP,
	/* ED */ INVALID_OP,
	/* EE */ OP_BYTE(xor_n),
	/* EF */ OP_VOID(rst_28),

	/* F0 */ OP_VOID(ld_ff_ap_n),
	/* F1 */ OP_VOID(pop_af),
	/* F2 */ OP_VOID(ld_a_ff_c),
	/* F3 */ OP_VOID(di_inst),
	/* F4 */ INVALID_OP,
	/* F5 */ OP_VOID(push_af),
	/* F6 */ OP_VOID(or_n),
	/* F7 */ OP_VOID(rst_30),
	/* F8 */ OP_VOID(ld_hl_sp_n),
	/* F9 */ OP_VOID(ld_sp_hl),
	/* FA */ OP_VOID(ld_a_nnp),
	/* FB */ OP_VOID(ei),
	/* FC */ INVALID_OP,
	/* FD */ INVALID_OP,
	/* FE */ OP_VOID(cp_n),
	/* FF */ OP_VOID(rst_38)
};


const byte_t OPCODE_TICKS[256] = {
	/* 0x */ 4,  12, 8, 8,  4,  4,  8,  4,  20, 8,  8,  8, 4, 4,  8, 4,
	/* 1x */ 4,  12, 8, 8,  4,  4,  8,  4,  12, 8,  8,  8, 4, 4,  8, 4,
	/* 2x */ 0,  12, 8, 8,  4,  4,  8,  4,  0,  8,  8,  8, 4, 4,  8, 4,
	/* 3x */ 0,  12, 8, 8,  12, 12, 12, 4,  0,  8,  8,  8, 4, 4,  8, 4,
	/* 4x */ 4,  4,  4, 4,  4,  4,  8,  4,  4,  4,  4,  4, 4, 4,  8, 4,
	/* 5x */ 4,  4,  4, 4,  4,  4,  8,  4,  4,  4,  4,  4, 4, 4,  8, 4,
	/* 6x */ 4,  4,  4, 4,  4,  4,  8,  4,  4,  4,  4,  4, 4, 4,  8, 4,
	/* 7x */ 8,  8,  8, 8,  8,  8,  4,  4,  4,  4,  4,  4, 4, 4,  8, 4,
	/* 8x */ 4,  4,  4, 4,  4,  4,  8,  4,  4,  4,  4,  4, 4, 4,  8, 4,
	/* 9x */ 4,  4,  4, 4,  4,  4,  8,  4,  4,  4,  4,  4, 4, 4,  8, 4,
	/* Ax */ 4,  4,  4, 4,  4,  4,  8,  4,  4,  4,  4,  4, 4, 4,  8, 4,
	/* Bx */ 4,  4,  4, 4,  4,  4,  8,  4,  4,  4,  4,  4, 4, 4,  8, 4,
	/* Cx */ 0,  12, 0, 16, 0,  16, 8,  16, 0,  16, 0,  4, 0, 24, 8, 16,
	/* Dx */ 0,  12, 0, 0,  0,  16, 8,  16, 0,  16, 0,  0, 0, 0,  8, 16,
	/* Ex */ 12, 12, 8, 0,  0 , 16, 8,  16, 16, 4,  16, 0, 0, 0,  8, 16,
	/* Fx */ 12, 12, 8, 4,  0,  16, 8,  16, 12, 8,  16, 4, 0, 0,  8, 16
};

