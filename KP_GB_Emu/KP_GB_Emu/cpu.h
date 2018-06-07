#pragma once

#include "kernel.h"

class Kernel;

enum TItMask : BYTE
{
	VBLANK_BIT = 0x01,
	LCDC_BIT = 0x02,
	TIMER_OVERFLOW_BIT = 0x04,
	SERIAL_TRANSFER_BIT = 0x08,
	HILO_BIT = 0x010
};

enum TIJumpAddress : BYTE
{
	VBLANK_HANDLER_ADDRESS = 0x40,
	LCDC_HANDLER_ADDRESS = 0x48,
	TIMER_OVERFLOW_HANDLER_ADDRESS = 0x50,
	SERIAL_TRANSFER_HANDLER_ADDRESS = 0x58,
	HILO_HANDLER_ADDRESS = 0x60,
	W = 160,
	H = 144
};

#define BASE_CLOCK_SPEED 4194304;
#define F_Z 0x80
#define F_N 0x40
#define F_H 0x20
#define F_C 0x10

using reg8    = uint8;
using reg16   = uint16;
using cycle_t = QWORD;
using opcode_t = BYTE;

#define bireg(hi_reg, lo_reg) union { \
	reg16 hi_reg##lo_reg; \
	struct { \
		reg8 lo_reg; \
		reg8 hi_reg; \
	}; \
}

class CPU
{
private:
	const Kernel* core;

	FLAG interruptsEnabled;
	FLAG halted;
	FLAG emulateSpeed;
	FLAG doubleSpeed;

public:
	bireg(B, C);
	bireg(D, E);
	bireg(H, L);
	bireg(A, F);
	reg16 PC;
	reg16 SP;

	FLAG IME;
	FLAG I;
	FLAG R;

	cycle_t divCycle;
	cycle_t timerCycle;
	cycle_t cycle;
	cycle_t cyclesSinceLastSleep;
	cycle_t cyclesExecutedThisSecond;

	DWORD clockSpeed;

public:
	CPU(const Kernel* core);
	~CPU();

	void tick(cycle_t cycles);
	void execute();

	__forceinline FLAG isInterruptEnabled() const { return interruptsEnabled; }
	__forceinline FLAG isHalted() const { return halted; }
	__forceinline FLAG isSpeedEmulated() const { return emulateSpeed; }
	__forceinline FLAG isDoubleSpeedEnabled() const { return doubleSpeed; }

	__forceinline void setInterruptEnabled(FLAG flag) { interruptsEnabled = flag; }
	__forceinline void setHalted(FLAG flag) { halted = flag; }
	__forceinline void setSpeedEmulated(FLAG flag) { emulateSpeed = flag; }
	__forceinline void setDoubleSpeedEnabled(FLAG flag) { doubleSpeed = flag; }

private:
	FLAG getConditionalFlag(BYTE which);

	void setRegister(BYTE reg, reg8 value);
	reg8 getRegister(BYTE reg);

	void fireInterrupts();
	void updateInterrupts(cycle_t delta);

	void setByte(reg16 addr, BYTE value);
	BYTE getByte(reg16 addr);

	void setIO(reg16 addr, BYTE value);
	BYTE getIO(reg16 addr);

	void setInterruptTriggered(TItMask mask);
	FLAG isInterruptTriggered(TItMask mask) const;

	__forceinline BYTE nextByte() { return getByte(PC++); }
	__forceinline void pushWord(WORD word) { SP -= 2; setByte(SP, word & 0x00FF); setByte(SP + 1, (word & 0xFF00) >> 8); }

	reg16 getRegisterPair(BYTE value);

	/* OPCODES */
	void ADC_n();
	void ADC_r(opcode_t op);

	void ADD(BYTE n);
	void ADD_HL_rr(opcode_t op);
	cycle_t ADD_SP_n();
	void ADD_n();
	void ADD_r(opcode_t op);

	void AND_n();
	void AND_r(opcode_t op);

	cycle_t CALL_cc_nn(opcode_t op);
	cycle_t CALL_nn();

	void CBPrefix();

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
};

#undef bireg
