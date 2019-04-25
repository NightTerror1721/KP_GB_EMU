#include "extended_opcodes.h"
#include "mmu.h"
#include "cpu.h"


#define BYTE byte_t
#define WORD word_t

#define ZERO_FLAG cpu->regs.ZeroFlag
#define SUBTRACT_FLAG cpu->regs.SubtractFlag
#define HALFCARRY_FLAG cpu->regs.HalfCarryFlag
#define CARRY_FLAG cpu->regs.CarryFlag

#define SET_FLAG(flag) (flag) = ENABLED_FLAG
#define CLEAR_FLAG(flag) (flag) = DISABLED_FLAG

#define SET_IF_ELSE_CLEAR(flag, if_statement) if(if_statement) SET_FLAG(flag); else CLEAR_FLAG(flag)
#define CLEAR_IF_ELSE_SET(flag, if_statement) if(if_statement) CLEAR_FLAG(flag); else SET_FLAG(flag)

enum REG : BYTE {
	B = BYTE_OFFSET_REG_B,
	C = BYTE_OFFSET_REG_C,
	D = BYTE_OFFSET_REG_D,
	E = BYTE_OFFSET_REG_E,
	H = BYTE_OFFSET_REG_H,
	L = BYTE_OFFSET_REG_L,
	A = BYTE_OFFSET_REG_A
};

enum bit_t : BYTE {
	b0 = (1 << 0),
	b1 = (1 << 1),
	b2 = (1 << 2),
	b3 = (1 << 3),
	b4 = (1 << 4),
	b5 = (1 << 5),
	b6 = (1 << 6),
	b7 = (1 << 7)
};


struct ExtendedOpcode
{
	const bool hlp;
	BYTE ticks() const { return hlp ? 16 : 8; }
	ExtendedOpcode(const bool& hlp) : hlp(hlp) {}
	virtual void operator() (MMU* const& mmu, CPU* const& cpu) const = 0;
};

struct RLC : public ExtendedOpcode
{
	const REG reg;

	RLC() : ExtendedOpcode(true), reg() {}
	RLC(const REG& reg) : ExtendedOpcode(false), reg(reg) {}

	void operator() (MMU* const& mmu, CPU* const& cpu) const
	{
		BYTE value = hlp ? mmu->readByte(cpu->regs.HL) : cpu->regs.data8[reg];

		int carry = (value & 0x80) >> 7;
		SET_IF_ELSE_CLEAR(CARRY_FLAG, value & 0x80);

		value <<= 1;
		value += carry;

		CLEAR_IF_ELSE_SET(ZERO_FLAG, value);
		CLEAR_FLAG(SUBTRACT_FLAG);
		CLEAR_FLAG(HALFCARRY_FLAG);

		if (hlp)
			mmu->writeByte(cpu->regs.HL, value);
		else cpu->regs.data8[reg] = value;
	}
};

struct RRC : public ExtendedOpcode
{
	const REG reg;

	RRC() : ExtendedOpcode(true), reg() {}
	RRC(const REG& reg) : ExtendedOpcode(false), reg(reg) {}

	void operator() (MMU* const& mmu, CPU* const& cpu) const
	{
		BYTE value = hlp ? mmu->readByte(cpu->regs.HL) : cpu->regs.data8[reg];

		int carry = value & 0x01;

		value >>= 1;
		if (carry)
		{
			SET_FLAG(CARRY_FLAG);
			value |= 0x80;
		}
		else CLEAR_FLAG(CARRY_FLAG);

		CLEAR_IF_ELSE_SET(ZERO_FLAG, value);
		CLEAR_FLAG(SUBTRACT_FLAG);
		CLEAR_FLAG(HALFCARRY_FLAG);

		if (hlp)
			mmu->writeByte(cpu->regs.HL, value);
		else cpu->regs.data8[reg] = value;
	}
};

struct RL : public ExtendedOpcode
{
	const REG reg;

