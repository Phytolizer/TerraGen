#include "world.hpp"

World::World(std::vector<Tile::Tile> &&tiles, std::size_t width, std::size_t height) : tiles{std::move(tiles)}, width{width}, height{height}
{
    switch (width)
    {
    case 1280:
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
