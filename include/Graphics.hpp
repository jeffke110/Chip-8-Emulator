#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#pragma once

#include <cstdint>
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <string>
#include <queue>

class Graphics
{
    friend class Imgui;

public:
    Graphics(const char *title);
    ~Graphics();

    void Update(const void *buffer, int pitch);
    void DisplayRegisters(uint8_t *registers);
    void DisplayStack(uint16_t *stack);
    void DisplayPC(uint16_t pc);
    void DisplaySP(uint8_t sp);
    void DisplayCycleDelay();
    void DisplayMemory(uint8_t *memory);
    void DistplayInstructions(std::string instruction);
    void DrawDebugBordrer();
    void EndDraw();

    bool ProcessInput(uint8_t *keys);
    
    int getCycleDelay();
    void setCycleDelay(int delay);

private:
    const float WINDOW_WIDTH = 740;
    const float WINDOW_HEIGHT = 520;
    const float CHIP8_SCREEN_WIDTH = 640;
    const float CHIP8_SCREEN_HEIGHT = 320;
    const int PANEL_WIDTH = 740;
    const int PANEL_HEIGHT = 200;
    const int PANEL_X = 0;
    const int PANEL_Y = CHIP8_SCREEN_HEIGHT;
    const int BYTES_PER_ROW = 16;
    const int visibleRows = PANEL_HEIGHT / 12;
    int memoryOffset = 0x000;
    
    std::queue<std::string> instructionQueue;
    int cycleDelay = 3;

    SDL_Window *window{};
    SDL_Renderer *renderer{};
    SDL_Texture *texture{};
    SDL_GLContext gl_context{};
    GLuint framebuffer_texture;
};

#endif // GRAPHICS_HPP