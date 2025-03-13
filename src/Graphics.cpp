#include "Graphics.hpp"
#include <iostream>

Graphics::Graphics(const char *title)
{
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    int panelWidth = 200;
    int panelHeight = 100; 

    window = SDL_CreateWindow(title, CHIP8_SCREEN_WIDTH + panelWidth, CHIP8_SCREEN_HEIGHT + panelHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!window)
    {
        SDL_Log("Window could not be created! SDL_Error: %s", SDL_GetError());
        exit(1);
    }

    // Create OpenGL Context
    gl_context = SDL_GL_CreateContext(window);
    if (!gl_context)
    {
        SDL_Log("Failed to create OpenGL context: %s", SDL_GetError());
        exit(1);
    }

    // Enable V-Sync
    SDL_GL_SetSwapInterval(1);

    // Create SDL Renderer
    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
        SDL_Log("Renderer could not be created! SDL_Error: %s", SDL_GetError());
        exit(1);
    }
    //Create SDL Texture
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
    if (!texture)
    {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        exit(1);
    }
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
}

Graphics::~Graphics()
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Graphics::Update(const void *buffer, int pitch)
{
    if (!buffer || !texture)
    {
        SDL_Log("Buffer or Texture is NULL!");
        return;
    }

    SDL_UpdateTexture(texture, NULL, buffer, pitch);
    SDL_RenderClear(renderer);
}

void Graphics::DisplayRegisters(uint8_t *registers)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE); /* white, full alpha */
    SDL_RenderDebugText(renderer, CHIP8_SCREEN_WIDTH, 0, "registers");
    int registerTextHeight = 10;
    for (int i = 0; i < 16; i++)
    {
        char buffer[4];
        snprintf(buffer, sizeof(buffer), "%u", registers[i]);
        SDL_RenderDebugText(renderer, CHIP8_SCREEN_WIDTH + 50, registerTextHeight, buffer);
        snprintf(buffer, sizeof(buffer), "%d", i + 1);
        SDL_RenderDebugText(renderer, CHIP8_SCREEN_WIDTH, registerTextHeight, buffer);
        registerTextHeight += 10;
    }
}

void Graphics::DisplayStack(uint16_t *stack)
{

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE); /* white, full alpha */
    SDL_RenderDebugText(renderer, CHIP8_SCREEN_WIDTH + 80, 0, "stack");
    int stackTextHeight = 10;
    for (int i = 0; i < 16; i++)
    {
        char buffer[8];
        snprintf(buffer, sizeof(buffer), "%u", stack[i]);
        SDL_RenderDebugText(renderer, CHIP8_SCREEN_WIDTH + 130, stackTextHeight, buffer);
        snprintf(buffer, sizeof(buffer), "%d", i + 1);
        SDL_RenderDebugText(renderer, CHIP8_SCREEN_WIDTH + 80, stackTextHeight, buffer);
        stackTextHeight += 10;
    }
}

void Graphics::DisplayPC(uint16_t pc)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE); /* white, full alpha */
    SDL_RenderDebugText(renderer, CHIP8_SCREEN_WIDTH, 180, "Program Counter");
    char buffer[8];
    snprintf(buffer, sizeof(buffer), "%d", pc);
    SDL_RenderDebugText(renderer, 770, 180, buffer);
}

void Graphics::DisplaySP(uint8_t sp)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE); /* white, full alpha */
    SDL_RenderDebugText(renderer, CHIP8_SCREEN_WIDTH, 200, "Stack Pointer");
    char buffer[4];
    snprintf(buffer, sizeof(buffer), "%d", sp);
    SDL_RenderDebugText(renderer, 770, 200, buffer);
}

void Graphics::DisplayCycleDelay()
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE); /* white, full alpha */
    SDL_RenderDebugText(renderer, CHIP8_SCREEN_WIDTH, 220, "Cycle Delay");
    char buffer[4];
    snprintf(buffer, sizeof(buffer), "%d", cycleDelay);
    SDL_RenderDebugText(renderer, 770, 220, buffer);
    SDL_RenderDebugText(renderer, CHIP8_SCREEN_WIDTH, 250, "Use left & right arrow");
    SDL_RenderDebugText(renderer, CHIP8_SCREEN_WIDTH, 260, "keys to change");
    SDL_RenderDebugText(renderer, CHIP8_SCREEN_WIDTH, 270, "cycle delay");
}

void Graphics::DisplayMemory(uint8_t *memory)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE); /* white, full alpha */
    SDL_RenderDebugText(renderer, 0, CHIP8_SCREEN_HEIGHT, "Memory: Use up & down arrow keys to scroll through");
    for (int row = 0; row < (visibleRows / 2); row++)
    {
        int memAddress = memoryOffset + (row * BYTES_PER_ROW);
        if (memAddress >= 0x1000)
            break; // Stop if beyond CHIP-8 memory

        // Format the memory row (e.g., "0200: A2 F0 33 44 ...")
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "%04X: ", memAddress);
        for (int i = 0; i < BYTES_PER_ROW; i++)
        {
            snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "%02X ", memory[memAddress + i]);
        }
        // Render text using SDL_RenderDebugText
        SDL_RenderDebugText(renderer, PANEL_X + 10, PANEL_Y + 10 + (row * 12), buffer);
    }
}

