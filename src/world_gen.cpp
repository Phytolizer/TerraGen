#include "world_gen.hpp"
#include "randomness.hpp"
#include <FastNoiseLite.h>
#include <chrono>

namespace WorldGen
{

static void FillRow(std::vector<std::vector<Tile>> *tiles, int row, TileType type);

World Generate(WorldSize size)
{
    auto start = std::chrono::high_resolution_clock::now();
    std::random_device _rd;
    std::default_random_engine eng{_rd()};
    int seed = 100; // std::uniform_int_distribution<>{0, INT_MAX}(eng);
    eng.seed(seed);
    FastNoiseLite n;

    std::vector<std::vector<Tile>> tiles;
    int width = 0;
    int height = 0;
    switch (size)
    {
    case WorldSize::Tiny:
        width = 1280;
        height = 720;
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
    int surfaceLine = RandInt(height * 0.16, height * 0.2, eng);
    // double surfaceScale = 0.06;
    // double surfaceAmplitude;
    // int surfaceAmplitudeTime = 0;

    std::vector<int> surfaceHeights;
    surfaceHeights.reserve(width);
    // VARIABLES TO SET
    int spawnArea = 30;
    int spawnAreaAmplitude = 1;
    int spawnAreaFallout = 60;
    double perlinScale = 5;
    double perlinAmplitudeMin = 0.5;
    double perlinAmplitudeMax = 10;
    int amplitudeTimeMin = width / 32;
    int amplitudeTimeMax = width / 12;
    // Other Variables
    double perlinAmplitude;
    int amplitudeTime = 0;
    for (int i = 0; i < width; ++i)
    {
        // if (--surfaceAmplitudeTime <= 0)
        //{
        // surfaceAmplitude = std::uniform_real_distribution<double>{0, std::min<double>(12, 0.5 + i * 0.1)}(eng);
        // surfaceAmplitudeTime = std::uniform_int_distribution<>{width / 32, width / 12}(eng);
        //}
        // int surfaceOffset = perlin.GetValue(10, i * 0.00001, seed) * 25;
        // int h = surfaceLine + surfaceOffset + surfaceAmplitude * perlin.GetValue(0, i * surfaceScale, seed);

        int dist = std::abs(i - width / 2);

        double noiseScale1 = n.GetNoise<double>(0, i * perlinScale);
        double noiseScale2 = n.GetNoise<double>(0, i * perlinScale / 2) / 2;
        double noiseScale4 = n.GetNoise<double>(0, i * perlinScale / 4) / 4;

        double noise = noiseScale1 + noiseScale2 + noiseScale4;

        double amplitude = 0;
        if (dist <= spawnArea)
        {
            amplitude = spawnAreaAmplitude;
        }
        else
        {
            if (--amplitudeTime <= 0)
            {
                amplitudeTime = RandInt(perlinAmplitudeMin, amplitudeTimeMax, eng);
                perlinAmplitude = RandDouble(perlinAmplitudeMin, perlinAmplitudeMax, eng);
            }
            if (dist <= spawnArea + spawnAreaFallout)
            {
                double ampSwitch = (dist - spawnArea) / spawnAreaFallout;
                amplitude = spawnAreaAmplitude * (1 - ampSwitch) + perlinAmplitude * ampSwitch;
            }
            else
            {
                amplitude = perlinAmplitude; // * (1 + (width / 2) * static_cast<double>(dist) / width / 2);
            }
        }
        int pos = surfaceLine + noise * amplitude;

        surfaceHeights.push_back(pos);
    }

    // Choose Cavern line (32-36% down)
    int cavernLine = RandInt(height * 0.32, height * 0.36, eng);
    // double cavernScale = 0.02;
    // double cavernAmplitude = 10;

    std::vector<int> cavernHeights;
    cavernHeights.reserve(width);
    for (int i = 0; i < width; ++i)
    {
        cavernHeights.push_back(cavernLine);
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

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = end - start;
    fmt::print("Took {} years\n", elapsed.count());
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
