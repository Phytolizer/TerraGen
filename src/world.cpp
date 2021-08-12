#include "world.hpp"

World::World(std::vector<Tile> &&tiles, std::size_t width, std::size_t height) : tiles{std::move(tiles)}, width{width}, height{height}
{
    switch (width)
    {
    case 1750:
        size = WorldSize::Tiny;
        break;
    case 4200:
        size = WorldSize::Small;
        break;
    case 6400:
        size = WorldSize::Medium;
        break;
    case 8400:
        size = WorldSize::Large;
        break;
    default:
        throw std::logic_error{fmt::format("Invalid world size: {}", tiles.size())};
    }
}
