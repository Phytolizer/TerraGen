#include "world_generator.hpp"
#include <cstdlib>
#include <math.h>

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

std::vector<int> WorldGenerator::GenerateTerrain(int baseHeight, double scale, double amplitude)
{
    std::vector<int> terrainHeight(m_width);
    double walk = 0;
    for (int x = 0; x < m_width; ++x)
    {
        const double noise_scale_1 = m_random.GetNoise(x * scale, scale * m_random.Next());
        const double noise_scale_2 = m_random.GetNoise(x * scale / 2, scale * m_random.Next()) / 2;
        const double noise_scale_4 = m_random.GetNoise(x * scale / 4, scale * m_random.Next()) / 4;

        const double noise = noise_scale_1 + noise_scale_2 + noise_scale_4;

        const double dist = std::abs(x - static_cast<int>(m_width) / 2);

        double amp = 0;
        if (dist > 30 && dist <= 60){
            amp = (dist - 30) / 30;
        } else if (dist > 60){
            amp = 1;
        }

        const double offset = noise * amp * amplitude;

        terrainHeight[x] = baseHeight + offset;
    }
    return terrainHeight;
}

void WorldGenerator::GenerateLayers(std::vector<int> dirtLevel, std::vector<int> stoneLevel, std::vector<int> ashLevel)
{
    for (int x = 0; x < m_width; ++x)
    {
        int dirt = dirtLevel[x];
        int stone = stoneLevel[x];
        int ash = ashLevel[x];
        for (int y = 0; y < dirt; ++y)
        {
            m_tiles[x + m_width * y] = Tile{Tile::Type::Air};
        }
        m_tiles[x + m_width * dirt] = Tile{Tile::Type::Grass};
        for (int y = dirt + 1; y < stone; ++y)
        {
            m_tiles[x + m_width * y] = Tile{Tile::Type::Dirt};
        }
        for (int y = stone; y < ash; ++y)
        {
            m_tiles[x + m_width * y] = Tile{Tile::Type::Stone};
        }
        for (int y = ash; y < m_height; ++y)
        {
            m_tiles[x + m_width * y] = Tile{Tile::Type::Ash};
        }
    }
}

constexpr double desertScale = 1;
constexpr double desertAmplitude = 30;
void WorldGenerator::GenerateDeserts(int baseHeight, std::vector<int> surfaceLevel)
{
    int height = baseHeight - 10;
    const int rand1 = m_random.Next();
    const int rand2 = m_random.Next();
    for (int x = 0; x < m_width; ++x) {
        // Use a sine wave for the base noise level with random offset. This will remove tiny deserts. Keep secondary noise and dist for no desert at spawn
        const int noise_scale_1 = floor(m_random.GetNoise(x * desertScale, rand1) * desertAmplitude);
        const int noise_scale_2 = floor(m_random.GetNoise(x * desertScale / 2, rand2 / 2) * desertAmplitude / 2);
        int depth = height + noise_scale_1 + noise_scale_2;
        const double dist = std::abs(x - static_cast<int>(m_width) / 2);
        if (dist < 100) {
            depth -= dist / 50;
        }
        if (depth < surfaceLevel[x]) { continue; }
        for (int y = surfaceLevel[x]; y < depth; ++y) {
            m_tiles[x + m_width * y] = Tile{Tile::Type::Sand};
        }
    }
}

constexpr double sandPileScale = 1.6;
constexpr double sandPileCutoff = 0.85;
void WorldGenerator::GenerateSandPiles(int start, int end)
{
    end += 40;
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = start; y < end; ++y)
        {
            double noise = m_random.GetNoise(x * sandPileScale, y * sandPileScale);
            if (y - start <= 5){
                double proximity = 5 - (y - start);
                noise -= proximity / 30;
            } else if (end - y <= 5){
                double proximity = 6 - (end - y);
                noise -= proximity / 30;
            }
            if (noise > sandPileCutoff)
            {
                m_tiles[x + m_width * y] = Tile{Tile::Type::Sand};
            }
        }
    }
}

