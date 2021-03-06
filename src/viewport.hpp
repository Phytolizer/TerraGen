#ifndef TERRAGEN_VIEWPORT_HPP
#define TERRAGEN_VIEWPORT_HPP

#include "world.hpp"
#include <SDL_render.h>
#include <SDL_video.h>

struct Viewport
{
    int dx;
    int dy;
    int width;
    int height;
    int tile_size;
    SDL_Window* window;
    SDL_Renderer* renderer;

    Viewport(int dx, int dy, int width, int height, int tileSize);
    ~Viewport();
    Viewport(const Viewport&) = delete;
    Viewport(Viewport&&) = default;
    Viewport& operator=(const Viewport&) = delete;
    Viewport& operator=(Viewport&&) = default;

    void Render(const World& world);
    void Move(int ddx, int ddy);
};

#endif    // TERRAGEN_VIEWPORT_HPP