	RL() : ExtendedOpcode(true), reg() {}
	RL(const REG& reg) : ExtendedOpcode(false), reg(reg) {}

	void operator() (MMU* const& mmu, CPU* const& cpu) const
	{
		BYTE value = hlp ? mmu->readByte(cpu->regs.HL) : cpu->regs.data8[reg];

		int carry = CARRY_FLAG ? 1 : 0;
		SET_IF_ELSE_CLEAR(CARRY_FLAG, value & 0x80);

		value <<= 1;
		value += carry;

		CLEAR_IF_ELSE_SET(ZERO_FLAG, value);
		CLEAR_FLAG(SUBTRACT_FLAG);
		CLEAR_FLAG(HALFCARRY_FLAG);

		if (hlp)
			mmu->writeByte(cpu->regs.HL, value);
		else cpu->regs.data8[reg] = value;
	}
};

struct RR : public ExtendedOpcode
{
	const REG reg;

	RR() : ExtendedOpcode(true), reg() {}
	RR(const REG& reg) : ExtendedOpcode(false), reg(reg) {}

	void operator() (MMU* const& mmu, CPU* const& cpu) const
	{
		BYTE value = hlp ? mmu->readByte(cpu->regs.HL) : cpu->regs.data8[reg];

		value >>= 1;
		if (CARRY_FLAG)
			value |= 0x80;

		SET_IF_ELSE_CLEAR(CARRY_FLAG, value & 0x01);

		CLEAR_IF_ELSE_SET(ZERO_FLAG, value);
		CLEAR_FLAG(SUBTRACT_FLAG);
		CLEAR_FLAG(HALFCARRY_FLAG);

		if (hlp)
			mmu->writeByte(cpu->regs.HL, value);
		else cpu->regs.data8[reg] = value;
	}
};

struct SLA : public ExtendedOpcode
{
	const REG reg;

	SLA() : ExtendedOpcode(true), reg() {}
	SLA(const REG& reg) : ExtendedOpcode(false), reg(reg) {}

	void operator() (MMU* const& mmu, CPU* const& cpu) const
	{
		BYTE value = hlp ? mmu->readByte(cpu->regs.HL) : cpu->regs.data8[reg];

		SET_IF_ELSE_CLEAR(CARRY_FLAG, value & 0x01);
		value <<= 1;

		CLEAR_IF_ELSE_SET(ZERO_FLAG, value);
		CLEAR_FLAG(SUBTRACT_FLAG);
		CLEAR_FLAG(HALFCARRY_FLAG);

		if (hlp)
			mmu->writeByte(cpu->regs.HL, value);
		else cpu->regs.data8[reg] = value;
	}
};

struct SRA : public ExtendedOpcode
{
	const REG reg;

	SRA() : ExtendedOpcode(true), reg() {}
	SRA(const REG& reg) : ExtendedOpcode(false), reg(reg) {}

	void operator() (MMU* const& mmu, CPU* const& cpu) const
	{
		BYTE value = hlp ? mmu->readByte(cpu->regs.HL) : cpu->regs.data8[reg];

		SET_IF_ELSE_CLEAR(CARRY_FLAG, value & 0x01);
		value = (value & 0x80) | (value >> 1);

		CLEAR_IF_ELSE_SET(ZERO_FLAG, value);
		CLEAR_FLAG(SUBTRACT_FLAG);
		CLEAR_FLAG(HALFCARRY_FLAG);

		if (hlp)
			mmu->writeByte(cpu->regs.HL, value);
		else cpu->regs.data8[reg] = value;
	}
};

struct SWAP : public ExtendedOpcode
{
	const REG reg;

	SWAP() : ExtendedOpcode(true), reg() {}
	SWAP(const REG& reg) : ExtendedOpcode(false), reg(reg) {}

