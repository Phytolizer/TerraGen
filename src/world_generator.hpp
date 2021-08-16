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
  private:
    std::size_t m_width;
    std::size_t m_height;
    WorldSize m_size;
    std::vector<Tile> m_tiles;
    Random m_random;

    int ComputeStartCoordinate(int side);
    int ComputeWithinUsableArea(
        const std::vector<int>& surfaceTerrain, int side, int size, Tile::Type mask = Tile::Type::Air);
    void FillBlobAtRandomPosition(Vector2<int> horizontal, Vector2<int> vertical, Tile::Type, Vector2<double> size, Vector2<double> variation);

  public:
    WorldGenerator(WorldSize size, std::uint64_t seed);
    void SetTile(int x, int y, Tile::Type type);
    void SetWall(int x, int y, Tile::Wall wall);
    void SetLiquid(int x, int y, Tile::Liquid liquid);
    void SetDepth(int x, int y, Tile::Depth depth);
    bool IsTile(int x, int y, Tile::Type type);
    bool IsWall(int x, int y, Tile::Wall wall);
    bool IsLiquid(int x, int y, Tile::Liquid liquid);
    void FillBlob(
        int x,
        int y,
        Tile::Type type,
        double radius,
        double variation,
        bool replaceAir = false,
        bool overrideBlocks = true);
    int RandomHeight(double min, double max);
    std::vector<int> RandomTerrain(int minHeight, int maxHeight, double amplitude, int timer);
    [[nodiscard]] std::size_t GetHeight() const;

    // World Setup
    void GenerateDepthLevels(int surface, int cavern, int underworld);
    void GenerateLayers(const std::vector<int>& dirtTerrain, const std::vector<int>& stoneTerrain, int ash);
    void GenerateSurfaceTunnels(const std::vector<int>& surfaceTerrain);
    void GenerateSandDesert(const std::vector<int>& surfaceTerrain);
    void GenerateSandPiles(int dirtLevel, const std::vector<int>& rockHeights);
    std::vector<int> GenerateAnthills(const std::vector<int>& surfaceTerrain);
    void GenerateSurfaceStone(const std::vector<int>& start, const std::vector<int>& end);
    void GenerateUndergroundStone(const std::vector<int>& start, const std::vector<int>& end);
    void GenerateCavernDirt(const std::vector<int>& start, int end);
    // Caves
    void GenerateCaves(const std::vector<int>& undergroundStart);
    void GenerateEntranceCaves(const std::vector<int>& surface);
    void GenerateLargeCaves(const std::vector<int>& cavernStart);
    // Scattered Blocks
    void GenerateClay(const std::vector<int>& start, const std::vector<int>& mid, const std::vector<int>& end);
    void GenerateGrass(const std::vector<int>& start, int end);
    void GenerateMud(int start, int end);
    void GenerateSilt(int start, int end);
    // Biomes Part 1
    // Metals, Gems, and Webs
    void GenerateMetals(int surface, int underground, int cavern, int underworld);
    void GenerateGems(int start, int end);
    void GenerateWebs(int start, int end);
    // Biomes Part 2
    // Fixes
    void GenerateAnthillCaves(const std::vector<int>& positions);
    void FixGravitatingSand(const std::vector<int>& surface);
    void FixDirtWalls(const std::vector<int>& surface);
    void FixWaterOnSand(const std::vector<int>& surface);
    // Biomes Part 3
    // Clean up World
    void SmoothWorld();
    void SettleLiquids();
    void AddWaterfalls();

    World Finish();
};