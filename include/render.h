#pragma once
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <string>
#include "Cloth.h"
#include "ClothSimulator.h"

class Render {
public:
    int create(std::string windowName, int screenWidth, int screenHeight, unsigned int currentFlags);
    void render(ClothSimulator& clothSimulator, Cloth& cloth);
    void draw(const Cloth& cloth);
    void present();
    int destroy();

private:
    SDL_Window* _sdlWindow = nullptr;
    SDL_GLContext glContext;

    GLuint shaderProgram;

    GLuint particleVAO = 0;
    GLuint particleVBO = 0;

    GLuint springVAO = 0;
    GLuint springVBO = 0;

    int screenWidth = 800;
    int screenHeight = 600;
};
