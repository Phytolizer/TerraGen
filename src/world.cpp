#include "world.hpp"

World::World(std::vector<Tile> &&tiles, std::size_t width, std::size_t height)
    : tiles{std::move(tiles)}, width{width}, height{height}
{
    switch (width)
    {
    case WIDTH_TINY:
        size = WorldSize::Tiny;
        break;
    case WIDTH_SMALL:
        size = WorldSize::Small;
        break;
    case WIDTH_MEDIUM:
        size = WorldSize::Medium;
        break;
    case WIDTH_LARGE:
        size = WorldSize::Large;
        break;
    default:
        throw std::logic_error{fmt::format("Invalid world size: {}", tiles.size())};
    }
}
