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

    World(std::vector<std::vector<Tile>> &&tiles) : tiles{tiles}
    {
        switch (tiles.size())
        {
        case 500:
            size = WorldSize::Tiny;
            break;
        case 8400:
            size = WorldSize::Small;
            break;
        case 12800:
            size = WorldSize::Medium;
            break;
        case 16800:
            size = WorldSize::Large;
            break;
        default:
            throw std::logic_error{fmt::format("Invalid world size: {}", tiles.size())};
        }
    }
};

#endif // TERRAGEN_WORLD_HPP
