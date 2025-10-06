#include "simulator.h"

Simulator::Simulator() {}
Simulator::~Simulator() {}

void Simulator::init() {
    cloth = clothSimulator.makeCloth(1, 80, 20.0f, 10.0f);
	clothSimulator.addInvertedPyramid(cloth, 50, 120, 10.0f, 100.0f, 20.0f);
   
}
void Simulator::run() {
    render.create("LIFE SIMULATOR", SCREEN_WIDTH, SCREEEN_HEIGHT, 2);
  //  renderSDL.create("LIFE SIMULATOR", SCREEN_WIDTH, SCREEEN_HEIGHT, 2);

    init();
    loop();
}

void Simulator::loop() {
  
    bool running = true;
    bool dragging = false;
    bool rightMouseDown = false;
    
    int selectedParticle = -1;   

    while (running) {
		inputHandler.handleInputs(cloth, running, selectedParticle, dragging, rightMouseDown);
        
       // clothSimulator.applyRuntimeParameters(cloth);
        clothSimulator.applySpringForces(cloth);
       // clothSimulator.applyRepulsionForces(cloth); // taking too much time 
		//clothSimulator.applyGravity(cloth);
        clothSimulator.integrate(cloth, m_dt);
        for (int i = 0; i < 10; i++) { // iterate multiple times for stability
            clothSimulator.satisfyConstraints(cloth);
        }
        render.render(clothSimulator, cloth);
        render.draw(cloth);
        render.present();

       /* renderSDL.render(clothSimulator, cloth);
        renderSDL.draw(cloth);
        renderSDL.present();*/

        clothSimulator.removeOffscreenParticles(cloth, SCREEN_WIDTH, SCREEEN_HEIGHT);

       
        if (ImGui::GetIO().Framerate > 60.0f) {
            SDL_Delay(16);
        }else 
			SDL_Delay(1);


    }
    render.destroy();
    //renderSDL.destroy();

}

