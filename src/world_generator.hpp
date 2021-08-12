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
  public:
    std::size_t m_width;
    std::size_t m_height;
  private:
    WorldSize m_size;
    std::vector<Tile> m_tiles;
    std::vector<size_t> m_anthills;
    Random m_random;

  public:
    WorldGenerator(WorldSize size, std::uint64_t seed);
    int RandomHeight(double min, double max);
    std::vector<int> RandomTerrain(int baseHeight, int maxHeight, double amplitude);

    void GenerateLayers(std::vector<int> surfaceTerrain, std::vector<int> cavernLayer, int underworldLayer);
    void GenerateSurfaceTunnels(std::vector<int> dirtLevel);
    void GenerateSand(std::vector<int> surfaceTerrain, int dirtLevel, std::vector<int> rockHeights);
    void GenerateAnthills(std::vector<int> surfaceTerrain);
    void GenerateSurfaceStone(std::vector<int> start, std::vector<int> end);
    void GenerateUndergroundStone(std::vector<int> start, std::vector<int> end);
    void GenerateCavernDirt(std::vector<int> start, int end);
    void GenerateClay(std::vector<int> start, std::vector<int> mid, std::vector<int> end);
    void GenerateCaves(std::vector<int> undergroundStart);
    void GenerateEntranceCaves(std::vector<int> surface);
    void GenerateLargeCaves(std::vector<int> cavernStart);

    World Finish();
};