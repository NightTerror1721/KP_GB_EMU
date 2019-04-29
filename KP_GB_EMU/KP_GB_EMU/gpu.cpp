#include "gpu.h"
#include "cpu.h"

#include <SFML/OpenGL.hpp>


VRAM::VRAM(GPU* const& gpu, const size_t& size) :
	RAM(size),
	_gpu(gpu)
{}

void VRAM::onMmuWrite(const address_t& address, const byte_t& value)
{
	if(addr_range(0x8000, address, 0x97FF))
		_gpu->updateTile(address, value);
}

void VRAM::onMmuWrite(const address_t& address, const word_t& value)
{
	if (addr_range(0x8000, address, 0x97FF))
		_gpu->updateTile(address, static_cast<byte_t>(value & 0xFF));
}


sprite_t::sprite_t(const uint32_t& value) :
	_all(value)
{}

sprite_t& sprite_t::operator= (const uint32_t& value)
{
	_all = value;
	return *this;
}


GPU::GPU() :
	_control(0),
	_scroll_x(0),
	_scroll_y(0),
	_scanline(0),
	_tick(0),
	_lastTicks(0),
	_gpuMode(GpuMode::HBlank),
	_tiles(),
	_backgroundPalette(),
	_spritePalette(),
	_frameBuffer(),
	vram(this, 0x2000),
	oam(0xA0)
{}

void GPU::step(CPU* const& cpu)
{
	_tick += cpu->ticks - _lastTicks;
	_lastTicks = cpu->ticks;

	switch (_gpuMode)
	{
		case GpuMode::HBlank:
			if (_tick >= 204)
			{
				hblank();

				if (_scanline == 143)
				{
					if (cpu->ints.enabled_vblank)
						cpu->ints.int_vblank = ENABLED_FLAG;
					_gpuMode = GpuMode::VBlank;
				}
				else _gpuMode = GpuMode::Oam;

				_tick -= 204;
			}
			break;

		case GpuMode::VBlank:
			if (_tick >= 456)
			{
				if (++_scanline > 153)
				{
					_scanline = 0;
					_gpuMode = GpuMode::Oam;
				}
				_tick -= 456;
			}
			break;

		case GpuMode::Oam:
			if (_tick >= 80)
			{
				_gpuMode = GpuMode::Vram;
				_tick -= 80;
			}

		case GpuMode::Vram:
			if (_tick >= 172)
			{
				_gpuMode = GpuMode::HBlank;
				_tick -= 172;
			}
	}
}

void GPU::hblank() { _scanline++; }

void GPU::drawFramebuffer()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glRasterPos2f(-1.f, 1.f);
	glPixelZoom(1.f, -1.f);
	glDrawPixels(160, 144, GL_RGB, GL_UNSIGNED_BYTE, _frameBuffer);
}

void GPU::updateTile(const address_t& addr, const byte_t& value)
{
	address_t address = addr & 0x1FFE;

	word_t tile = (address >> 4) & 511;
	byte_t y = (address >> 1) & 7;
	
	for (byte_t x = 0; x < 8; x++)
	{
		byte_t bitidx = 1 << (7 - x);
		_tiles[tile][x][y] = (vram[address] & bitidx ? 1 : 0) + (vram[address + 1] & bitidx ? 2 : 0);
	}
}

void GPU::renderScanline()
{
	int mapOffset = _control & GpuControl::Tilemap ? 0x1c00 : 0x1800;
	mapOffset += (((_scanline + _scroll_y) & 255) >> 3) << 5;

	int lineOffset = _scroll_x >> 3;

	int x = _scroll_x & 7;
	int y = (_scanline + _scroll_y) & 7;

	int pixelOffset = _scanline * 160;

	word_t tile = static_cast<word_t>(vram[static_cast<address_t>(mapOffset + lineOffset)]);

	byte_t scanlineRow[160];

	//if background enabled
	for (int i = 0; i < 160; i++)
	{
		byte_t color = _tiles[tile][x][y];
		scanlineRow[i] = color;

		_frameBuffer[pixelOffset] = _backgroundPalette[color];

		if (++x == 8)
		{
			x = 0;
			lineOffset = (lineOffset + 1) & 31;
			tile = vram[mapOffset + lineOffset];
		}
	}

	//if sprites enabled
	AddressAccessor<sprite_t> s_acc = (*oam).as<sprite_t>();
	for (int i = 0; i < 40; i++)
	{
		sprite_t sprite = s_acc[i];

		int sx = sprite.x;
		int sy = sprite.y;

		if (sy < _scanline && (sy + 8) > _scanline)
		{
			color_t* pal = _spritePalette[sprite.palette];

			int pixelOffset = _scanline * 160 + sx;

			byte_t tileRow = sprite.vFlip
				? 7 - (_scanline - sy)
				: _scanline - sy;

			for (int x = 0; x < 8; x++)
			{
				if (sx + x >= 0 && sx + x < 160 && (~sprite.priority || !scanlineRow[sx + x]))
				{
					byte_t color = sprite.hFlip
						? _tiles[sprite.tile][tileRow][7 - x]
						: _tiles[sprite.tile][tileRow][x];

					if (color)
						_frameBuffer[pixelOffset] = pal[color];
					pixelOffset++;
				}
			}
				
		}
	}
}
