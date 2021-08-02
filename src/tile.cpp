#include "tile.hpp"

std::unordered_map<Tile::Type, SDL_Color> Tile::g_Colors{
    // Airs
    {Type::SpaceAir, {0, 0, 139, 255}},
    {Type::Air, {145, 216, 250, 255}},
    {Type::Dirt, {155, 118, 83, 255}},
    {Type::Stone, {160, 160, 160, 255}},
    // Liquids
    {Type::Water, {30, 144, 255, 255}},
    {Type::Lava, {255, 75, 0, 255}},
    {Type::Underworld, {153, 0, 0, 255}},
};
