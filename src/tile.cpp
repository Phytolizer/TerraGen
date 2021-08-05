#include "tile.hpp"

std::unordered_map<Tile::Type, SDL_Color> g_Colors{
    // Air
    {Tile::Type::Air, {123, 152, 254, 255}},
    // Block
    {Tile::Type::Dirt, {151, 107, 75, 255}},
    {Tile::Type::Grass, {40, 182, 80, 255}},
    {Tile::Type::Sand, {190, 170, 80, 255}},
    {Tile::Type::Clay, {146, 81, 68, 255}},
    {Tile::Type::Stone, {128, 128, 128, 255}},
    {Tile::Type::Ash, {79, 65, 64, 255}},
    // Liquids
    {Tile::Type::Water, {9, 61, 255, 255}},
    {Tile::Type::Lava, {253, 32, 3, 255}},
};