	void operator() (MMU* const& mmu, CPU* const& cpu) const
	{
		BYTE value = hlp ? mmu->readByte(cpu->regs.HL) : cpu->regs.data8[reg];

		value = ((value & 0x0f) << 4) | ((value & 0xf0) >> 4);

		CLEAR_IF_ELSE_SET(ZERO_FLAG, value);
		CLEAR_FLAG(SUBTRACT_FLAG);
		CLEAR_FLAG(HALFCARRY_FLAG);
		CLEAR_FLAG(CARRY_FLAG);

		if (hlp)
			mmu->writeByte(cpu->regs.HL, value);
		else cpu->regs.data8[reg] = value;
	}
};

struct SRL : public ExtendedOpcode
{
	const REG reg;

	SRL() : ExtendedOpcode(true), reg() {}
	SRL(const REG& reg) : ExtendedOpcode(false), reg(reg) {}

	void operator() (MMU* const& mmu, CPU* const& cpu) const
	{
		BYTE value = hlp ? mmu->readByte(cpu->regs.HL) : cpu->regs.data8[reg];

		SET_IF_ELSE_CLEAR(CARRY_FLAG, value & 0x01);
		value >>= 1;

		CLEAR_IF_ELSE_SET(ZERO_FLAG, value);
		CLEAR_FLAG(SUBTRACT_FLAG);
		CLEAR_FLAG(HALFCARRY_FLAG);

		if (hlp)
			mmu->writeByte(cpu->regs.HL, value);
		else cpu->regs.data8[reg] = value;
	}
};

struct BIT : public ExtendedOpcode
{
	const bit_t bit;
	const REG reg;

	BIT(const bit_t& bit) : ExtendedOpcode(true), bit(bit), reg() {}
	BIT(const bit_t& bit, const REG& reg) : ExtendedOpcode(false), bit(bit), reg(reg) {}

	void operator() (MMU* const& mmu, CPU* const& cpu) const
	{
		BYTE value = hlp ? mmu->readByte(cpu->regs.HL) : cpu->regs.data8[reg];

		CLEAR_IF_ELSE_SET(ZERO_FLAG, value & bit);
		CLEAR_FLAG(SUBTRACT_FLAG);
		SET_FLAG(HALFCARRY_FLAG);
	}
};

struct RES : public ExtendedOpcode
{
	const bit_t bit;
	const REG reg;

	RES(const bit_t& bit) : ExtendedOpcode(true), bit(bit), reg() {}
	RES(const bit_t& bit, const REG& reg) : ExtendedOpcode(false), bit(bit), reg(reg) {}

	void operator() (MMU* const& mmu, CPU* const& cpu) const
	{
		if (hlp)
			mmu->writeByte(cpu->regs.HL, mmu->readByte(cpu->regs.HL) & ~bit);
		else cpu->regs.data8[reg] = cpu->regs.data8[reg] & ~bit;
	}
};

struct SET : public ExtendedOpcode
{
	const bit_t bit;
	const REG reg;

	SET(const bit_t& bit) : ExtendedOpcode(true), bit(bit), reg() {}
	SET(const bit_t& bit, const REG& reg) : ExtendedOpcode(false), bit(bit), reg(reg) {}

	void operator() (MMU* const& mmu, CPU* const& cpu) const
	{
		if (hlp)
			mmu->writeByte(cpu->regs.HL, mmu->readByte(cpu->regs.HL) | bit);
		else cpu->regs.data8[reg] = cpu->regs.data8[reg] | bit;
	}
};


const RLC RLC_OPS[8] = {
	/* 00 */ RLC(REG::B),
	/* 01 */ RLC(REG::C),
	/* 02 */ RLC(REG::D),
	/* 03 */ RLC(REG::E),
	/* 04 */ RLC(REG::H),
	/* 05 */ RLC(REG::L),
	/* 06 */ RLC(),
	/* 07 */ RLC(REG::A)
};

const RRC RRC_OPS[8] = {
	/* 08 */ RRC(REG::B),
	/* 09 */ RRC(REG::C),
	/* 0A */ RRC(REG::D),
	/* 0B */ RRC(REG::E),
	/* 0C */ RRC(REG::H),
	/* 0D */ RRC(REG::L),
	/* 0E */ RRC(),
	/* 0F */ RRC(REG::A)
};

