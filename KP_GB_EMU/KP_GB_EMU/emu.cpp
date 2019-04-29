#include "emu.h"

#include <SFML/OpenGL.hpp>

GBEmulator::GBEmulator() :
	_mmu(),
	_cpu(),
	_gpu(),
	_inputs(),
	_window()
{
	_mmu.setGPU(&_gpu);
	_mmu.setInterrupts(&_cpu.ints);
}

GBEmulator::~GBEmulator()
{
	if (_window.isOpen())
		_window.close();
	glDisable(GL_TEXTURE_2D);
}

void GBEmulator::start(sf::VideoMode videoMode, int wstyle)
{
	if (!_window.isOpen())
	{
		sf::ContextSettings setts;
		setts.depthBits = 24;
		setts.stencilBits = 8;
		setts.antialiasingLevel = 0;
		setts.majorVersion = 4;
		setts.minorVersion = 0;

		_window.create(videoMode, "KTEmu", wstyle, setts);
		glEnable(GL_TEXTURE_2D);

		_window.setVerticalSyncEnabled(true);
		_window.setActive(true);


		mainLoop();
	}
}

void GBEmulator::runStep()
{
	_cpu.step(&_mmu);
}

void GBEmulator::reset()
{
	_mmu.reset();
	_cpu.reset();
	_inputs.reset();
}

void GBEmulator::mainLoop()
{
	for (;;)
	{
		if (_window.isOpen())
		{
			sf::Event event;
			while (_window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					_window.close();
					goto end_for;
				}
			}

			//_window.clear(sf::Color::Black);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			_window.display();
		}
	}
end_for:
	return;
}
