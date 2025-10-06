
#include "renderSDL.h"
#include <cmath>
#include "Errors.h"
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_sdl2.h>
#include <imgui/backends/imgui_impl_sdlrenderer2.h>

void RenderSDL::SDL_RenderFillCircle(int centerX, int centerY, int radius)
{
    for (int y = -radius; y <= radius; ++y)
    {
        int dx = static_cast<int>(std::sqrt(radius * radius - y * y));
        SDL_RenderDrawLine(renderer, centerX - dx, centerY + y, centerX + dx, centerY + y);
    }
}

int RenderSDL::create(std::string windowName, int screenWidth, int screenHeight, unsigned int currentFlags) {

    Uint32 flags = SDL_WINDOW_OPENGL;

    if (currentFlags & INVISIBLE) flags |= SDL_WINDOW_HIDDEN;
    if (currentFlags & FULLSCREEN) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    if (currentFlags & BORDERLESS) flags |= SDL_WINDOW_BORDERLESS;

    _sdlWindow = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, flags);
    renderer = SDL_CreateRenderer(_sdlWindow, -1, SDL_RENDERER_ACCELERATED);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(_sdlWindow, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    return 0;
}

void RenderSDL::render(ClothSimulator& clothSimulator, Cloth& cloth) {
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    bool updated = false;

    ImGui::Begin("Cloth Parameters");
    if (ImGui::SliderFloat("Particle Radius", &clothSimulator.m_particle_radius, 1.0f, 3.0f)) updated = true;
    if (ImGui::SliderFloat("Bond Strength", &clothSimulator.m_spring_k, 10.0f, 200.0f)) updated = true;
    if (ImGui::SliderFloat("Particle Mass", &clothSimulator.m_particle_mass, 0.1f, 10.0f)) updated = true;
    if (ImGui::SliderFloat("Gravity", &clothSimulator.gravityStrength.y, 0.0f, 1000.0f)) updated = true;
    ImGui::End();

    if (updated) clothSimulator.applyRuntimeParameters(cloth);

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

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void RenderSDL::present() {
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
}

int RenderSDL::destroy() {
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(_sdlWindow);
    SDL_Quit();

    return 0;
}

void RenderSDL::renderParticle(int x, int y, float radius, Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderFillCircle(x, y, radius);
}

void RenderSDL::draw(const Cloth& c) {
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    for (auto& s : c.springs) {
        auto& A = c.particles[s.a];
        auto& B = c.particles[s.b];
        SDL_RenderDrawLine(renderer, (int)A.pos.x, (int)A.pos.y, (int)B.pos.x, (int)B.pos.y);
    }

    for (auto& p : c.particles) {
        renderParticle((int)p.pos.x, (int)p.pos.y, p.radius, p.color);
    }
}