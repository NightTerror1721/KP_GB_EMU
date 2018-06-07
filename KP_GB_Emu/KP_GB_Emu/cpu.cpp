#include "cpu.h"

#define __mem core->mem
#define __cmem const_cast<Memory*>(__mem)

CPU::CPU(const Kernel* core) : core(core) {

}

CPU::~CPU() {}


void CPU::setByte(reg16 addr, BYTE value) { tick(4); const_cast<Memory*>(core->mem)->setValue(addr, value); }
BYTE CPU::getByte(reg16 addr) { tick(4); return core->mem->getValue(addr); }

void CPU::setIO(reg16 addr, BYTE value) { tick(4); const_cast<Memory*>(core->mem)->setIO(addr, value); }
BYTE CPU::getIO(reg16 addr) { tick(4); return core->mem->getIO(addr); }

void CPU::setInterruptTriggered(TItMask mask) { const_cast<Memory*>(core->mem)->registers[R_TRIGGERED_INTERRUPTS] |= mask; }
FLAG CPU::isInterruptTriggered(TItMask mask) const { return (core->mem->registers[R_TRIGGERED_INTERRUPTS] & core->mem->registers[R_ENABLED_INTERRUPTS] & mask); }


FLAG CPU::getConditionalFlag(BYTE which)
{
	switch (which & 0x7)
	{
		case 0b100: return !(F & F_Z);
		case 0b101: return (F & F_Z);
		case 0b110: return !(F & F_C);
		case 0b111: return (F & F_C);
	}
	return false;
}

reg8 CPU::getRegister(BYTE reg)
{
	switch (reg & 0b111)
	{
		case 0b111: return A;
		case 0b000: return B;
		case 0b001: return C;
		case 0b010: return D;
		case 0b011: return E;
		case 0b100: return H;
		case 0b101: return L;
		case 0b110: return getByte((H << 8) | L);
	}
	return 0;
}

void CPU::setRegister(BYTE reg, reg8 value)
{
	switch (reg & 0b111)
	{
		case 0b111: A = value; break;
		case 0b000: B = value; break;
		case 0b001: C = value; break;
		case 0b010: D = value; break;
		case 0b011: E = value; break;
		case 0b100: H = value; break;
		case 0b101: L = value; break;
		case 0b110: setByte((H << 8) | L, value); break;
	}
}

void CPU::fireInterrupts()
{
	if (!isInterruptEnabled())
		return;

	BYTE triggeredInterrupts = __mem->registers[R_TRIGGERED_INTERRUPTS];
	BYTE enabledInterrupts = __mem->registers[R_ENABLED_INTERRUPTS];

	if ((triggeredInterrupts & enabledInterrupts) != 0)
	{
		pushWord(PC);
		setInterruptEnabled(false);

		if (isInterruptTriggered(VBLANK_BIT))
		{
			PC = VBLANK_HANDLER_ADDRESS;
			triggeredInterrupts &= ~VBLANK_BIT;
		}
		else if (isInterruptTriggered(LCDC_BIT))
		{
			PC = LCDC_HANDLER_ADDRESS;
			triggeredInterrupts &= ~LCDC_BIT;
		}
		else if (isInterruptTriggered(TIMER_OVERFLOW_BIT))
		{
			PC = TIMER_OVERFLOW_HANDLER_ADDRESS;
			triggeredInterrupts &= ~TIMER_OVERFLOW_BIT;
		}
		else if (isInterruptTriggered(SERIAL_TRANSFER_BIT))
		{
			PC = SERIAL_TRANSFER_HANDLER_ADDRESS;
			triggeredInterrupts &= ~SERIAL_TRANSFER_BIT;
		}
		else if (isInterruptTriggered(HILO_BIT))
		{
			PC = HILO_HANDLER_ADDRESS;
			triggeredInterrupts &= ~HILO_BIT;
		}
		__cmem->registers[R_TRIGGERED_INTERRUPTS] = triggeredInterrupts;
	}
}

void CPU::updateInterrupts(cycle_t delta)
{
	if (isDoubleSpeedEnabled())
		delta /= 2;

	divCycle += delta;
	if (divCycle >= 256)
	{
		divCycle -= 256;
		__cmem->registers[R_DIV]++;
	}

	BYTE tac = __mem->registers[R_TAC];
	if ((tac & 0b100))
	{
		timerCycle += delta;

		cycle_t timerPeriod = 0;
		switch (tac & 0b11)
		{
			case 0b00: timerPeriod = clockSpeed / 4096;
			case 0b01: timerPeriod = clockSpeed / 262144;
			case 0b10: timerPeriod = clockSpeed / 65536;
			case 0b11: timerPeriod = clockSpeed / 16384;
		}

		while (timerCycle >= timerPeriod)
		{
			timerCycle -= timerPeriod;

			WORD tima = (__mem->registers[R_TIMA] & 0xff) + 1;
			if (tima > 0xff)
			{
				tima = __mem->registers[R_TIMA] & 0xff;
				setInterruptTriggered(TIMER_OVERFLOW_BIT);
			}
			__cmem->registers[R_TIMA] = static_cast<BYTE>(tima);
		}
	}

	/* TODO */
	//core->sound->tick();
	//core->lcd->tick();
}

