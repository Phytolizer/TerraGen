#ifndef TILE_HPP_
#define TILE_HPP_

#include <SDL_video.h>
#include <cstdint>
#include <unordered_map>

namespace Tile
{

enum class Type
{
    SpaceAir,
    Air,
    Dirt,
    Grass,
    Stone,
    Water,
    Lava,
    Underworld
};

extern std::unordered_map<Type, SDL_Color> g_Colors;

struct Tile
{
    Type type;
    std::int64_t x;
    std::int64_t y;
};

} // namespace Tile

#endif // TILE_HPP_
