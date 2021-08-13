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
    Random m_random;

  public:
    WorldGenerator(WorldSize size, std::uint64_t seed);
    void SetTile(int x, int y, Tile::Type type);
    void SetWall(int x, int y, Tile::Wall wall);
    void SetLiquid(int x, int y, Tile::Liquid liquid);
    void SetDepth(int x, int y, Tile::Depth depth);
    bool IsTile(int x, int y, Tile::Type type);
    bool IsWall(int x, int y, Tile::Wall wall);
    bool IsLiquid(int x, int y, Tile::Liquid liquid);
    void FillBlob(int x, int y, Tile::Type type, double radius, double variation, bool replaceAir = false, bool overrideBlocks = true);
    int RandomHeight(double min, double max);
    std::vector<int> RandomTerrain(int baseHeight, int maxHeight, double amplitude);

    // World Setup
    void GenerateDepthLevels(int surface, int cavern, int underworld);
    void GenerateLayers(std::vector<int> surfaceTerrain, std::vector<int> cavernTerrain, int ash);
    void GenerateSurfaceTunnels(std::vector<int> surfaceTerrain);
    void GenerateSand(std::vector<int> surfaceTerrain, int dirtLevel, std::vector<int> rockHeights);
    std::vector<int> GenerateAnthills(std::vector<int> surfaceTerrain);
    void GenerateSurfaceStone(std::vector<int> start, std::vector<int> end);
    void GenerateUndergroundStone(std::vector<int> start, std::vector<int> end);
    void GenerateCavernDirt(std::vector<int> start, int end);
    // Caves
    void GenerateCaves(std::vector<int> undergroundStart);
    void GenerateEntranceCaves(std::vector<int> surface);
    void GenerateLargeCaves(std::vector<int> cavernStart);
    // Scattered Blocks
    void GenerateClay(std::vector<int> start, std::vector<int> mid, std::vector<int> end);
    void GenerateGrass(std::vector<int> start, int end);
    void GenerateMud(int start, int end);
    void GenerateSilt(int start, int end);
    // Biomes Part 1
    // Metals, Gems, and Webs
    void GenerateMetals(std::vector<int> surface, int underground, int cavern, int underworld);
    void GenerateGems(int start, int end);
    void GenerateWebs(int start, int end);
    // Biomes Part 2
    // Fixes
    void GenerateAnthillCaves(std::vector<int> positions);
    void FixGravitatingSand(std::vector<int> surface);
    void FixDirtWalls(std::vector<int> surface);
    void FixWaterOnSand(std::vector<int> surface);
    // Biomes Part 3
    // Clean up World
    void SmoothWorld();
    void SettleLiquids();
    void AddWaterfalls();

    World Finish();
};