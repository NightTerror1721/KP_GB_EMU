#include "cartridge.h"

#include <ctgmath>



Cartridge::Cartridge(RAW_DATA& raw) : rawRom(raw), _invalidCartridge(false)
{
	this->_isColorGB = raw[0x0143];

	std::string title(&raw[0x0134], (_isColorGB ? 11 : 16));
	size_t stop = title.find('\0');
	this->gameTitle = (stop != std::string::npos ? title.substr(0, stop) : title);

	this->_isSuperGB = raw[0x0146] == 0x03;

	this->cartridgeType = static_cast<CartridgeType>(raw[0x0147]);

	BYTE romSize = raw[0x0148];
	switch(romSize)
	{
		case 52: this->romBanks = 72; break;
		case 53: this->romBanks = 80; break;
		case 54: this->romBanks = 96; break;
		default: this->romBanks = std::pow(2, romSize + 1);
	}
	if(raw.length != 0x2000 * romBanks)
		_invalidCartridge = true;

	switch(raw[0x0149])
	{
		case 0: this->ramBanks = 0; break;
		case 1:
		case 2:
			this->ramBanks = 1; break;
		case 3: this->ramBanks = 4;  break;
		case 4: this->ramBanks = 16; break;
		default:
			this->ramBanks = 0;
			_invalidCartridge = true;
			break;
	}

	this->_isJapanese = !raw[0x014A];

	uint8 chk = 0;
	for(int i = 0; i < 16; i++)
		chk += raw[0x0134 + i];
	this->checksum = chk;
}


Cartridge::~Cartridge()
{
}

bool Cartridge::hasBattery()
{
	switch(cartridgeType)
	{
		case ROM_MBC1_RAM_BATT:
		case ROM_RAM_BATT:
		case ROM_MBC5_RAM_BATT:
		case ROM_MBC5_RUMBLE_SRAM_BATT:
		case ROM_MBC3_TIMER_RAM_BATT:
		case ROM_MBC3_RAM_BATT:
		case ROM_MBC2_BATT:
			return true;
		default: return false;
	}
}
