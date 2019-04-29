#pragma once

#include <cinttypes>

#include "address.h"

struct Inputs
{
	union
	{
		union
		{
			struct
			{
				uint8_t a : 1;
				uint8_t b : 1;
				uint8_t select : 1;
				uint8_t start : 1;
				uint8_t right : 1;
				uint8_t left : 1;
				uint8_t up : 1;
				uint8_t down : 1;
			};
			uint8_t butons : 4;
			uint8_t pad : 4;
		};
		uint8_t all;
	};

	Inputs() : all(0) {}

	void reset() { all = 0; }
};

/*enum class Button : uint8_t
{

};

struct Inputs : public AddressSide
{
	union
	{
		struct
		{
			uint8_t P10 : 1;
			uint8_t P11 : 1;
			uint8_t P12 : 1;
			uint8_t P13 : 1;
			uint8_t P14 : 1;
			uint8_t P15 : 1;
			uint8_t : 2;
		};
		uint8_t P1;
	};

	Inputs();


	void reset() override;

	ByteAddressAccessor operator[] (const size_t& offset) override;
	ConstByteAddressAccessor operator[] (const size_t& offset) const override;
};*/
