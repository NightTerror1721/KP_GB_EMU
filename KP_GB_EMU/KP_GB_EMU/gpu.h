#pragma once

#include "address.h"

class CPU;
class GPU;

enum GpuControl : byte_t
{
	BG_Enable	   = 1 << 0,
	Sprite_Enable  = 1 << 1,
	Sprite_VDouble = 1 << 2,
	Tilemap		   = 1 << 3,
	Tileset		   = 1 << 4,
	Window_Enable  = 1 << 5,
	Window_Tilemap = 1 << 6,
	Display_Enable = 1 << 7
};

enum GpuMode : byte_t
{
	HBlank = 0,
	VBlank = 1,
	Oam = 2,
	Vram = 3
};

class VRAM : public RAM
{
private:
	GPU* const _gpu;

public:
	VRAM(GPU* const& gpu, const size_t& size);

	void onMmuWrite(const address_t& address, const byte_t& value) override;
	void onMmuWrite(const address_t& address, const word_t& value) override;
};

union color_t
{
	struct { byte_t r, g, b, a; };
	uint32_t rgba;
};

union sprite_t
{
	struct
	{
		byte_t y;
		byte_t x;
		byte_t tile;
		union
		{
			struct
			{
				byte_t priority : 1;
				byte_t vFlip : 1;
				byte_t hFlip : 1;
				byte_t palette : 1;
				byte_t : 4;
			};
			byte_t options;
		};
	};
private:
	uint32_t _all;

public:
	sprite_t(const uint32_t& value);

	sprite_t& operator= (const uint32_t& value);
};

class GPU
{
	byte_t _control;
	byte_t _scroll_x;
	byte_t _scroll_y;
	byte_t _scanline;
	uint64_t _tick;
	uint64_t _lastTicks;

	byte_t _gpuMode;

	byte_t _tiles[384][8][8];

	color_t _backgroundPalette[4];
	color_t _spritePalette[2][4];

	color_t _frameBuffer[160 * 144];


public:
	GPU();

	void step(CPU* const& cpu);

	void hblank();

	void drawFramebuffer();

	void updateTile(const address_t& addr, const byte_t& value);

	void renderScanline();

public:
	VRAM vram;
	RAM oam;
};



static const color_t Pallete[4] {
	{ 255, 255, 255, 255 },
	{ 192, 192, 192, 192 },
	{ 96,  96,  96,  96  },
	{ 0,   0,   0,   0   }
};