void Graphics::DistplayInstructions(std::string instruction)
{
    SDL_RenderDebugText(renderer, PANEL_X + 500, CHIP8_SCREEN_HEIGHT, "Instructions");
    instructionQueue.push(instruction);
    const int queueSize = instructionQueue.size();
    if (queueSize > (visibleRows / 2))
    {
        instructionQueue.pop();
    }
    int row = 0;
    std::queue<std::string> tempQueue = instructionQueue;
    while (!tempQueue.empty())
    {
        SDL_RenderDebugText(renderer, PANEL_X + 500, PANEL_Y + 10 + (row * 12), tempQueue.front().c_str());
        row++;
        tempQueue.pop();
    }
}

void Graphics::DrawDebugBordrer()
{
    // Define Chip8 screen position
    SDL_FRect chip8ScreenRect = {0.0f, 0.0f, CHIP8_SCREEN_WIDTH, CHIP8_SCREEN_HEIGHT}; // Use floats for SDL_FRect
    SDL_RenderTexture(renderer, texture, NULL, &chip8ScreenRect);

    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    // Draw the register panel (right side)
    SDL_SetRenderDrawColor(renderer, 10, 0, 0, 255);                                                                                 
    SDL_FRect registerPanel = {CHIP8_SCREEN_WIDTH, 0.0f, static_cast<float>(windowWidth) - CHIP8_SCREEN_WIDTH, CHIP8_SCREEN_HEIGHT}; 
    SDL_RenderFillRect(renderer, &registerPanel);

    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);                                                                                           
    SDL_FRect debugPanel = {0.0f, CHIP8_SCREEN_HEIGHT, static_cast<float>(windowWidth), static_cast<float>(windowHeight) - CHIP8_SCREEN_HEIGHT}; 
    SDL_RenderFillRect(renderer, &debugPanel);
}

void Graphics::EndDraw()
{
    // Reset render color to prevent affecting other elements
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Reset to black (or your background color)
    SDL_RenderPresent(renderer);
}

bool Graphics::ProcessInput(uint8_t *keys)
{
    bool quit = false;
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
        {
            quit = true;
        }
        break;

        case SDL_EVENT_KEY_DOWN:
        {
            switch (event.key.key)
            {
            case SDLK_DOWN:
            {
                if (memoryOffset + (visibleRows * BYTES_PER_ROW) < 0x1000)
                {
                    memoryOffset += BYTES_PER_ROW;
                }
            }
            break;
            case SDLK_RIGHT:
            {
                if (cycleDelay < 100)
                    cycleDelay++;
            }
            break;
            case SDLK_LEFT:
            {
                if (cycleDelay > 1)
                    cycleDelay--;
            }
            break;
            case SDLK_UP:
            {
                if (memoryOffset > 0)
                {
                    memoryOffset -= BYTES_PER_ROW;
                }
            }
            break;
            case SDLK_ESCAPE:
            {
                quit = true;
            }
            break;

            case SDLK_X:
            {
                keys[0] = 1;
            }
            break;

            case SDLK_1:
            {
                keys[1] = 1;
            }
            break;

            case SDLK_2:
            {
                keys[2] = 1;
            }
            break;

            case SDLK_3:
            {
                keys[3] = 1;
            }
            break;

            case SDLK_Q:
            {
                keys[4] = 1;
            }
            break;

            case SDLK_W:
            {
                keys[5] = 1;
            }
            break;

            case SDLK_E:
            {
                keys[6] = 1;
            }
            break;

            case SDLK_A:
            {
                keys[7] = 1;
            }
            break;

            case SDLK_S:
            {
                keys[8] = 1;
            }
            break;

            case SDLK_D:
            {
                keys[9] = 1;
            }
            break;

            case SDLK_Z:
            {
                keys[0xA] = 1;
            }
            break;

            case SDLK_C:
            {
                keys[0xB] = 1;
            }
            break;

            case SDLK_4:
            {
                keys[0xC] = 1;
            }
            break;

            case SDLK_R:
            {
                keys[0xD] = 1;
            }
            break;

            case SDLK_F:
            {
                keys[0xE] = 1;
            }
            break;

            case SDLK_V:
            {
                keys[0xF] = 1;
            }
            break;
            }
        }
        break;

        case SDL_EVENT_KEY_UP:
        {
            switch (event.key.key)
            {
            case SDLK_X:
            {
                keys[0] = 0;
            }
            break;

            case SDLK_1:
            {
                keys[1] = 0;
            }
            break;

            case SDLK_2:
            {
                keys[2] = 0;
            }
            break;

            case SDLK_3:
            {
                keys[3] = 0;
            }
            break;

            case SDLK_Q:
            {
                keys[4] = 0;
            }
            break;

            case SDLK_W:
            {
                keys[5] = 0;
            }
            break;

            case SDLK_E:
            {
                keys[6] = 0;
            }
            break;

            case SDLK_A:
            {
                keys[7] = 0;
            }
            break;

            case SDLK_S:
            {
                keys[8] = 0;
            }
            break;

            case SDLK_D:
            {
                keys[9] = 0;
            }
            break;

            case SDLK_Z:
            {
                keys[0xA] = 0;
            }
            break;

            case SDLK_C:
            {
                keys[0xB] = 0;
            }
            break;

            case SDLK_4:
            {
                keys[0xC] = 0;
            }
            break;

            case SDLK_R:
            {
                keys[0xD] = 0;
            }
            break;

            case SDLK_F:
            {
                keys[0xE] = 0;
            }
            break;

            case SDLK_V:
            {
                keys[0xF] = 0;
            }
            break;
            }
        }
        break;
        }
    }

    return quit;
}

int Graphics::getCycleDelay()
{
    return cycleDelay;
}

void Graphics::setCycleDelay(int delay)
{
    cycleDelay = delay;
}
