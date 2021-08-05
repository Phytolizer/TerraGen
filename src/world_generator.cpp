#include "world_generator.hpp"

WorldGenerator::WorldGenerator(WorldSize size, std::uint64_t seed) : m_random{seed}
{
    switch (size)
    {
    case WorldSize::Tiny:
        m_width = 1280;
        m_height = 720;
        break;
    case WorldSize::Small:
        m_width = 8400;
        m_height = 2400;
        break;
    case WorldSize::Medium:
        m_width = 12800;
        m_height = 3600;
        break;
    case WorldSize::Large:
        m_width = 16800;
        m_height = 4800;
        break;
    }
    m_tiles.resize(m_width * m_height);
}

int WorldGenerator::GenerateHeight(double min, double max)
{
    return m_height * m_random.GetDouble(min, max);
}

// Add Parameters for Spawn Area and Noise Variables
std::vector<int> WorldGenerator::GenerateTerrain(int h)
{
    std::vector<int> terrainHeight(m_width);
    for (int x = 0; x < m_width; ++x)
    {
        // Add Walk
        // int offset = m_random.GetNoise();
        terrainHeight[x] = h; // + offset;
    }
    return terrainHeight;
}

void WorldGenerator::GenerateLayers(std::vector<int> dirtLevel, std::vector<int> stoneLevel)
{
    for (int x = 0; x < m_width; ++x)
    {
        int dirt = dirtLevel[x];
        int stone = stoneLevel[x];
        for (int y = 0; y < dirt; ++y)
        {
            m_tiles[x + m_width * y] = Tile{Tile::Type::Air};
        }
        m_tiles[x + m_width * dirt] = Tile{Tile::Type::Grass};
        for (int y = dirt; y < stone; ++y)
        {
            m_tiles[x + m_width * y] = Tile{Tile::Type::Dirt};
        }
        for (int y = stone; y < m_height; ++y)
        {
            m_tiles[x + m_width * y] = Tile{Tile::Type::Stone};
        }
    }
}

/// Choose a block on surface and create a circle. Set any block to Sand within circle
void WorldGenerator::GenerateDeserts(std::vector<int> surfaceLevel)
{
    int desertCount = m_random.GetInt(2, 4);
    for (int i = 0; i < desertCount; ++i)
    {
        int desertStart = m_random.GetInt(0, m_width);
        int desertSize = m_random.GetInt(50, 200);
        for (int x = desertStart; x < desertStart + desertSize; ++x)
        {
            if (x >= m_width)
            {
                break;
            }
            int h = surfaceLevel[x];
            // int desertHeight = 0—50 randomWalkUpThenDown;
            for (int y = h; y < h + 10 /* + desertHeight */; ++y)
            {
                m_tiles[x + m_width * y] = Tile{Tile::Type::Sand};
            }
        }
    }
}

constexpr double sandPileScale = 1;
constexpr double sandPileCutoff = 0.2;
void WorldGenerator::GenerateSandPiles(int start, int end)
{
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = start; y < end; ++y)
        {
            double n = m_random.GetNoise(x * sandPileScale, y * sandPileScale);
            if (n > sandPileCutoff)
            {
                continue;
            }
            m_tiles[x + m_width * y] = Tile{Tile::Type::Sand};
        }
    }
}

constexpr double surfaceStoneScale = 5;
constexpr double surfaceStoneCutoff = 0.3;
void WorldGenerator::GenerateSurface(std::vector<int> start, std::vector<int> end)
{
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = start[x]; y < end[x]; ++y)
        {
            double n = m_random.GetNoise(x * surfaceStoneScale, y * surfaceStoneScale);
            if (n > surfaceStoneCutoff)
            {
                continue;
            }
            m_tiles[x + m_width * y] = Tile{Tile::Type::Stone};
        }
    }
}

constexpr double undergroundStoneScale = 6;
constexpr double undergroundStoneCutoff = 0.6;
void WorldGenerator::GenerateUnderground(std::vector<int> start, std::vector<int> end)
{
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = start[x]; y < end[x]; ++y)
        {
            double n = m_random.GetNoise(x * undergroundStoneScale, y * undergroundStoneScale);
            if (n > undergroundStoneCutoff)
            {
                continue;
            }
            m_tiles[x + m_width * y] = Tile{Tile::Type::Stone};
        }
    }
}

constexpr double cavernDirtScale = 6;
constexpr double cavernDirtCutoff = 0.6;
void WorldGenerator::GenerateCavern(std::vector<int> start, std::vector<int> end)
{
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = start[x]; y < end[x]; ++y)
        {
            double n = m_random.GetNoise(x * cavernDirtScale, y * cavernDirtScale);
            if (n > cavernDirtCutoff)
            {
                continue;
            }
            m_tiles[x + m_width * y] = Tile{Tile::Type::Dirt};
        }
    }
}

constexpr double clayScale = 2;
constexpr double clayCutoff = 0.2;
void WorldGenerator::GenerateClay(std::vector<int> start, std::vector<int> mid, std::vector<int> end)
{
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = start[x]; y < mid[x]; ++y)
        {
            double n = m_random.GetNoise(x * clayScale, y * clayScale);
            if (n > clayCutoff)
            {
                continue;
            }
            m_tiles[x + m_width * y] = Tile{Tile::Type::Clay};
            for (int y = mid[x]; y < end[x] + 10; ++y)
            {
                double n = m_random.GetNoise(x * clayScale, y * clayScale);
                if (n > clayCutoff / 2)
                {
                    continue;
                }
                m_tiles[x + m_width * y] = Tile{Tile::Type::Clay};
            }
        }
    }
}

World WorldGenerator::Finish()
{
    return World{std::move(m_tiles), m_width, m_height};
}