#include "viewport.hpp"
#include <SDL.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <fmt/format.h>
#include <stdexcept>

Viewport::Viewport(int dx, int dy, int width, int height, int tileSize)
    : dx{dx}, dy{dy}, width{width}, height{height}, tile_size{tileSize},
      window{SDL_CreateWindow("TerraGen", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN)},
      renderer{SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)}
{
    if (!window)
    {
        throw std::runtime_error{fmt::format("could not create SDL window: {}", SDL_GetError())};
    }
    if (!renderer)
    {
        throw std::runtime_error{fmt::format("could not create SDL renderer: {}", SDL_GetError())};
    }
}

Viewport::~Viewport()
{
    SDL_DestroyWindow(window);
}

// dx dy = Block on map from top left
// 0 0 = TOP LEFT
// min(dx, world_height)
// width = viewport width moving right
// height = viewport width moving down

void Viewport::Render(const World &world)
{
    for (int i = std::max(dx, 0); i < std::min<int>(world.width, dx + width / tile_size); ++i)
    {
        for (int j = std::max(dy, 0); j < std::min<int>(world.height, dy + height / tile_size); ++j)
        {
            SDL_Rect rect{(i - dx) * tile_size, (j - dy) * tile_size, tile_size, tile_size};

            Tile tile = world.tiles.at(i + j * world.width);
            const auto [r, g, b, a] = tile.GetColor();
            SDL_SetRenderDrawColor(renderer, r, g, b, a);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void Viewport::Move(int ddx, int ddy)
{
    dx += ddx;
    dy += ddy;
}
