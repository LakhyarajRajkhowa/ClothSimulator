#include "handleInputs.h"


void HandleInputs::handleMouseResponse(SDL_Event& e, Cloth& cloth, int &selectedParticle, bool& dragging, bool& rightMouseDown) {
	switch (e.type) {

	case SDL_MOUSEBUTTONDOWN:
		switch (e.button.button) {
		case SDL_BUTTON_LEFT: {
			float mx = (float)e.button.x;
			float my = (float)e.button.y;
			// find nearest particle
			float bestDist = 1e9f;
			for (int i = 0; i < cloth.particles.size(); i++) {
				float dx = cloth.particles[i].pos.x - mx;
				float dy = cloth.particles[i].pos.y - my;
				float d2 = dx * dx + dy * dy;
				if (d2 < bestDist) {
					bestDist = d2;
					selectedParticle = i;
				}
			}
			// select if close enough
			if (bestDist < 100.0f) {
				dragging = true;
			}
			else {
				selectedParticle = -1;
			}
		}
							break;
		case SDL_BUTTON_RIGHT:
			rightMouseDown = true;
			break;
		}
		break;
	case SDL_MOUSEBUTTONUP:
		switch (e.button.button) {
		case SDL_BUTTON_LEFT:
			dragging = false;
			selectedParticle = -1;
			break;
		case SDL_BUTTON_RIGHT:
			rightMouseDown = false;
			break;
		}
		break;

	}
}

void HandleInputs::handleKeyboardResponse(SDL_Event& e, bool& running) {
	switch (e.type) {
	case SDL_QUIT:
		running = false;
		break;
	case SDL_KEYDOWN:
		if (e.key.keysym.sym == SDLK_ESCAPE)
			running = false;
		break;
	}
}

void HandleInputs::handleMouseDraggingLeft(Cloth& cloth, int& selectedParticle, bool& dragging) {
	if (dragging && selectedParticle != -1) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);
		if (selectedParticle < 0 || selectedParticle >= (int)cloth.particles.size()) {
			dragging = false;
			selectedParticle = -1;
			return;
		}

		Particle& p = cloth.particles[selectedParticle];

		Vec2 mousePos = { (float)mx, (float)my };
		Vec2 delta = mousePos - p.pos;

		float k = 4000.0f;      // spring stiffness (tweak)
		float damping = 10.0f; // damping factor (tweak)

		// spring force towards mouse
		Vec2 Fspring = delta * k;

		// damping force opposite to velocity
		Vec2 Fdamp = p.vel * -damping;

		// apply combined force
		p.force = p.force + Fspring + Fdamp;
	}
}

void HandleInputs::handleMouseDraggingRight(Cloth& cloth, bool& rightMouseDown) {
	if (rightMouseDown) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);

		
		// Iterate through all springs
		for (auto it = cloth.springs.begin(); it != cloth.springs.end(); ) {
			const auto& s = *it;
			const Particle& A = cloth.particles[s.a];
			const Particle& B = cloth.particles[s.b];

			// Midpoint of the spring
			float midX = (A.pos.x + B.pos.x) * 0.5f;
			float midY = (A.pos.y + B.pos.y) * 0.5f;

			float dx = mx - midX;
			float dy = my - midY;
			float dist2 = dx * dx + dy * dy;

			if (dist2 < 100.0f) { // within 10 pixels radius
				it = cloth.springs.erase(it); // break bond
			}
			else {
				++it;
			}
		}
		// Limit spawn rate (so it doesn’t spawn every frame)
		//static Uint32 lastSpawnTime = 0;
		//Uint32 now = SDL_GetTicks();
		//if (now - lastSpawnTime < 100) return; // 100 ms delay between spawns
		//lastSpawnTime = now;

		//// Column settings
		//int columnCount = 10; // number of particles in column
		//float spacing = 15.0f;

		//for (int i = 0; i < columnCount; i++) {
		//	float px = (float)mx;
		//	float py = (float)my + i * spacing;

		//	clothSimulator.addParticleAt(cloth, px, py, false);
		//}
	}
}


void HandleInputs::handleInputs(Cloth& cloth, bool& running, int& selectedParticle, bool& dragging, bool& rightMouseDown) {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		ImGui_ImplSDL2_ProcessEvent(&e);
		handleKeyboardResponse(e, running);
		handleMouseResponse(e, cloth, selectedParticle, dragging, rightMouseDown);		
	}
	
	handleMouseDraggingLeft(cloth, selectedParticle, dragging);
	handleMouseDraggingRight(cloth, rightMouseDown);
	
}
