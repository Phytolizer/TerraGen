#include "tile.hpp"

std::unordered_map<Tile::Type, SDL_Color> g_Colors{
    // Airs
    {Tile::Type::Air, {145, 216, 250, 255}},
    {Tile::Type::Dirt, {155, 118, 83, 255}},
    {Tile::Type::Grass, {27, 216, 94, 255}},
    {Tile::Type::Sand, {27, 216, 94, 255}},
    {Tile::Type::Clay, {27, 216, 94, 255}},
    {Tile::Type::Stone, {160, 160, 160, 255}},
    {Tile::Type::Ash, {153, 0, 0, 255}},
    // Liquids
    {Tile::Type::Water, {30, 144, 255, 255}},
    {Tile::Type::Lava, {255, 75, 0, 255}},
};
