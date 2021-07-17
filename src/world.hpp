#ifndef TERRAGEN_WORLD_HPP
#define TERRAGEN_WORLD_HPP

#include "tile.hpp"
#include "world_size.hpp"
#include <fmt/format.h>
#include <stdexcept>
#include <vector>

struct World
{
    std::vector<std::vector<Tile>> tiles;
    WorldSize size;

    World(std::vector<std::vector<Tile>> &&tiles);
};

#endif // TERRAGEN_WORLD_HPP
