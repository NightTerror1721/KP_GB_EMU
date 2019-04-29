#pragma once

#include "mmu.h"
#include "cpu.h"
#include "gpu.h"
#include "inputs.h"

#include <SFML/Graphics.hpp>

class GBEmulator
{
	MMU _mmu;
	CPU _cpu;
	GPU _gpu;
	Inputs _inputs;

	sf::RenderWindow _window;

public:
	GBEmulator();
	~GBEmulator();

	void start(sf::VideoMode videoMode, int wstyle = sf::Style::Default);

	void runStep();
	void reset();

private:
	void mainLoop();
};
