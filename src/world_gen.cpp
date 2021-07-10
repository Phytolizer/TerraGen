#include "world_gen.hpp"
#include <noise.h>

namespace WorldGen
{

static void FillRow(std::vector<std::vector<Tile>> *tiles, int row, TileType type);

World Generate(WorldSize size)
{
    std::random_device _rd;
    std::default_random_engine eng{_rd()};
    int seed = std::uniform_int_distribution<>{0, INT_MAX}(eng);
    noise::module::Perlin perlin;

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

    // Choose Surface line (16-20% down)
    int surfaceLine = 100;
    double surfaceScale = 0.08;
    double surfaceAmplitude = 4;

    std::vector<int> surfaceHeights;
    surfaceHeights.reserve(width);
    for (int i = 0; i < width; ++i)
    {
        int h = surfaceLine + surfaceAmplitude * perlin.GetValue(0, i * surfaceScale, seed);
        surfaceHeights.push_back(h);
    }

    // Choose Cavern line (32-36% down)
    int cavernLine = 200;
    double cavernScale = 0.02;
    double cavernAmplitude = 10;

    std::vector<int> cavernHeights;
    cavernHeights.reserve(width);
    for (int i = 0; i < width; ++i)
    {
        int h = cavernLine + cavernAmplitude * perlin.GetValue(1, i * cavernScale, seed);
        cavernHeights.push_back(h);
    }

    for (int col = 0; col < width; ++col)
    {
        for (int row = 0; row < height; ++row)
        {
            if (row < surfaceHeights[col])
            {
                tiles[col][row].type = TileType::Air;
            }
            else if (row < cavernHeights[col])
            {
                tiles[col][row].type = TileType::Dirt;
            }
            else
            {
                tiles[col][row].type = TileType::Stone;
            }
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