const RL RL_OPS[8] = {
	/* 10 */ RL(REG::B),
	/* 11 */ RL(REG::C),
	/* 12 */ RL(REG::D),
	/* 13 */ RL(REG::E),
	/* 14 */ RL(REG::H),
	/* 15 */ RL(REG::L),
	/* 16 */ RL(),
	/* 17 */ RL(REG::A)
};

const RR RR_OPS[8] = {
	/* 18 */ RR(REG::B),
	/* 19 */ RR(REG::C),
	/* 1A */ RR(REG::D),
	/* 1B */ RR(REG::E),
	/* 1C */ RR(REG::H),
	/* 1D */ RR(REG::L),
	/* 1E */ RR(),
	/* 1F */ RR(REG::A)
};

const SLA SLA_OPS[8] = {
	/* 20 */ SLA(REG::B),
	/* 21 */ SLA(REG::C),
	/* 22 */ SLA(REG::D),
	/* 23 */ SLA(REG::E),
	/* 24 */ SLA(REG::H),
	/* 25 */ SLA(REG::L),
	/* 26 */ SLA(),
	/* 27 */ SLA(REG::A)
};

const SRA SRA_OPS[8] = {
	/* 28 */ SRA(REG::B),
	/* 29 */ SRA(REG::C),
	/* 2A */ SRA(REG::D),
	/* 2B */ SRA(REG::E),
	/* 2C */ SRA(REG::H),
	/* 2D */ SRA(REG::L),
	/* 2E */ SRA(),
	/* 2F */ SRA(REG::A)
};

const SWAP SWAP_OPS[8] = {
	/* 30 */ SWAP(REG::B),
	/* 31 */ SWAP(REG::C),
	/* 32 */ SWAP(REG::D),
	/* 33 */ SWAP(REG::E),
	/* 34 */ SWAP(REG::H),
	/* 35 */ SWAP(REG::L),
	/* 36 */ SWAP(),
	/* 37 */ SWAP(REG::A)
};

const SRL SRL_OPS[8] = {
	/* 38 */ SRL(REG::B),
	/* 39 */ SRL(REG::C),
	/* 3A */ SRL(REG::D),
	/* 3B */ SRL(REG::E),
	/* 3C */ SRL(REG::H),
	/* 3D */ SRL(REG::L),
	/* 3E */ SRL(),
	/* 3F */ SRL(REG::A)
};

