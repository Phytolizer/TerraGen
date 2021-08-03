#ifndef TERRAGEN_WORLD_HPP
#define TERRAGEN_WORLD_HPP

#include "tile.hpp"
#include "world_size.hpp"
#include <fmt/format.h>
#include <vector>

struct World
{
    std::vector<Tile::Tile> tiles;
    std::size_t width;
    std::size_t height;
    WorldSize size;

    explicit World(std::vector<Tile::Tile> &&tiles, std::size_t width, std::size_t height);
};

#endif // TERRAGEN_WORLD_HPP
