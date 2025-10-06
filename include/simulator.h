#pragma once

#include "render.h"
#include "renderSDL.h"

#include "clothSimulator.h"
#include "handleInputs.h"

const int SCREEN_WIDTH = 1500;
const int SCREEEN_HEIGHT = 800;

class Simulator
{
public:
	Simulator();
	~Simulator();

	void run();
	void loop();
	void init();
	
	ClothSimulator clothSimulator;
	Cloth cloth;
	Render render;
	RenderSDL renderSDL;

	HandleInputs inputHandler;
	SDL_Event e;
	 

private:
	float m_dt = 0.020f; 

};
