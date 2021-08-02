#ifndef TERRAGEN_TILE_HPP
#define TERRAGEN_TILE_HPP

#include <SDL_video.h>
#include <cstdint>
#include <unordered_map>

enum class TileType
{
    SpaceAir,
    Air,
    Dirt,
    Stone,
    Water,
    Lava,
    Underworld
};

extern std::unordered_map<TileType, SDL_Color> TILE_COLORS;

struct Tile
{
    TileType type;
    std::int64_t x;
    std::int64_t y;
};

#endif // TERRAGEN_TILE_HPP
