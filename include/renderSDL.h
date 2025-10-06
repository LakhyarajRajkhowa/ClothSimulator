
#pragma once
#include <SDL2/SDL.h>
#include <string>

#include "Color.h"
#include "clothSimulator.h"

enum  windowFlags { INVISIBLE = 0x1, FULLSCREEN = 0x2, BORDERLESS = 0x4 };

class RenderSDL
{
public:
	void  SDL_RenderFillCircle(int centerX, int centerY, int radius);
	int create(std::string windowName, int screenWidth, int screenHeight, unsigned int currentFlags);
	void render(ClothSimulator& clothSimulator, Cloth& cloth);
	void present();
	void renderParticle(int x, int y, float radius, Color color);
	int destroy();
	void draw(const Cloth& c);



private:
	SDL_Window* _sdlWindow;
	SDL_Renderer* renderer;
};