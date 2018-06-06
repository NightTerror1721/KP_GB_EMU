#pragma once

#include <string>

#include "basics.h"

enum CartridgeType : BYTE {
	ROM_ONLY					= 0x00,
	ROM_MBC1					= 0x01,
	ROM_MBC1_RAM				= 0x02,
	ROM_MBC1_RAM_BATT			= 0x03,
	//							= 0x04,
	ROM_MBC2					= 0x05,
	ROM_MBC2_BATT				= 0x06,
	//							= 0x07,
	ROM_RAM						= 0x08,
	ROM_RAM_BATT				= 0x09,
	//							= 0x0a,
	ROM_MMM01					= 0x0b,
	ROM_MMM01_SRAM				= 0x0c,
	ROM_MMM01_SRAM_BATT			= 0x0d,
	ROM_MMM01_TIMER_BATT		= 0x0e,
	ROM_MBC3_TIMER_BATT			= 0x0f,
	ROM_MBC3_TIMER_RAM_BATT		= 0x10,
	ROM_MBC3					= 0x11,
	ROM_MBC3_RAM				= 0x12,
	ROM_MBC3_RAM_BATT			= 0x13,
	//							= 0x14,
	//							= 0x15,
	//							= 0x16,
	//							= 0x17,
	//							= 0x18,
	ROM_MBC5					= 0x19,
	ROM_MBC5_RAM				= 0x1a,
	ROM_MBC5_RAM_BATT			= 0x1b,
	ROM_MBC5_RUMBLE				= 0x1c,
	ROM_MBC5_RUMBLE_SRAM		= 0x1d,
	ROM_MBC5_RUMBLE_SRAM_BATT	= 0x1e
};



class Cartridge
{
private:
	std::string gameTitle;

	FLAG _isColorGB;
	FLAG _isSuperGB;
	FLAG _isJapanese;
	FLAG _invalidCartridge;

	CartridgeType cartridgeType;

	uint16 romBanks;
	uint16 ramBanks;

	RAW_DATA rawRom;

	uint8 checksum;

public:
	Cartridge(RAW_DATA& rawRomData);
	~Cartridge();

	bool hasBattery() const;

	__forceinline std::string getGameTitle() const { return gameTitle; }

	__forceinline CartridgeType getCartridgeType() const { return cartridgeType; }

	__forceinline FLAG isColorGB() const { return _isColorGB; }
	__forceinline FLAG isSuperGB() const { return _isSuperGB; }
	__forceinline FLAG isJapanese() const { return _isJapanese; }
	__forceinline FLAG isInvalidCartridge() const { return _invalidCartridge; }

	__forceinline uint16 getRomBanks() const { return romBanks; }
	__forceinline uint16 getRamBanks() const { return ramBanks; }

	__forceinline BYTE romdata(DWORD addr) const { return (BYTE) rawRom[addr]; }


};

