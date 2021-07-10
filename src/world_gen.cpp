#include "world_gen.hpp"
#include <noise.h>

namespace WorldGen
{

static void FillRow(std::vector<std::vector<Tile>> *tiles, int row, TileType type);

World Generate(WorldSize size)
{
    std::random_device _rd;
    std::default_random_engine eng(_rd());

    std::vector<std::vector<Tile>> tiles;
    int width;
    int height;
    switch (size)
    {
    case WorldSize::Tiny:
        width = 500;
        height = 500;
        break;
    case WorldSize::Small:
        width = 8400;
        height = 2400;
        break;
    case WorldSize::Medium:
        width = 12800;
        height = 3600;
        break;
    case WorldSize::Large:
        width = 16800;
        height = 4800;
        break;
    }
    tiles.reserve(width);
    for (int i = 0; i < width; ++i)
    {
        tiles.emplace_back(height);
    }

    /* LAYERS
     * 06% Space
     * 18% Surface
     *      - 12% Air
     *      - 6% Ground
     * 10% Underground
     * 35% Cavern (Water)
     * 20% Cavern (Lava)
     * 11% Underworld
     */

    // FILL ALL AIR

    // Choose Surface line (16-20% down)
    // For each in width
    //      Choose a height from line (using perlin noise)
    //      Fill all beneath to dirt

    // Choose Cavern line (32-36% down)
    // For each in width
    //      Choose a height from line (using perlin noise)
    //      Fill all beneath to stone

    for (int row = 0; row < height; ++row)
    {
        double pct = static_cast<double>(row) / static_cast<double>(height);
        if (pct < 0.06)
        {
            // space
            FillRow(&tiles, row, TileType::SpaceAir);
        }
        else if (pct < 0.24)
        {
            // surface
            FillRow(&tiles, row, TileType::Air);
        }
        else if (pct < 0.34)
        {
            // underground
            FillRow(&tiles, row, TileType::Dirt);
        }
        else if (pct < 0.69)
        {
            // cavern (water)
            FillRow(&tiles, row, TileType::Water);
        }
        else if (pct < 0.89)
        {
            // cavern (lava)
            FillRow(&tiles, row, TileType::Lava);
        }
        else
        {
            // underworld
            FillRow(&tiles, row, TileType::Underworld);
        }
    }

    /* BIOMES
     * Forest
     * Corruption/Crimson (mutually exclusive)
     * Ocean
     * Desert
     * Big desert (+caves)
     */

    return World{std::move(tiles)};
}

static void FillRow(std::vector<std::vector<Tile>> *tiles, int row, TileType type)
{
    for (int i = 0; i < tiles->size(); ++i)
    {
        (*tiles)[i][row].type = type;
    }
}

} // namespace WorldGen
