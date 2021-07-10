#include "viewport.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <fmt/format.h>
#include <stdexcept>

Viewport::Viewport(int dx, int dy, int width, int height, int tileSize)
    : dx{dx}, dy{dy}, width{width}, height{height}, tileSize{tileSize},
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

void Viewport::render(const World &world)
{
    for (int i = std::max(dx, 0); i < std::min<int>(world.tiles.size(), dx + width / tileSize); ++i)
    {
        for (int j = std::max(dy, 0); j < std::min<int>(world.tiles[0].size(), dy + height / tileSize); ++j)
        {
            SDL_Rect rect{(i - dx) * tileSize, (j - dy) * tileSize, tileSize, tileSize};

            auto color = TILE_COLORS.at(world.tiles.at(i).at(j).type);
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}