reg16 CPU::getRegisterPair(BYTE value)
{
	switch (value)
	{
		case 0: return BC;
		case 1: return DE;
		case 2: return HL;
		case 3: return SP;
	}
	return 0xff;
}

void CPU::tick(cycle_t delta)
{
	cycle += delta;
	cyclesSinceLastSleep += delta;
	cyclesExecutedThisSecond += delta;

	updateInterrupts(delta);
}

void CPU::execute()
{

}


/* OPCODE Functions */
void CPU::ADC_n()
{
	int val = nextByte();
	int carry = ((F & F_C) != 0 ? 1 : 0);
	int n = val + carry;

	F = 0;
	if ((((A & 0xf) + (val & 0xf)) + carry & 0xF0) != 0)
		F |= F_H;
	A += n;
	if (A > 0xFF)
	{
		F |= F_C;
		A &= 0xFF;
	}
	if (A == 0)
		F |= F_Z;
}

void CPU::ADC_r(opcode_t op)
{
	int carry = ((F & F_C) != 0 ? 1 : 0);
	int reg = (getRegister(op & 0b111) & 0xff);

	int d = carry + reg;
	F = 0;
	if ((((A & 0xf) + (reg & 0xf) + carry) & 0xF0) != 0) F |= F_H;

	A += d;
	if (A > 0xFF)
	{
		F |= F_C;
		A &= 0xFF;
	}
	if (A == 0) F |= F_Z;
}

void CPU::ADD(BYTE n)
{
	F = 0;
	if ((((A & 0xf) + (n & 0xf)) & 0xF0) != 0) F |= F_H;
	A += n;
	if (A > 0xFF)
	{
		F |= F_C;
		A &= 0xFF;
	}
	if (A == 0) F |= F_Z;
}

void CPU::ADD_HL_rr(opcode_t op)
{
	int ss = getRegisterPair((op >> 4) & 0x3);
	int hl = HL;

	F &= F_Z;

	if (((hl & 0xFFF) + (ss & 0xFFF)) > 0xFFF)
	{
		F |= F_H;
	}

	hl += ss;

	if (hl > 0xFFFF)
	{
		F |= F_C;
		hl &= 0xFFFF;
	}

	HL = hl;
}

cycle_t CPU::ADD_SP_n()
{
	int offset = nextByte();
	int nsp = (SP + offset);

	F = 0;
	int carry = nsp ^ SP ^ offset;
	if ((carry & 0x100) != 0) F |= F_C;
	if ((carry & 0x10) != 0) F |= F_H;

	nsp &= 0xffff;

	SP = nsp;
	return 4;
}

void CPU::ADD_n()
{
	ADD(nextByte());
}

void CPU::ADD_r(opcode_t op)
{
	ADD(getRegister(op & 0b111) & 0xff);
}

void CPU::AND_n()
{
	A &= nextByte();
	F = F_H;
	if (A == 0) F |= F_Z;
}

void CPU::AND_r(opcode_t op)
{
	A = (A & getRegister(op & 0b111)) & 0xff;
	F = F_H;
	if (A == 0) F |= F_Z;
}

cycle_t CPU::CALL_cc_nn(opcode_t op)
{
	int jmp = (nextByte()) | (nextByte() << 8);
	if (getConditionalFlag(0b100 | ((op >> 3) & 0x7)))
	{
		pushWord(PC);
		PC = jmp;
		return 4;
	}
	return 0;
}

cycle_t CPU::CALL_nn()
{
	int jmp = (nextByte()) | (nextByte() << 8);
	pushWord(PC);
	PC = jmp;
	return 4;
}

