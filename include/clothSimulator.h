#pragma once

#include "cloth.h"



class ClothSimulator
{

public:
	ClothSimulator();
	~ClothSimulator();
	void applySpringForces(Cloth& c);
	void integrate(Cloth& c, float dt);
	void applyRepulsionForces(Cloth& c);
	void satisfyConstraints(Cloth& c);




	Cloth makeCloth(int rows, int cols, float spacing, float yPos);
	void addInvertedPyramid(Cloth& c, int rows, int baseCols, float spacing, float xPos, float yPos);

	void addParticleAt(Cloth& c, float x, float y, bool isFixed);
	void applyRuntimeParameters(Cloth& cloth);
	void applyGravity(Cloth& c);
	void removeOffscreenParticles(Cloth& cloth, int screenWidth, int screenHeight);

	float m_particle_radius = 1.0f;
	float m_spring_k = 200.0f; // spring stiffness
	float m_particle_mass = 0.25f;
	Vec2 gravityStrength = { 0.0f, 350.0f }; // gravity strength

private:
	 

	Color m_particle_color = Color(255, 255, 255);

	Color m_nail_color = Color(255, 0, 0);
	float m_nail_radius = 5.0f;

	float m_spring_restLen = 15.0f; // rest length
	float m_repulsion_k = 100.0f; // repulsion strength
	float m_repulsion_minDist = 1 * m_particle_radius; // minimum distance for repulsion
	float m_spring_max_len = 50.0f; // dist at which spring breaks
};

