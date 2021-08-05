#pragma once

#include "random.hpp"
#include "tile.hpp"
#include "world.hpp"
#include "world_size.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>

class WorldGenerator
{
    std::vector<Tile> m_tiles;
    Random m_random;

    std::size_t m_width;
    std::size_t m_height;

  public:
    WorldGenerator(WorldSize size, std::uint64_t seed);
    int GenerateHeight(double min, double max);
    std::vector<int> GenerateTerrain(int baseHeight, double scale, double amplitude);
    void GenerateLayers(std::vector<int> dirtLevel, std::vector<int> stoneLevel, std::vector<int> ashLevel);
    void GenerateDeserts(std::vector<int> surfaceLevel);
    void GenerateSandPiles(int start, int end);
    void GenerateSurface(std::vector<int> start, std::vector<int> end);
    void GenerateUnderground(std::vector<int> start, std::vector<int> end);
    void GenerateCavern(std::vector<int> start, std::vector<int> end);
    void GenerateClay(std::vector<int> start, std::vector<int> mid, std::vector<int> end);
    void GenerateCaves(std::vector<int> undergroundStart);
    void GenerateEntranceCaves(std::vector<int> surface);
    void GenerateLargeCaves(std::vector<int> cavernStart);
    World Finish();
};