const BIT BIT_OPS[64] = {
	/* 40 */ BIT(bit_t::b0, REG::B),
	/* 41 */ BIT(bit_t::b0, REG::C),
	/* 42 */ BIT(bit_t::b0, REG::D),
	/* 43 */ BIT(bit_t::b0, REG::E),
	/* 44 */ BIT(bit_t::b0, REG::H),
	/* 45 */ BIT(bit_t::b0, REG::L),
	/* 46 */ BIT(bit_t::b0),
	/* 47 */ BIT(bit_t::b0, REG::A),
	/* 48 */ BIT(bit_t::b1, REG::B),
	/* 49 */ BIT(bit_t::b1, REG::C),
	/* 4A */ BIT(bit_t::b1, REG::D),
	/* 4B */ BIT(bit_t::b1, REG::E),
	/* 4C */ BIT(bit_t::b1, REG::H),
	/* 4D */ BIT(bit_t::b1, REG::L),
	/* 4E */ BIT(bit_t::b1),
	/* 4F */ BIT(bit_t::b1, REG::A),
	/* 50 */ BIT(bit_t::b2, REG::B),
	/* 51 */ BIT(bit_t::b2, REG::C),
	/* 52 */ BIT(bit_t::b2, REG::D),
	/* 53 */ BIT(bit_t::b2, REG::E),
	/* 54 */ BIT(bit_t::b2, REG::H),
	/* 55 */ BIT(bit_t::b2, REG::L),
	/* 56 */ BIT(bit_t::b2),
	/* 57 */ BIT(bit_t::b2, REG::A),
	/* 58 */ BIT(bit_t::b3, REG::B),
	/* 59 */ BIT(bit_t::b3, REG::C),
	/* 5A */ BIT(bit_t::b3, REG::D),
	/* 5B */ BIT(bit_t::b3, REG::E),
	/* 5C */ BIT(bit_t::b3, REG::H),
	/* 5D */ BIT(bit_t::b3, REG::L),
	/* 5E */ BIT(bit_t::b3),
	/* 5F */ BIT(bit_t::b3, REG::A),
	/* 60 */ BIT(bit_t::b4, REG::B),
	/* 61 */ BIT(bit_t::b4, REG::C),
	/* 62 */ BIT(bit_t::b4, REG::D),
	/* 63 */ BIT(bit_t::b4, REG::E),
	/* 64 */ BIT(bit_t::b4, REG::H),
	/* 65 */ BIT(bit_t::b4, REG::L),
	/* 66 */ BIT(bit_t::b4),
	/* 67 */ BIT(bit_t::b4, REG::A),
	/* 68 */ BIT(bit_t::b5, REG::B),
	/* 69 */ BIT(bit_t::b5, REG::C),
	/* 6A */ BIT(bit_t::b5, REG::D),
	/* 6B */ BIT(bit_t::b5, REG::E),
	/* 6C */ BIT(bit_t::b5, REG::H),
	/* 6D */ BIT(bit_t::b5, REG::L),
	/* 6E */ BIT(bit_t::b5),
	/* 6F */ BIT(bit_t::b5, REG::A),
	/* 70 */ BIT(bit_t::b6, REG::B),
	/* 71 */ BIT(bit_t::b6, REG::C),
	/* 72 */ BIT(bit_t::b6, REG::D),
	/* 73 */ BIT(bit_t::b6, REG::E),
	/* 74 */ BIT(bit_t::b6, REG::H),
	/* 75 */ BIT(bit_t::b6, REG::L),
	/* 76 */ BIT(bit_t::b6),
	/* 77 */ BIT(bit_t::b6, REG::A),
	/* 78 */ BIT(bit_t::b7, REG::B),
	/* 79 */ BIT(bit_t::b7, REG::C),
	/* 7A */ BIT(bit_t::b7, REG::D),
	/* 7B */ BIT(bit_t::b7, REG::E),
	/* 7C */ BIT(bit_t::b7, REG::H),
	/* 7D */ BIT(bit_t::b7, REG::L),
	/* 7E */ BIT(bit_t::b7),
	/* 7F */ BIT(bit_t::b7, REG::A)
};

