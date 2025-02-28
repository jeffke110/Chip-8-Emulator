#include "Graphics.hpp"
#include <iostream>

Graphics::Graphics(const char* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight)
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    window = SDL_CreateWindow(
        title, 
        windowWidth, windowHeight, 
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	gl_context = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(1);
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        SDL_Log("Failed to initialize OpenGL context");
    }

	glGenTextures(1, &framebuffer_texture);
	glBindTexture(GL_TEXTURE_2D, framebuffer_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 640, 320, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}



Graphics::~Graphics()
{
    SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Graphics::Update(const void* buffer, int pitch)
{
	SDL_UpdateTexture(texture, nullptr, buffer, pitch);
	SDL_RenderClear(renderer);
	SDL_RenderTexture(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}

bool Graphics::ProcessInput(uint8_t* keys)
{
    bool quit = false;
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_EVENT_QUIT:
                quit = true;
                break;

            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP:
            {
                bool keyPressed = (event.type == SDL_EVENT_KEY_DOWN);
                SDL_Keycode key = event.key.key;

                switch (key)
                {
                    case SDLK_ESCAPE: quit = true; break;
                    case SDLK_X: keys[0] = keyPressed; break;
                    case SDLK_1: keys[1] = keyPressed; break;
                    case SDLK_2: keys[2] = keyPressed; break;
                    case SDLK_3: keys[3] = keyPressed; break;
                    case SDLK_Q: keys[4] = keyPressed; break;
                    case SDLK_W: keys[5] = keyPressed; break;
                    case SDLK_E: keys[6] = keyPressed; break;
                    case SDLK_S: keys[7] = keyPressed; break;
                    case SDLK_A: keys[8] = keyPressed; break;
                    case SDLK_D: keys[9] = keyPressed; break;
                    case SDLK_Z: keys[0xA] = keyPressed; break;
                    case SDLK_C: keys[0xB] = keyPressed; break;
                    case SDLK_4: keys[0xC] = keyPressed; break;
                    case SDLK_R: keys[0xD] = keyPressed; break;
                    case SDLK_F: keys[0xE] = keyPressed; break;
                    case SDLK_V: keys[0xF] = keyPressed; break;
                }
            } break;
        }
    }
    return quit;
}
