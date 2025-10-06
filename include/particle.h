#pragma once
#include "vec2.h"
#include "Color.h"
struct Particle {
    Vec2 pos;
    Vec2 vel;
    Vec2 force; 
    float mass;
    Color color;
	float radius;
    bool fixed = false;
};