const RES RES_OPS[64] = {
	/* 80 */ RES(bit_t::b0, REG::B),
	/* 81 */ RES(bit_t::b0, REG::C),
	/* 82 */ RES(bit_t::b0, REG::D),
	/* 83 */ RES(bit_t::b0, REG::E),
	/* 84 */ RES(bit_t::b0, REG::H),
	/* 85 */ RES(bit_t::b0, REG::L),
	/* 86 */ RES(bit_t::b0),
	/* 87 */ RES(bit_t::b0, REG::A),
	/* 88 */ RES(bit_t::b1, REG::B),
	/* 89 */ RES(bit_t::b1, REG::C),
	/* 8A */ RES(bit_t::b1, REG::D),
	/* 8B */ RES(bit_t::b1, REG::E),
	/* 8C */ RES(bit_t::b1, REG::H),
	/* 8D */ RES(bit_t::b1, REG::L),
	/* 8E */ RES(bit_t::b1),
	/* 8F */ RES(bit_t::b1, REG::A),
	/* 90 */ RES(bit_t::b2, REG::B),
	/* 91 */ RES(bit_t::b2, REG::C),
	/* 92 */ RES(bit_t::b2, REG::D),
	/* 93 */ RES(bit_t::b2, REG::E),
	/* 94 */ RES(bit_t::b2, REG::H),
	/* 95 */ RES(bit_t::b2, REG::L),
	/* 96 */ RES(bit_t::b2),
	/* 97 */ RES(bit_t::b2, REG::A),
	/* 98 */ RES(bit_t::b3, REG::B),
	/* 99 */ RES(bit_t::b3, REG::C),
	/* 9A */ RES(bit_t::b3, REG::D),
	/* 9B */ RES(bit_t::b3, REG::E),
	/* 9C */ RES(bit_t::b3, REG::H),
	/* 9D */ RES(bit_t::b3, REG::L),
	/* 9E */ RES(bit_t::b3),
	/* 9F */ RES(bit_t::b3, REG::A),
	/* A0 */ RES(bit_t::b4, REG::B),
	/* A1 */ RES(bit_t::b4, REG::C),
	/* A2 */ RES(bit_t::b4, REG::D),
	/* A3 */ RES(bit_t::b4, REG::E),
	/* A4 */ RES(bit_t::b4, REG::H),
	/* A5 */ RES(bit_t::b4, REG::L),
	/* A6 */ RES(bit_t::b4),
	/* A7 */ RES(bit_t::b4, REG::A),
	/* A8 */ RES(bit_t::b5, REG::B),
	/* A9 */ RES(bit_t::b5, REG::C),
	/* AA */ RES(bit_t::b5, REG::D),
	/* AB */ RES(bit_t::b5, REG::E),
	/* AC */ RES(bit_t::b5, REG::H),
	/* AD */ RES(bit_t::b5, REG::L),
	/* AE */ RES(bit_t::b5),
	/* AF */ RES(bit_t::b5, REG::A),
	/* B0 */ RES(bit_t::b6, REG::B),
	/* B1 */ RES(bit_t::b6, REG::C),
	/* B2 */ RES(bit_t::b6, REG::D),
	/* B3 */ RES(bit_t::b6, REG::E),
	/* B4 */ RES(bit_t::b6, REG::H),
	/* B5 */ RES(bit_t::b6, REG::L),
	/* B6 */ RES(bit_t::b6),
	/* B7 */ RES(bit_t::b6, REG::A),
	/* B8 */ RES(bit_t::b7, REG::B),
	/* B9 */ RES(bit_t::b7, REG::C),
	/* BA */ RES(bit_t::b7, REG::D),
	/* BB */ RES(bit_t::b7, REG::E),
	/* BC */ RES(bit_t::b7, REG::H),
	/* BD */ RES(bit_t::b7, REG::L),
	/* BE */ RES(bit_t::b7),
	/* BF */ RES(bit_t::b7, REG::A)
};

