#pragma once

#include <cinttypes>

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
};
