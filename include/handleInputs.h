#pragma once
#include "cloth.h"
#include "clothSimulator.h"

#include "window.h"

class HandleInputs
{

public:
	HandleInputs() {}
	~HandleInputs() {}
	void handleInputs(Cloth& cloth, bool& running, int& selectedParticle, bool& dragging, bool& rightMouseDown);
	void handleMouseResponse(SDL_Event& e, Cloth& cloth, int& selectedParticle, bool& dragging, bool& rightMouseDown);
	void handleKeyboardResponse(SDL_Event & e, bool& running);
	void handleMouseDraggingLeft(Cloth& cloth, int& selectedParticle, bool& dragging);
	void handleMouseDraggingRight(Cloth& cloth, bool& rightMouseDown);


	ClothSimulator clothSimulator;
private:
};