void CPU::CBPrefix()
{
	int x = PC++;

	int cbop = getByte(x);
	int r = cbop & 0x7;
	int d = getRegister(r) & 0xff;

	switch ((cbop & 0b11000000))
	{
		case 0x80:
		{
			// RES b, r
			// 1 0 b b b r r r
			setRegister(r, d & ~(0x1 << (cbop >> 3 & 0x7)));
			return;
		}
		case 0xc0:
		{
			// SET b, r
			// 1 1 b b b r r r
			setRegister(r, d | (0x1 << (cbop >> 3 & 0x7)));
			return;
		}
		case 0x40:
		{
			// BIT b, r
			// 0 1 b b b r r r
			F &= F_C;
			F |= F_H;
			if ((d & (0x1 << (cbop >> 3 & 0x7))) == 0) F |= F_Z;
			return;
		}
		case 0x0:
		{
			switch (cbop & 0xf8)
			{
				case 0x00: // RLC m
				{
					F = 0;
					if ((d & 0x80) != 0) F |= F_C;
					d <<= 1;

					// we're shifting circular left, add back bit 7
					if ((F & F_C) != 0) d |= 0x01;
					d &= 0xff;
					if (d == 0) F |= F_Z;
					setRegister(r, d);
					return;
				}
				case 0x08: // RRC m
				{
					F = 0;
					if ((d & 0b1) != 0) F |= F_C;
					d >>= 1;

					// we're shifting circular right, add back bit 7
					if ((F & F_C) != 0) d |= 0x80;
					d &= 0xff;
					if (d == 0) F |= F_Z;
					setRegister(r, d);
					return;
				}
				case 0x10: // RL m
				{
					bool carryflag = (F & F_C) != 0;
					F = 0;

					// we'll be shifting left, so if bit 7 is set we set carry
					if ((d & 0x80) == 0x80) F |= F_C;
					d <<= 1;
					d &= 0xff;

					// move old C into bit 0
					if (carryflag) d |= 0b1;
					if (d == 0) F |= F_Z;
					setRegister(r, d);
					return;
				}
				case 0x18: // RR m
				{
					bool carryflag = (F & F_C) != 0;
					F = 0;

					// we'll be shifting right, so if bit 1 is set we set carry
					if ((d & 0x1) == 0x1) F |= F_C;
					d >>= 1;

					// move old C into bit 7
					if (carryflag) d |= 0b10000000;
					if (d == 0) F |= F_Z;
					setRegister(r, d);
					return;
				}
				case 0x38: // SRL m
				{
					F = 0;

					// we'll be shifting right, so if bit 1 is set we set carry
					if ((d & 0x1) != 0) F |= F_C;
					d >>= 1;
					if (d == 0) F |= F_Z;
					setRegister(r, d);
					return;
				}
				case 0x20: // SLA m
				{
					F = 0;

					// we'll be shifting right, so if bit 1 is set we set carry
					if ((d & 0x80) != 0) F |= F_C;
					d <<= 1;
					d &= 0xff;
					if (d == 0) F |= F_Z;
					setRegister(r, d);
					return;
				}
				case 0x28: // SRA m
				{
					bool bit7 = (d & 0x80) != 0;
					F = 0;
					if ((d & 0b1) != 0) F |= F_C;
					d >>= 1;
					if (bit7) d |= 0x80;
					if (d == 0) F |= F_Z;
					setRegister(r, d);
					return;
				}
				case 0x30: // SWAP m
				{
					d = ((d & 0xF0) >> 4) | ((d & 0x0F) << 4);
					F = d == 0 ? F_Z : 0;
					setRegister(r, d);
					return;
				}
				default:
					core->cpush_error("cb-&f8-" + cbop);
					return;
			}
		}
		default:
			core->cpush_error("cb-" + cbop);
			return;
	}
}

cycle_t CCD();

void CP(BYTE n);
void CP_n();

void DAA();

void DEC_r(opcode_t op);
void DEC_rr(opcode_t op);

void DI();
cycle_t EI();

cycle_t HALT();

void INC_r(opcode_t op);
void INC_rr(opcode_t op);

void JP_HL();
cycle_t JP_c_nn(opcode_t op);
cycle_t JP_nn();

cycle_t JR_nn(opcode_t op);
cycle_t JR_e();

cycle_t LDHL_SP_n();

cycle_t LDH_FFC_A();
void LDH_FFnn();

cycle_t LD_A_BC();
cycle_t LD_A_C();
cycle_t LD_A_DE();
cycle_t LD_A_HLI();
cycle_t LD_A_n();
cycle_t LD_A_nn();
cycle_t LD_BC_A();
cycle_t LD_DE_A();
cycle_t LD_FFn_A();
cycle_t LD_HLD_A();
cycle_t LD_HLI_A();
cycle_t LD_a16_SP();
cycle_t LD_dd_nn(opcode_t op);
cycle_t LD_nn_A();
cycle_t LD_r_n(opcode_t op);
void LD_r_r(opcode_t op);

cycle_t NOP();

void OP(BYTE n);
void OR_n();
void OR_r(opcode_t op);

cycle_t POP_rr(opcode_t op);
cycle_t PUSH_rr(opcode_t op);
cycle_t RET();
cycle_t RETI();
cycle_t RET_c(opcode_t op);

void RLA();
void RLCA();
void RRA();
void RRCA();

cycle_t RST_p(opcode_t op);

void SBC_n();
void SBC_r(opcode_t op);

cycle_t SCF();

cycle_t STOP();

void SUB(BYTE n);
void SUB_n();
void SUB_r(opcode_t op);

void XOR_n();
void XOR_r(opcode_t op);
