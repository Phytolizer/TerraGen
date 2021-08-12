#ifndef TILE_HPP_
#define TILE_HPP_

#include <SDL_video.h>
#include <cstdint>
#include <unordered_map>

struct Tile
{
    // Flags 1
    
    enum class Type
    {
        Air,
        Dirt,
        Grass,
        Sand,
        Clay,
        Stone,
        Ash,
        Water,
        Lava,
    };

    enum class Liquid
    {
        Air,
        Water,
        Lava,
        Honey,
    };

    enum class Wall
    {
        Air,
        Dirt,
        Grass,
    };

    Type type;
    std::int64_t x;
    std::int64_t y;
};

extern std::unordered_map<Tile::Type, SDL_Color> g_Colors;

#endif // TILE_HPP_
