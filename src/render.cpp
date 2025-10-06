#include "Render.h"
#include "Errors.h"
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <imgui/backends/imgui_impl_sdl2.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#define MAX_PARTICLES 10000   // Adjust to maximum number of particles your cloth simulator can have
#define MAX_SPRINGS   20000   // Adjust to maximum number of springs

// Helper function to compile shader
GLuint compileShader(const std::string& vertPath, const std::string& fragPath) {
    auto readFile = [](const std::string& path) {
        std::ifstream file(path);
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
        };

    std::string vertCode = readFile(vertPath);
    std::string fragCode = readFile(fragPath);
    const char* vShaderCode = vertCode.c_str();
    const char* fShaderCode = fragCode.c_str();

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);

    GLint success;
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) std::cerr << "Vertex Shader compilation failed!\n";

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) std::cerr << "Fragment Shader compilation failed!\n";

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) std::cerr << "Shader program linking failed!\n";

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}

int Render::create(std::string windowName, int screenWidth, int screenHeight, unsigned int currentFlags) {
    Uint32 flags = SDL_WINDOW_OPENGL ;

    _sdlWindow = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        screenWidth, screenHeight, 2);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    glContext = SDL_GL_CreateContext(_sdlWindow);
    SDL_GL_SetSwapInterval(1);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ImGui setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(_sdlWindow, glContext);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Shader setup
    shaderProgram = compileShader("C:/Users/llakh/OneDrive/Desktop/firework particle simulator/LifeSimulator/cloth_simulator/shaders/particle.vert", "C:/Users/llakh/OneDrive/Desktop/firework particle simulator/LifeSimulator/cloth_simulator/shaders/particle.frag");

    // Particle VAO/VBO
    glGenVertexArrays(1, &particleVAO);
    glGenBuffers(1, &particleVBO);

    glBindVertexArray(particleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 5 * MAX_PARTICLES, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // Spring VAO/VBO
    glGenVertexArrays(1, &springVAO);
    glGenBuffers(1, &springVBO);
    glBindVertexArray(springVAO);
    glBindBuffer(GL_ARRAY_BUFFER, springVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * MAX_SPRINGS, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;

    return 0;
}

void Render::render(ClothSimulator& clothSimulator, Cloth& cloth) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    bool updated = false;

    ImGui::Begin("Cloth Parameters");

    if (ImGui::SliderFloat("Bond Strength", &clothSimulator.m_spring_k, 10.0f, 500.0f)) updated = true;
    if (ImGui::SliderFloat("Particle Mass", &clothSimulator.m_particle_mass, 0.1f, 10.0f)) updated = true;
    if (ImGui::SliderFloat("Gravity", &clothSimulator.gravityStrength.y, 0.0f, 1000.0f)) updated = true;

    ImGui::End();

    if (updated) clothSimulator.applyRuntimeParameters(cloth);

    // FPS Display
    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::SetNextWindowBgAlpha(0.35f);
    ImGui::Begin("FPS", nullptr,
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav);
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::End();

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Render::draw(const Cloth& cloth) {
    glUseProgram(shaderProgram);

    // Set orthographic projection
    glm::mat4 projection = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uProjection"), 1, GL_FALSE, &projection[0][0]);

    // -------------------
    // Draw springs
    // -------------------
    std::vector<float> springData;
    springData.reserve(cloth.springs.size() * 4);
    for (auto& s : cloth.springs) {
        const auto& A = cloth.particles[s.a];
        const auto& B = cloth.particles[s.b];
        springData.push_back(A.pos.x);
        springData.push_back(A.pos.y);
        springData.push_back(B.pos.x);
        springData.push_back(B.pos.y);
    }

    glBindVertexArray(springVAO);
    glBindBuffer(GL_ARRAY_BUFFER, springVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, springData.size() * sizeof(float), springData.data());

    glUniform3f(glGetUniformLocation(shaderProgram, "uColor"), 0.5f, 0.5f, 0.5f); // gray
    glLineWidth(1.0f);
    glDrawArrays(GL_LINES, 0, cloth.springs.size() * 2);
    glBindVertexArray(0);

    // -------------------
    // Draw particles
    // -------------------
    std::vector<float> particleData;
    particleData.reserve(cloth.particles.size() * 5);
    for (auto& p : cloth.particles) {
        particleData.push_back(p.pos.x);
        particleData.push_back(p.pos.y);
        particleData.push_back(p.color.r / 255.0f);
        particleData.push_back(p.color.g / 255.0f);
        particleData.push_back(p.color.b / 255.0f);
    }

    glBindVertexArray(particleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particleData.size() * sizeof(float), particleData.data());

    glUniform3f(glGetUniformLocation(shaderProgram, "uColor"), 1.0f, 1.0f, 1.0f); // white
    glPointSize(1.0f); // increase size of particles
    glDrawArrays(GL_POINTS, 0, cloth.particles.size());
    glBindVertexArray(0);
}

void Render::present() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(_sdlWindow);
}

int Render::destroy() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    glDeleteBuffers(1, &particleVBO);
    glDeleteVertexArrays(1, &particleVAO);
    glDeleteBuffers(1, &springVBO);
    glDeleteVertexArrays(1, &springVAO);
    glDeleteProgram(shaderProgram);

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(_sdlWindow);
    SDL_Quit();
    return 0;
}
