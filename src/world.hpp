#ifndef TERRAGEN_WORLD_HPP
#define TERRAGEN_WORLD_HPP

#include "tile.hpp"
#include "world_size.hpp"
#include <fmt/format.h>
#include <vector>

struct World
{
    std::vector<Tile> tiles;
    std::size_t width;
    std::size_t height;
    WorldSize size;

    static constexpr int WIDTH_TINY = 1750;
    static constexpr int HEIGHT_TINY = 900;
    static constexpr int WIDTH_SMALL = 4200;
    static constexpr int HEIGHT_SMALL = 1200;
    static constexpr int WIDTH_MEDIUM = 6400;
    static constexpr int HEIGHT_MEDIUM = 1800;
    static constexpr int WIDTH_LARGE = 8400;
    static constexpr int HEIGHT_LARGE = 2400;

    explicit World(std::vector<Tile> &&tiles, std::size_t width, std::size_t height);
};

#endif    // TERRAGEN_WORLD_HPP
