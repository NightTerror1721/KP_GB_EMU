#pragma once
#include <fstream>

#include "kernel.h"

class Kernel;

#define MEM_BASE_SIZE 0x10000
#define ROM_PAGE_LEN 0x4000
#define VRAM_PAGE_LEN 0x2000
#define ERAM_PAGE_LEN 0x2000
#define WRAM_PAGE_LEN 0x2000
#define RAM_PAGE_LEN WRAM_PAGE_LEN

using address = WORD;

enum RegisterId : BYTE
{
	R_DIV					= 0x04,
	R_JOYPAD				= 0x00,
	R_SERIAL				= 0x02,
	R_TAC					= 0x07,
	R_TIMA					= 0x05,
	R_TMA					= 0x06,
	R_NR10					= 0x10,
	R_NR11					= 0x11,
	R_NR12					= 0x12,
	R_NR13					= 0x13,
	R_NR14					= 0x14,
	R_NR21					= 0x16,
	R_NR22					= 0x17,
	R_NR23					= 0x18,
	R_NR24					= 0x1A,
	R_NR30					= 0x1B,
	R_NR31					= 0x1C,
	R_NR32					= 0x1D,
	R_NR33					= 0x1E,
	R_NR34					= 0x1F,
	R_NR41					= 0x20,
	R_NR42					= 0x21,
	R_NR43					= 0x22,
	R_NR44					= 0x23,
	R_NR51					= 0x25,
	R_NR52					= 0x26,
	R_WRAM_BANK				= 0x70,
	R_VRAM_BANK				= 0x4F,
	R_LCDC					= 0x40,
	R_LCD_STAT				= 0x41,
	R_SCY					= 0x42,
	R_SCX					= 0x43,
	R_LY					= 0x44,
	R_LYC					= 0x45,
	R_BGP					= 0x47,
	R_OBP0					= 0x48,
	R_OBP1					= 0x49,
	R_TRIGGERED_INTERRUPTS	= 0x0F,
	R_ENABLED_INTERRUPTS	= 0xFF,
	R_DMA					= 0x46,
	R_WY					= 0x4A,
	R_WX					= 0x4B
};


class Memory
{
protected:
	const Kernel* core;

private:
	class HDMA;
	HDMA * hdma;

public:
	BYTE registers[0x0100];
	BYTE oam[0x00A0];

	BYTE* vram;
	BYTE* wram;

protected:
	DWORD vramPageStart;
	DWORD wramPageStart;
	DWORD romPageStart;

public:
	Memory(const Kernel* core);
	~Memory();

	bool hasBatery() const;

	virtual void save(std::ofstream& output) const;
	virtual void load(std::ifstream& input);

	virtual void setValue(address addr, BYTE value);
	virtual BYTE getValue(address addr) const;

	void setIO(address addr, BYTE value);
	BYTE getIO(address addr) const;

	__forceinline BYTE reg(RegisterId regId) const { return registers[regId]; }

	__forceinline FLAG hasHdma() { return hdma != __nullptr; }
	__forceinline void hdmaTick() { if(hdma) hdma->tick(); }

private:
	class HDMA
	{
	private:
		Memory* mem;

		const address source;
		const address dest;

		uint32 length;
		uint32 ptr;

		HDMA(Memory* mem, const address source, const address dest, const uint32 length);

	public:
		~HDMA() = default;

		/**
		* Ticks DMA.
		*/
		void tick();
	};
};


namespace
{
	using bankid = DWORD;

	class MBC : protected Memory
	{
	protected:
		DWORD ramPageStart;

		FLAG ramEnabled;

		BYTE* cartRam;
		const DWORD cartRamSize;

	public:
		MBC(const Kernel* core, DWORD cartRamSize);
		~MBC();

		void save(std::ofstream& output) const;
		void load(std::ifstream& input);

		BYTE getValue(address h_addr) const;
	};

	class MBC1 : MBC
	{
	private:
		FLAG modeSelect;

		bankid romBank;

		void mapRom(bankid bank);

	public:
		MBC1(const Kernel* core);
		~MBC1() = default;

		void setValue(address h_addr, BYTE value);
	};

	class MBC2 : MBC
	{
	public:
		MBC2(const Kernel* core);
		~MBC2() = default;
	};

	class MBC3 : MBC
	{
	private:
		int16 ramBank;

		FLAG rtcEnabled;

		BYTE rtc[4];

	public:
		MBC3(const Kernel* core);
		~MBC3() = default;

		void setValue(address h_addr, BYTE value);
	};

	class MBC5 : MBC
	{
	private:
		bankid romBank;

		void mapRom(bankid bank);

	public:
		MBC5(const Kernel* core);
		~MBC5() = default;

		void setValue(address h_addr, BYTE value);
	};
}

