#include "tile.hpp"

std::unordered_map<TileType, SDL_Color> TILE_COLORS{
    // Airs
    {TileType::SpaceAir, {0, 0, 139}},
    {TileType::Air, {145, 216, 250}},
    {TileType::Dirt, {155, 118, 83}},
    {TileType::Stone, {160, 160, 160}},
    // Liquids
    {TileType::Water, {30, 144, 255}},
    {TileType::Lava, {255, 75, 0}},
    {TileType::Underworld, {153, 0, 0}},

};