const SET SET_OPS[64] = {
	/* C0 */ SET(bit_t::b0, REG::B),
	/* C1 */ SET(bit_t::b0, REG::C),
	/* C2 */ SET(bit_t::b0, REG::D),
	/* C3 */ SET(bit_t::b0, REG::E),
	/* C4 */ SET(bit_t::b0, REG::H),
	/* C5 */ SET(bit_t::b0, REG::L),
	/* C6 */ SET(bit_t::b0),
	/* C7 */ SET(bit_t::b0, REG::A),
	/* C8 */ SET(bit_t::b1, REG::B),
	/* C9 */ SET(bit_t::b1, REG::C),
	/* CA */ SET(bit_t::b1, REG::D),
	/* CB */ SET(bit_t::b1, REG::E),
	/* CC */ SET(bit_t::b1, REG::H),
	/* CD */ SET(bit_t::b1, REG::L),
	/* CE */ SET(bit_t::b1),
	/* CF */ SET(bit_t::b1, REG::A),
	/* D0 */ SET(bit_t::b2, REG::B),
	/* D1 */ SET(bit_t::b2, REG::C),
	/* D2 */ SET(bit_t::b2, REG::D),
	/* D3 */ SET(bit_t::b2, REG::E),
	/* D4 */ SET(bit_t::b2, REG::H),
	/* D5 */ SET(bit_t::b2, REG::L),
	/* D6 */ SET(bit_t::b2),
	/* D7 */ SET(bit_t::b2, REG::A),
	/* D8 */ SET(bit_t::b3, REG::B),
	/* D9 */ SET(bit_t::b3, REG::C),
	/* DA */ SET(bit_t::b3, REG::D),
	/* DB */ SET(bit_t::b3, REG::E),
	/* DC */ SET(bit_t::b3, REG::H),
	/* DD */ SET(bit_t::b3, REG::L),
	/* DE */ SET(bit_t::b3),
	/* DF */ SET(bit_t::b3, REG::A),
	/* E0 */ SET(bit_t::b4, REG::B),
	/* E1 */ SET(bit_t::b4, REG::C),
	/* E2 */ SET(bit_t::b4, REG::D),
	/* E3 */ SET(bit_t::b4, REG::E),
	/* E4 */ SET(bit_t::b4, REG::H),
	/* E5 */ SET(bit_t::b4, REG::L),
	/* E6 */ SET(bit_t::b4),
	/* E7 */ SET(bit_t::b4, REG::A),
	/* E8 */ SET(bit_t::b5, REG::B),
	/* E9 */ SET(bit_t::b5, REG::C),
	/* EA */ SET(bit_t::b5, REG::D),
	/* EB */ SET(bit_t::b5, REG::E),
	/* EC */ SET(bit_t::b5, REG::H),
	/* ED */ SET(bit_t::b5, REG::L),
	/* EE */ SET(bit_t::b5),
	/* EF */ SET(bit_t::b5, REG::A),
	/* F0 */ SET(bit_t::b6, REG::B),
	/* F1 */ SET(bit_t::b6, REG::C),
	/* F2 */ SET(bit_t::b6, REG::D),
	/* F3 */ SET(bit_t::b6, REG::E),
	/* F4 */ SET(bit_t::b6, REG::H),
	/* F5 */ SET(bit_t::b6, REG::L),
	/* F6 */ SET(bit_t::b6),
	/* F7 */ SET(bit_t::b6, REG::A),
	/* F8 */ SET(bit_t::b7, REG::B),
	/* F9 */ SET(bit_t::b7, REG::C),
	/* FA */ SET(bit_t::b7, REG::D),
	/* FB */ SET(bit_t::b7, REG::E),
	/* FC */ SET(bit_t::b7, REG::H),
	/* FD */ SET(bit_t::b7, REG::L),
	/* FE */ SET(bit_t::b7),
	/* FF */ SET(bit_t::b7, REG::A),
};

#define INSERT_OPS(base_ops, idx, ops, len) for(BYTE i = 0; i < (len); i++) (base_ops)[(idx)++] = &(ops[i])

struct EXOPS
{
	const ExtendedOpcode* ops[256];

	EXOPS()
	{
		BYTE idx = 0;
		INSERT_OPS(ops, idx, RLC_OPS, 8);
		INSERT_OPS(ops, idx, RRC_OPS, 8);
		INSERT_OPS(ops, idx, RL_OPS, 8);
		INSERT_OPS(ops, idx, RR_OPS, 8);
		INSERT_OPS(ops, idx, SLA_OPS, 8);
		INSERT_OPS(ops, idx, SRA_OPS, 8);
		INSERT_OPS(ops, idx, SWAP_OPS, 8);
		INSERT_OPS(ops, idx, SRL_OPS, 8);
		INSERT_OPS(ops, idx, BIT_OPS, 64);
		INSERT_OPS(ops, idx, RES_OPS, 64);
		INSERT_OPS(ops, idx, SET_OPS, 64);
	}
};
const EXOPS EX_OPS;




void OpCB::cb_op(MMU* const& mmu, CPU* const& cpu, const uint8_t& opcode)
{
	const ExtendedOpcode* op = EX_OPS.ops[opcode];
	(*op)(mmu, cpu);
	cpu->ticks += op->ticks();
}
