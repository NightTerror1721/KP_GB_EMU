#pragma once

#include <cinttypes>

using address_t = uint16_t;
using byte_t = uint8_t;
using word_t = uint16_t;

unsigned long long operator "" _kB(unsigned long long value);
unsigned long long operator "" _MB(unsigned long long value);

class AddressSide
{
protected:
	const address_t _offset;
	const address_t _length;

public:
	AddressSide(const address_t& offset, const address_t& length);

	void writeByte(const address_t& offset, const byte_t& value);
	void writeWord(const address_t& offset, const word_t& value);

	byte_t readByte(const address_t& offset) const;
	word_t readWord(const address_t& offset) const;

	bool validAddress(const address_t& offset);

	virtual void reset() = 0;

protected:
	virtual void _writeByte(const address_t& offset, const byte_t& value) = 0;
	virtual void _writeWord(const address_t& offset, const word_t& value) = 0;

	virtual byte_t _readByte(const address_t& offset) const = 0;
	virtual word_t _readWord(const address_t& offset) const = 0;
};

class InvalidAddressSide : public AddressSide
{
public:
	InvalidAddressSide() : AddressSide(0, 0) {}

protected:
	void _writeByte(const address_t& offset, const byte_t& value) {}
	void _writeWord(const address_t& offset, const word_t& value) {}

	byte_t _readByte(const address_t& offset) const { return 0; }
	word_t _readWord(const address_t& offset) const { return 0; }

	void reset() {}
};
