#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#pragma once

#include <cstdint>
#include <SDL3/SDL.h>
#include <glad/glad.h>

class Graphics
{
    friend class Imgui;

public:
    Graphics(const char *title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
    ~Graphics();
    void Update(const void *buffer, int pitch);
    bool ProcessInput(uint8_t *keys);

private:
    SDL_Window *window{};
    SDL_GLContext gl_context{};
    GLuint framebuffer_texture;

    SDL_Renderer *renderer{};
    SDL_Texture *texture{};
};

#endif // GRAPHICS_HPP