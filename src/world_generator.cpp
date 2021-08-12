#include "world_generator.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <math.h>
#include <stdio.h>

WorldGenerator::WorldGenerator(WorldSize size, std::uint64_t seed) : m_random{seed}
{
    m_size = size;
    switch (size)
    {
    case WorldSize::Tiny: //Old Mobile version only -- for testing whole world
        m_width = 1750; //1743
        m_height = 900; //893
        break; //1,575,000 tiles
    case WorldSize::Small:
        m_width = 4200;  //4117
        m_height = 1200; //1117
        break; //5,040,000 tiles
    case WorldSize::Medium:
        m_width = 6400;  //6317
        m_height = 1800; //1717
        break; //11,520,000 tiles
    case WorldSize::Large:
        m_width = 8400;  //8317
        m_height = 2400; //2317
        break; //20,160,000 tiles
    }
    m_tiles.resize(m_width * m_height);
}

int WorldGenerator::RandomHeight(double min, double max)
{
    return m_height * m_random.GetDouble(min, max);
}

std::vector<int> WorldGenerator::RandomTerrain(int minHeight, int maxHeight, double amplitude)
{
    std::vector<int> terrainHeight(m_width);

    const int bounds = (int) ((minHeight - maxHeight) / 3);
    const int r = m_random.Next();

    double height = m_random.GetInt(minHeight + bounds, maxHeight - bounds);
    double velocity = 0;
    double goal = height;
    int trend = 0;
    int goalTimer = 0;
    
    for (int x = 0; x < m_width; ++x)
    {
        if (--goalTimer <= 0) 
        {
            // move goal to new location within min-max bounds
            goalTimer = m_random.GetInt(40, 120);
            double change = m_random.GetDouble(-amplitude * (2 + trend), amplitude * (2 - trend));
            if (goal + change < minHeight) change += 15;
            if (goal + change > maxHeight) change -= 15;
            goal += change;
            // keep track of whether ground moves up or down in a row. Used to modify chances
            trend += goal > height ? 1 : -1;
        }
        // Velocity moves towards the goal, only if velocity is small enough
        velocity = std::lerp(velocity, goal - height - velocity * 10, 0.01);
        height += velocity;

        // Small noise to add to the height walk
        const double noise = m_random.GetNoise(x, r) * amplitude;

        if (height < minHeight) height = minHeight;
        if (height > maxHeight) height = maxHeight;
        terrainHeight[x] = (int) (height + noise);
    }

    return terrainHeight;
}

void WorldGenerator::GenerateLayers(std::vector<int> dirtLevel, std::vector<int> stoneLevel, int ashLevel)
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
        for (int y = dirt + 1; y < stone; ++y)
        {
            m_tiles[x + m_width * y] = Tile{Tile::Type::Dirt};
        }
        for (int y = stone; y < ashLevel; ++y)
        {
            m_tiles[x + m_width * y] = Tile{Tile::Type::Stone};
        }
        for (int y = ashLevel; y < m_height; ++y)
        {
            m_tiles[x + m_width * y] = Tile{Tile::Type::Ash};
        }
    }
}

void WorldGenerator::GenerateSurfaceTunnels(std::vector<int> dirtLevel)
{

}

constexpr double desertScale = 1;
constexpr double desertAmplitude = 30;
constexpr double sandPileScale = 1.6;
constexpr double sandPileCutoff = 0.85;
void WorldGenerator::GenerateSand(std::vector<int> top, int mid, std::vector<int> end)
{
    const int desertCount = m_random.GetInt(6, 10);
    for (int i = 0; i < desertCount; ++i) {
        int start;
        if (i % 2 == 0)
        {
            // Left side of world
            start = m_random.GetInt(50, m_width / 2 - 100);
        }
        else
        {
            // Right side of world
            start = m_random.GetInt(m_width / 2 + 50, m_width - 100);
        }
        int size = m_random.GetInt(30, 70);

        int depth = top[start] + 5;
        for (int x = start; x < start + size; ++x)
        {
            if (x < start + 10)
            {
                // Left side of desert, go deeper
                depth += m_random.GetInt(0, 2);
            }
            else if (x > start + size - 10)
            {
                // Right side of desert, go shallower
                depth += m_random.GetInt(-2, 0);
            }
            else
            {
                // Middle of desert, random walk
                depth += m_random.GetInt(-1, 1);
            }
            
            // Dont go too shallow
            if (depth < top[x] + 5) {
                depth += 2;
            }

            for (int y = top[x]; y < depth; ++y)
            {
                m_tiles[x + m_width * y] = Tile{Tile::Type::Sand};
            }
        }
    }
    // Generate Sand Piles
    for (int x = 0; x < m_width; ++x)
    {
        int bottom = end[x] + 40;
        for (int y = mid; y < bottom; ++y)
        {
            double noise = m_random.GetNoise(x * sandPileScale, y * sandPileScale);
            if (y - mid <= 5){
                double proximity = 5 - (y - mid);
                noise -= proximity / 30;
            } else if (bottom - y <= 5){
                double proximity = 6 - (bottom - y);
                noise -= proximity / 30;
            }
            if (noise > sandPileCutoff)
            {
                m_tiles[x + m_width * y] = Tile{Tile::Type::Sand};
            }
        }
    }
}

void WorldGenerator::GenerateAnthills(std::vector<int> surfaceTerrain)
{
    int anthillCount;
    switch (m_size)
    {
    default:
        anthillCount = 2;
        break;
    case WorldSize::Medium:
        anthillCount = 3;
        break;
    case WorldSize::Large:
        anthillCount = 4;
        break;
    }
    for (int i = 0; i < anthillCount; ++i) {
        int start;
        int size;
        do {
            // Left side of world
            if (i % 2 == 0) start = m_random.GetInt(50, m_width / 2 - 100);
            // Right side of world
            else start = m_random.GetInt(m_width / 2 + 50, m_width - 100);
            // Size of anthill
            size = m_random.GetInt(40, 60);
        } while (m_tiles[start + m_width * surfaceTerrain[start]].type == Tile::Type::Sand
                || m_tiles[start + size + m_width * surfaceTerrain[start + size]].type == Tile::Type::Sand);
        

        for (int x = start; x < start + size; ++x)
        {
            const int max = static_cast<int>(size) / 2;
            int dist = std::abs((x - start) - max);
            dist *= dist;
            dist /= (max / 2);
            dist = std::min(max, dist);
            
            const int base = surfaceTerrain[x];
            const int high = surfaceTerrain[x] - (max - dist);
            if (high > base) break;
            for (int y = high; y <= base; ++y)
            {
                if (y == high)
                {
                    m_tiles[x + m_width * y] = Tile{Tile::Type::Grass};
                }
                else
                {
                    m_tiles[x + m_width * y] = Tile{Tile::Type::Dirt};
                }
            }
        }
    }
}

constexpr double surfaceStoneScale = 10;
constexpr double surfaceStoneCutoff = 0.75;
void WorldGenerator::GenerateSurfaceStone(std::vector<int> start, std::vector<int> end)
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
void WorldGenerator::GenerateUndergroundStone(std::vector<int> start, std::vector<int> end)
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
void WorldGenerator::GenerateCavernDirt(std::vector<int> start, int end)
{
    int offset = m_random.Next();
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = start[x]; y < end; ++y)
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