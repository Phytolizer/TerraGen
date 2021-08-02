#ifndef TERRAGEN_WORLD_HPP
#define TERRAGEN_WORLD_HPP

#include "tile.hpp"
#include "world_size.hpp"
#include <fmt/format.h>
#include <vector>

struct World
{
    std::vector<std::vector<Tile::Tile>> tiles;
    WorldSize size;

    explicit World(std::vector<std::vector<Tile::Tile>> &&tiles);
};

#endif // TERRAGEN_WORLD_HPP
