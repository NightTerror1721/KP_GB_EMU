#include "cpu.h"

void invalid_opcode(MMU* const& mmu, CPU* const& cpu) {}


Opcode::Opcode(const std::string& name, uint8_t length, opfunc_w func) :
	_name(name),
	_length(length),
	_func(static_cast<opfunc>(func))
{}

Opcode::Opcode(const std::string& name, uint8_t length, opfunc_b func) :
	_name(name),
	_length(length),
	_func(static_cast<opfunc>(func))
{}

Opcode::Opcode(const std::string& name, uint8_t length, opfunc_v func) :
	_name(name),
	_length(length),
	_func(static_cast<opfunc>(func))
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

void Opcode::undefined()
{

}


CPU::CPU() :
	regs(),
	ints(),
	ticks(0),
	_stopped(false)
{}

void CPU::step(MMU* const& mmu)
{

}

void CPU::reset()
{

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
#define __ARGS mmu, cpu
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













#define DEF_OPCODE(name, length, func) { (name), (length), (func) }
#define OPV(func) DEF_OPCODE(#func, 0, static_cast<opfunc_v>(func))
#define OPB(func) DEF_OPCODE(#func, 1, static_cast<opfunc_b>(func))
#define OPW(func) DEF_OPCODE(#func, 2, static_cast<opfunc_w>(func))
#define INVALID_OP Opcode()



const Opcode OPCODES[256] = {
	/* 00 */ OPV(nop),
	/* 01 */ OPW(ld_bc_nn),
	/* 02 */ OPV(ld_bcp_a),
	/* 03 */ OPV(inc_bc),
	/* 04 */ OPV(inc_b),
	/* 05 */ OPV(dec_b),
	/* 06 */ OPB(ld_b_n),
	/* 07 */ OPV(rlca)
};