constexpr double surfaceStoneScale = 10;
constexpr double surfaceStoneCutoff = 0.75;
void WorldGenerator::GenerateSurface(std::vector<int> start, std::vector<int> end)
{
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = start[x]; y < end[x]; ++y)
        {
            double noise = m_random.GetNoise(x * surfaceStoneScale, y * surfaceStoneScale);
            if (noise > surfaceStoneCutoff)
            {
                int pos = x + m_width * y;
                switch (m_tiles[pos].type) {
                    case Tile::Type::Dirt:
                        m_tiles[pos] = Tile{Tile::Type::Stone};
                    default:
                        break;
                };
            }
        }
    }
}

constexpr double undergroundStoneScale = 22;
constexpr double undergroundStoneCutoff = 0.4;
void WorldGenerator::GenerateUnderground(std::vector<int> start, std::vector<int> end)
{
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = start[x]; y < end[x]; ++y)
        {
            const double noise_scale_1 = m_random.GetNoise(x * undergroundStoneScale, y * undergroundStoneScale);
            const double noise_scale_2 = m_random.GetNoise(x * undergroundStoneScale / 2, y * undergroundStoneScale / 2) / 2;
            const double noise_scale_4 = m_random.GetNoise(x * undergroundStoneScale / 4, y * undergroundStoneScale / 4) / 4;

            const double noise = noise_scale_1 + noise_scale_2 + noise_scale_4;
            if (noise > undergroundStoneCutoff)
            {
                m_tiles[x + m_width * y] = Tile{Tile::Type::Stone};
            }
        }
    }
}

constexpr double cavernDirtScale = 16;
constexpr double cavernDirtCutoff = 0.65;
void WorldGenerator::GenerateCavern(std::vector<int> start, std::vector<int> end)
{
    int offset = m_random.Next();
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = start[x]; y < end[x]; ++y)
        {
            const double noise_scale_1 = m_random.GetNoise(x * cavernDirtScale, y * cavernDirtScale);
            const double noise_scale_2 = m_random.GetNoise(x * cavernDirtScale / 2, y * cavernDirtScale / 2) / 2;
            const double noise_scale_4 = m_random.GetNoise(x * cavernDirtScale / 4, y * cavernDirtScale / 4) / 4;
            
            const double noise = noise_scale_1 + noise_scale_2 + noise_scale_4;
            
            const double cutoff_noise = m_random.GetNoise(x * 2, y * 2) / 4;
            if (noise > cavernDirtCutoff + cutoff_noise)
            {
                m_tiles[x + m_width * y] = Tile{Tile::Type::Dirt};
            }
        }
    }
}

constexpr double clayScale = 7;
constexpr double clayCutoff1 = 0.8;
constexpr double clayCutoff2 = 0.9;
void WorldGenerator::GenerateClay(std::vector<int> start, std::vector<int> mid, std::vector<int> end)
{
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = start[x] + 5; y < mid[x] + 10; ++y)
        {
            double n = m_random.GetNoise(x * clayScale, y * clayScale);
            if (n < clayCutoff1)
            {
                continue;
            }
            int pos = x + m_width * y;
                switch (m_tiles[pos].type) {
                    case Tile::Type::Dirt:
                    case Tile::Type::Stone:
                        m_tiles[pos] = Tile{Tile::Type::Clay};
                    default:
                        break;
                };
        }
        for (int y = mid[x] + 10; y < end[x] + 30; ++y)
        {
            double n = m_random.GetNoise(x * clayScale, y * clayScale);
            if (n < clayCutoff2)
            {
                continue;
            }
            m_tiles[x + m_width * y] = Tile{Tile::Type::Clay};
        }
    }
}

constexpr double caveScale = 5;
constexpr double caveCutoff = 0.65;
void WorldGenerator::GenerateCaves(std::vector<int> undergroundStart)
{
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = undergroundStart[x]; y < m_height; ++y)
        {
            double noise_scale_1 = m_random.GetNoise(x * caveScale * 1.5, y * caveScale);
            double noise_scale_2 = m_random.GetNoise(x * caveScale, y * caveScale / 1.5) / 2;

            double noise = noise_scale_1 + noise_scale_2;
            if (noise > caveCutoff)
            {
                // Some caves should be water, some lava, and the rest air. How to disinguish caves?
                m_tiles[x + m_width * y] = Tile{Tile::Type::Air};
            }
        }
    }
}

void WorldGenerator::GenerateEntranceCaves(std::vector<int> surface)
{

}

void WorldGenerator::GenerateLargeCaves(std::vector<int> cavernStart)
{

}

World WorldGenerator::Finish()
{
    return World{std::move(m_tiles), m_width, m_height};
}