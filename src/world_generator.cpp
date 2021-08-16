#include "world_generator.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>

// Constructor
WorldGenerator::WorldGenerator(WorldSize size, std::uint64_t seed) : m_random{seed}, m_size{size}
{
    switch (size)
    {
    case WorldSize::Tiny:    // Old Mobile version only -- for testing whole world
        m_width = World::WIDTH_TINY;    // 1743
        m_height = World::HEIGHT_TINY;    // 893
        break;    // 1,575,000 tiles
    case WorldSize::Small:
        m_width = World::WIDTH_SMALL;    // 4117
        m_height = World::HEIGHT_SMALL;    // 1117
        break;    // 5,040,000 tiles
    case WorldSize::Medium:
        m_width = World::WIDTH_MEDIUM;    // 6317
        m_height = World::HEIGHT_MEDIUM;    // 1717
        break;    // 11,520,000 tiles
    case WorldSize::Large:
        m_width = World::WIDTH_LARGE;    // 8317
        m_height = World::HEIGHT_LARGE;    // 2317
        break;    // 20,160,000 tiles
    }
    m_tiles.resize(m_width * m_height, Tile());
}

// Tile Functions, Terrain and Random Height Functions
#pragma region Main Functions
// Own Function to Set Tiles because Tile Class most likely will change often
void WorldGenerator::SetTile(int x, int y, Tile::Type type)
{
    m_tiles[x + m_width * y].m_type = type;
}
void WorldGenerator::SetWall(int x, int y, Tile::Wall wall)
{
    m_tiles[x + m_width * y].m_wall = wall;
}
void WorldGenerator::SetLiquid(int x, int y, Tile::Liquid liquid)
{
    m_tiles[x + m_width * y].m_liquid = liquid;
}
void WorldGenerator::SetDepth(int x, int y, Tile::Depth depth)
{
    m_tiles[x + m_width * y].m_depth = depth;
}
bool WorldGenerator::IsTile(int x, int y, Tile::Type type)
{
    return m_tiles[x + m_width * y].m_type == type;
}
bool WorldGenerator::IsWall(int x, int y, Tile::Wall wall)
{
    return m_tiles[x + m_width * y].m_wall == wall;
}
bool WorldGenerator::IsLiquid(int x, int y, Tile::Liquid liquid)
{
    return m_tiles[x + m_width * y].m_liquid == liquid;
}

void WorldGenerator::FillBlob(
    int x, int y, Tile::Type type, double radius, double variation, bool replaceAir, bool overrideBlocks)
{
    int r = static_cast<int>(radius / 2);
    for (int i = x - r; i < x + r; ++i)
    {
        for (int j = y - r; j < y + r; ++j)
        {
            double distance = std::sqrt(std::pow(i - x, 2) + std::pow(j - y, 2));
            distance *= 2 / radius;
            double rand = m_random.GetDouble(0, variation);

            // Distance is the distance from 0 (center) to 1 (max radius)
            // Rand is a random value based on variation
            double check = distance + rand;
            bool isAir = IsTile(i, j, Tile::Type::Air);
            if (replaceAir && isAir || overrideBlocks && !isAir)
            {
                if (check < 1)
                {
                    SetTile(i, j, type);
                }
            }
        }
    }
}

// Helper Functions
int WorldGenerator::RandomHeight(double min, double max)
{
    return static_cast<int>(static_cast<double>(m_height) * m_random.GetDouble(min, max));
}

std::vector<int> WorldGenerator::RandomTerrain(int minHeight, int maxHeight, double amplitude)
{
    std::vector<int> terrainHeight(m_width);

    const int bounds = (minHeight - maxHeight) / 3;
    const std::uint64_t r = m_random.Next();

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
            constexpr int GOAL_TIMER_MIN = 40;
            constexpr int GOAL_TIMER_MAX = 120;
            goalTimer = m_random.GetInt(GOAL_TIMER_MIN, GOAL_TIMER_MAX);
            double change = m_random.GetDouble(-amplitude * (2 + trend), amplitude * (2 - trend));
            constexpr int HEIGHT_CORRECTION_DELTA = 15;
            if (goal + change < minHeight)
            {
                change += HEIGHT_CORRECTION_DELTA;
            }
            if (goal + change > maxHeight)
            {
                change -= HEIGHT_CORRECTION_DELTA;
            }
            goal += change;
            // keep track of whether ground moves up or down in a row. Used to modify chances
            trend += goal > height ? 1 : -1;
        }
        // Velocity moves towards the goal, only if velocity is small enough
        constexpr int VELOCITY_LERP_MULTIPLIER = 10;
        constexpr double LERP_TIME = 0.01;
        velocity = std::lerp(velocity, goal - height - velocity * VELOCITY_LERP_MULTIPLIER, LERP_TIME);
        height += velocity;

        // Small noise to add to the height walk
        const double noise = m_random.GetNoise(x, static_cast<int>(r)) * amplitude;

        if (height < minHeight)
        {
            height = minHeight;
        }
        if (height > maxHeight)
        {
            height = maxHeight;
        }
        terrainHeight[x] = static_cast<int>(height + noise);
    }

    return terrainHeight;
}
#pragma endregion Main Functions

std::size_t WorldGenerator::GetWidth() const
{
    return m_width;
}

std::size_t WorldGenerator::GetHeight() const
{
    return m_height;
}

// World Setup
#pragma region WorldSetup
void WorldGenerator::GenerateDepthLevels(int surface, int cavern, int underworld)
{
    const int space = static_cast<int>(surface * 0.35);
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = 0; y < space; ++y)
        {
            SetDepth(x, y, Tile::Depth::Space);
        }
        for (int y = space; y < surface; ++y)
        {
            SetDepth(x, y, Tile::Depth::Overworld);
        }
        for (int y = surface; y < cavern; ++y)
        {
            SetDepth(x, y, Tile::Depth::Underground);
        }
        for (int y = cavern; y < underworld; ++y)
        {
            SetDepth(x, y, Tile::Depth::Cavern);
        }
        for (int y = underworld; y < m_height; ++y)
        {
            SetDepth(x, y, Tile::Depth::Underworld);
        }
    }
}

void WorldGenerator::GenerateLayers(std::vector<int> dirtTerrain, std::vector<int> stoneTerrain, int ash)
{
    for (int x = 0; x < m_width; ++x)
    {
        const int dirt = dirtTerrain[x];
        const int stone = stoneTerrain[x];
        Tile tile;
        for (int y = 0; y < dirt; ++y)
        {
            SetTile(x, y, Tile::Type::Air);
        }
        SetTile(x, dirt, Tile::Type::Grass);
        for (int y = dirt + 1; y < stone; ++y)
        {
            SetTile(x, y, Tile::Type::Dirt);
        }
        for (int y = stone; y < ash; ++y)
        {
            SetTile(x, y, Tile::Type::Stone);
        }
        for (int y = ash; y < m_height; ++y)
        {
            SetTile(x, y, Tile::Type::Ash);
        }
    }
}

int WorldGenerator::ComputeStartCoordinate(int side)
{
    constexpr int WORLD_START_OFFSET = 50;
    if (side % 2 == 0)
    {
        // Left side of world
        return m_random.GetInt(WORLD_START_OFFSET, static_cast<int>(m_width / 2) - WORLD_START_OFFSET * 2);
    }

    // Right side of world
    return m_random.GetInt(
        static_cast<int>(m_width / 2) + WORLD_START_OFFSET * 2, static_cast<int>(m_width) - WORLD_START_OFFSET);
}

int WorldGenerator::ComputeWithinUsableArea(const std::vector<int>& surfaceTerrain, int side, int size, Tile::Type mask)
{
    if (mask == Tile::Type::Air)
    {
        return ComputeStartCoordinate(side);
    }
    while (true)
    {
        int start = ComputeStartCoordinate(side);
        if (!(IsTile(start, surfaceTerrain[start], mask) || IsTile(start + size, surfaceTerrain[start + size], mask)))
        {
            return start;
        }
    }
}

void WorldGenerator::GenerateSurfaceTunnels(const std::vector<int>& surfaceTerrain)
{
    const int tunnelCount = m_random.GetInt(6, 10);
    const std::uint64_t r1 = m_random.Next();
    const std::uint64_t r2 = m_random.Next();
    for (int i = 0; i < tunnelCount; ++i)
    {
        constexpr int TUNNEL_SIZE_MIN = 30;
        constexpr int TUNNEL_SIZE_MAX = 50;
        int size = m_random.GetInt(TUNNEL_SIZE_MIN, TUNNEL_SIZE_MAX);
        int start = ComputeWithinUsableArea(surfaceTerrain, i, size);

        const int spacing = 6;
        for (int x = start; x < start + size; ++x)
        {
            constexpr double NOISE_OFFSET = 1.25;
            constexpr double TUNNEL_NOISE_SCALE = 1.5;
            constexpr int TUNNEL_OFFSET = 1;
            int above =
                static_cast<int>((m_random.GetNoise(x, r1) - NOISE_OFFSET) * TUNNEL_NOISE_SCALE - TUNNEL_OFFSET);
            int below =
                static_cast<int>((m_random.GetNoise(x, r2) + NOISE_OFFSET) * TUNNEL_NOISE_SCALE + TUNNEL_OFFSET);

            if (x == start || x == start + size - 1)
            {
                above++;
                below--;
            }
            int height = surfaceTerrain[x] - spacing;
            for (int y = height + above; y < height + below; ++y)
            {
                if (y == height + above || y == height + below - 1)
                {
                    SetTile(x, y, Tile::Type::Grass);
                }
                else
                {
                    SetTile(x, y, Tile::Type::Dirt);
                }
            }
            for (int y = height + below; y < surfaceTerrain[x]; ++y)
            {
                SetWall(x, y, Tile::Wall::Dirt);
            }
        }
    }
}

void WorldGenerator::GenerateSandDesert(const std::vector<int>& surfaceTerrain)
{
    const int desertCount = m_random.GetInt(6, 10);
    for (int i = 0; i < desertCount; ++i)
    {
        constexpr int DESERT_SIZE_MIN = 30;
        constexpr int DESERT_SIZE_MAX = 70;
        int size = m_random.GetInt(DESERT_SIZE_MIN, DESERT_SIZE_MAX);
        int start = ComputeWithinUsableArea(surfaceTerrain, i, size);

        constexpr int DESERT_MAX_OFFSET = 10;
        constexpr int DESERT_MAX_OFFSET_CORRECTION = 2;

        int depth = surfaceTerrain[start] + DESERT_MAX_OFFSET_CORRECTION;
        for (int x = start; x < start + size; ++x)
        {
            if (x < start + DESERT_MAX_OFFSET)
            {
                // Left side of desert, go deeper
                depth += m_random.GetInt(0, DESERT_MAX_OFFSET_CORRECTION);
            }
            else if (x > start + size - DESERT_MAX_OFFSET)
            {
                // Right side of desert, go shallower
                depth -= m_random.GetInt(0, DESERT_MAX_OFFSET_CORRECTION);
            }
            else
            {
                // Middle of desert, random walk
                depth += m_random.GetInt(-1, 1);
            }

            if (depth < surfaceTerrain[x] + DESERT_MAX_OFFSET_CORRECTION)
            {
                // Don't go too shallow
                depth += DESERT_MAX_OFFSET_CORRECTION;
            }

            for (int y = surfaceTerrain[x]; y < depth; ++y)
            {
                SetTile(x, y, Tile::Type::Sand);
            }
        }
    }
}

constexpr double SAND_PILE_SCALE = 1.6;
constexpr double SAND_PILE_CUTOFF = 0.85;
void WorldGenerator::GenerateSandPiles(int dirtLevel, const std::vector<int>& rockHeights)
{
    int mid = dirtLevel;
    const auto& end = rockHeights;
    // Generate Sand Piles
    for (int x = 0; x < m_width; ++x)
    {
        int bottom = end[x] + 40;
        for (int y = mid; y < bottom; ++y)
        {
            double noise = m_random.GetNoise(x * SAND_PILE_SCALE, y * SAND_PILE_SCALE);
            if (y - mid <= SAND_PILE_MAX_OFFSET)
            {
                double proximity = 5 - (y - mid);
                noise -= proximity / 30;
            }
            else if (bottom - y <= 5)
            {
                double proximity = 6 - (bottom - y);
                noise -= proximity / 30;
            }
            if (noise > SAND_PILE_CUTOFF)
            {
                SetTile(x, y, Tile::Type::Sand);
            }
        }
    }
}

static int AnthillCount(WorldSize size)
{
    switch (size)
    {
    case WorldSize::Medium:
        return 3;
    case WorldSize::Large:
        return 4;
    default:
        return 2;
    }
}

std::vector<int> WorldGenerator::GenerateAnthills(const std::vector<int>& surfaceTerrain)
{
    int anthillCount = AnthillCount(m_size);

    std::vector<int> anthillCavePositions(anthillCount * 2);

    for (int i = 0; i < anthillCount; ++i)
    {
        constexpr int ANTHILL_SIZE_MIN = 40;
        constexpr int ANTHILL_SIZE_MAX = 60;
        int size = m_random.GetInt(ANTHILL_SIZE_MIN, ANTHILL_SIZE_MAX);
        int start = ComputeWithinUsableArea(surfaceTerrain, i, size, Tile::Type::Sand);

        for (int x = start; x < start + size; ++x)
        {
            const int max = static_cast<int>(size) / 2;
            int dist = std::abs((x - start) - max);
            dist *= dist;
            dist /= (max / 2);
            dist = std::min(max, dist);

            const int base = surfaceTerrain[x];
            const int high = surfaceTerrain[x] - (max - dist);
            if (high > base)
            {
                break;
            }
            for (int y = high; y <= base; ++y)
            {
                if (y == high)
                {
                    SetTile(x, y, Tile::Type::Grass);
                }
                else
                {
                    SetTile(x, y, Tile::Type::Dirt);
                }
            }
        }

        const int mid = start + size / 2;
        anthillCavePositions[i * 2] = mid;
        anthillCavePositions[i * 2 + 1] = surfaceTerrain[mid] - size / 4;
    }

    return anthillCavePositions;
}

constexpr double SURFACE_STONE_SCALE = 10;
constexpr double SURFACE_STONE_CUTOFF = 0.75;
void WorldGenerator::GenerateSurfaceStone(const std::vector<int>& start, const std::vector<int>& end)
{
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = start[x]; y < end[x]; ++y)
        {
            double noise = m_random.GetNoise(x * SURFACE_STONE_SCALE, y * SURFACE_STONE_SCALE);
            if (noise > SURFACE_STONE_CUTOFF)
            {
                int pos = x + m_width * y;
                switch (m_tiles[pos].m_type)
                {
                case Tile::Type::Dirt:
                    SetTile(x, y, Tile::Type::Stone);
                default:
                    break;
                };
            }
        }
    }
}

constexpr double UNDERGROUND_STONE_SCALE = 22;
constexpr double UNDERGROUND_STONE_CUTOFF = 0.4;
void WorldGenerator::GenerateUndergroundStone(const std::vector<int>& start, const std::vector<int>& end)
{
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = start[x]; y < end[x]; ++y)
        {
            const double noiseScale1 = m_random.GetNoise(x * UNDERGROUND_STONE_SCALE, y * UNDERGROUND_STONE_SCALE);
            const double noiseScale2 =
                m_random.GetNoise(x * UNDERGROUND_STONE_SCALE / 2, (y * UNDERGROUND_STONE_SCALE / 2) / 2);
            const double noiseScale4 =
                m_random.GetNoise(x * UNDERGROUND_STONE_SCALE / 4, (y * UNDERGROUND_STONE_SCALE / 4) / 4);

            const double noise = noiseScale1 + noiseScale2 + noiseScale4;
            if (noise > UNDERGROUND_STONE_CUTOFF)
            {
                SetTile(x, y, Tile::Type::Stone);
            }
        }
    }
}

constexpr double CAVERN_DIRT_SCALE = 16;
constexpr double CAVERN_DIRT_CUTOFF = 0.65;
void WorldGenerator::GenerateCavernDirt(const std::vector<int>& start, int end)
{
    int offset = m_random.Next();
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = start[x]; y < end; ++y)
        {
            const double noiseScale1 = m_random.GetNoise(x * CAVERN_DIRT_SCALE, y * CAVERN_DIRT_SCALE + offset);
            const double noiseScale2 =
                m_random.GetNoise(x * CAVERN_DIRT_SCALE / 2, y * CAVERN_DIRT_SCALE / 2) / 2 + offset;
            const double noiseScale4 =
                m_random.GetNoise(x * CAVERN_DIRT_SCALE / 4, y * CAVERN_DIRT_SCALE / 4) / 4 + offset;

            const double noise = noiseScale1 + noiseScale2 + noiseScale4;

            const double cutoffNoise = m_random.GetNoise(x * 2, y * 2) / 4;
            if (noise > CAVERN_DIRT_CUTOFF + cutoffNoise)
            {
                SetTile(x, y, Tile::Type::Dirt);
            }
        }
    }
}
#pragma endregion

// Caves
#pragma region Caves
void WorldGenerator::GenerateCaves(std::vector<int> undergroundStart)
{
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = undergroundStart[x]; y < m_height; ++y)
        {
            double noise_scale_1 = m_random.GetNoise(x * CAVE_SCALE_HORIZONTAL, y * CAVE_SCALE);
            double noise_scale_2 = m_random.GetNoise(x * CAVE_SCALE, y * CAVE_SCALE_VERTICAL) / 2;

            double noise = noise_scale_1 + noise_scale_2;
            if (noise > CAVE_CUTOFF)
            {
                // Some caves should be water, some lava, and the rest air. How to disinguish caves?
                SetTile(x, y, Tile::Type::Air);
            }
        }
    }
}

void WorldGenerator::GenerateEntranceCaves(const std::vector<int>& surface)
{
}

constexpr double LARGE_CAVE_SCALE = 4;
constexpr double LARGE_CAVE_CUTOFF = 0.7;
void WorldGenerator::GenerateLargeCaves(const std::vector<int>& cavernStart)
{
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = cavernStart[x]; y < m_height; ++y)
        {
            double noise_scale_1 = m_random.GetNoise(x * LARGE_CAVE_SCALE, y * LARGE_CAVE_SCALE);
            double noise_scale_2 = m_random.GetNoise(x * LARGE_CAVE_SCALE / 2, y * LARGE_CAVE_SCALE / 2) / 2;

            double noise = noise_scale_1 + noise_scale_2;
            if (noise > LARGE_CAVE_CUTOFF)
            {
                // Some caves should be water, some lava, and the rest air. How to disinguish caves?
                SetTile(x, y, Tile::Type::Air);
            }
        }
    }
}
#pragma endregion Caves

// Scattered Blocks (Clay, Grass, Mud, Silt)
#pragma region Scattered Blocks
constexpr double CLAY_SCALE = 7;
constexpr double CLAY_CUTOFF1 = 0.8;
constexpr double CLAY_CUTOFF2 = 0.9;
void WorldGenerator::GenerateClay(
    const std::vector<int>& start, const std::vector<int>& mid, const std::vector<int>& end)
{
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = start[x] + 5; y < mid[x] + 10; ++y)
        {
            double n = m_random.GetNoise(x * CLAY_SCALE, y * CLAY_SCALE);
            if (n < CLAY_CUTOFF1)
            {
                continue;
            }
            int pos = x + m_width * y;
            switch (m_tiles[pos].m_type)
            {
            case Tile::Type::Dirt:
            case Tile::Type::Stone:
                SetTile(x, y, Tile::Type::Clay);
            default:
                break;
            };
        }
        for (int y = mid[x] + 10; y < end[x] + 30; ++y)
        {
            double n = m_random.GetNoise(x * CLAY_SCALE, y * CLAY_SCALE);
            if (n < CLAY_CUTOFF2)
            {
                continue;
            }
            SetTile(x, y, Tile::Type::Clay);
        }
    }
}

constexpr double CHANCE_OF_GRASS = 0.025;
void WorldGenerator::GenerateGrass(const std::vector<int>& start, int end)
{
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = start[x]; y < end; y++)
        {
            if (IsTile(x, y, Tile::Type::Dirt), CHANCE_OF_GRASS > m_random.GetDouble(0, 1))
            {
                SetTile(x, y, Tile::Type::Grass);
            }
        }
    }
}

constexpr double MUD_SCALE = 6;
constexpr double MUD_CUTOFF = 0.94;
void WorldGenerator::GenerateMud(int start, int end)
{
    const int r = m_random.Next();
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = start; y < end; y++)
        {
            const double noise = m_random.GetNoise(x * MUD_SCALE, y * MUD_SCALE / 2.5 + r);
            if (MUD_CUTOFF < noise && !IsTile(x, y, Tile::Type::Air))
            {
                SetTile(x, y, Tile::Type::Mud);
            }
        }
    }
}

constexpr double SILT_SCALE = 7;
constexpr double SILT_CUTOFF = 0.87;
void WorldGenerator::GenerateSilt(int start, int end)
{
    const int r = m_random.Next();
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = start; y < end; y++)
        {
            const double noise = m_random.GetNoise(x * SILT_SCALE, y * SILT_SCALE + r);
            if (SILT_CUTOFF < noise && !IsTile(x, y, Tile::Type::Air) && !IsTile(x, y + 1, Tile::Type::Air))
            {
                SetTile(x, y, Tile::Type::Silt);
            }
        }
    }
}
#pragma endregion Scattered Blocks

// Biomes Part 1
#pragma region Biomes Part 1
#pragma endregion Biomes Part 1

// Metals, Gems, and Webs
#pragma region Shinies
void WorldGenerator::GenerateMetals(std::vector<int> surface, int underground, int cavern, int underworld)
{
    int cavernRadius = (underground + cavern) / 2 - underground;
    // COPPER
    for (int i = 0; i < static_cast<int>(static_cast<double>(m_tiles.size()) * 6E-05); ++i)
    {
        const int x = m_random.GetInt(0, m_width);
        const int y = m_random.GetInt(surface[x], underground);
        FillBlob(x, y, Tile::Type::Copper, m_random.GetDouble(3, 6), m_random.GetDouble(0.1, 0.4));
    }
    for (int i = 0; i < static_cast<int>(static_cast<double>(m_tiles.size()) * 8E-05); ++i)
    {
        const int x = m_random.GetInt(0, m_width);
        const int y = m_random.GetInt(underground, cavern + cavernRadius);
        FillBlob(x, y, Tile::Type::Copper, m_random.GetDouble(3, 7), m_random.GetDouble(0.1, 0.4));
    }
    for (int i = 0; i < static_cast<int>(static_cast<double>(m_tiles.size()) * 0.0002); ++i)
    {
        const int x = m_random.GetInt(0, m_width);
        const int y = m_random.GetInt(cavern - cavernRadius, underworld);
        FillBlob(x, y, Tile::Type::Copper, m_random.GetDouble(4, 9), m_random.GetDouble(0.1, 0.4));
    }
    // IRON
    for (int i = 0; i < static_cast<int>(static_cast<double>(m_tiles.size()) * 8E-05); ++i)
    {
        const int x = m_random.GetInt(0, m_width);
        const int y = m_random.GetInt(surface[x], underground);
        FillBlob(x, y, Tile::Type::Iron, m_random.GetDouble(3, 7), m_random.GetDouble(0.1, 0.4));
    }
    for (int i = 0; i < static_cast<int>(static_cast<double>(m_tiles.size()) * 0.0002); ++i)
    {
        const int x = m_random.GetInt(0, m_width);
        const int y = m_random.GetInt(underground, cavern + cavernRadius);
        FillBlob(x, y, Tile::Type::Iron, m_random.GetDouble(3, 6), m_random.GetDouble(0.1, 0.4));
    }
    for (int i = 0; i < static_cast<int>(static_cast<double>(m_tiles.size()) * 2.6E-05); ++i)
    {
        const int x = m_random.GetInt(0, m_width);
        const int y = m_random.GetInt(cavern - cavernRadius, underworld);
        FillBlob(x, y, Tile::Type::Iron, m_random.GetDouble(4, 9), m_random.GetDouble(0.1, 0.4));
    }
    // SILVER
    for (int i = 0; i < static_cast<int>(static_cast<double>(m_tiles.size()) * 2.6E-05); ++i)
    {
        const int x = m_random.GetInt(0, m_width);
        const int y = m_random.GetInt(underground, cavern + cavernRadius);
        FillBlob(x, y, Tile::Type::Silver, m_random.GetDouble(3, 6), m_random.GetDouble(0.1, 0.4));
    }
    for (int i = 0; i < static_cast<int>(static_cast<double>(m_tiles.size()) * 0.00015); ++i)
    {
        const int x = m_random.GetInt(0, m_width);
        const int y = m_random.GetInt(cavern - cavernRadius, underworld);
        FillBlob(x, y, Tile::Type::Silver, m_random.GetDouble(4, 9), m_random.GetDouble(0.1, 0.4));
    }
    // GOLD
    for (int i = 0; i < static_cast<int>(static_cast<double>(m_tiles.size()) * 0.00012); ++i)
    {
        const int x = m_random.GetInt(0, m_width);
        const int y = m_random.GetInt(cavern - cavernRadius, underworld);
        FillBlob(x, y, Tile::Type::Gold, m_random.GetDouble(4, 8), m_random.GetDouble(0.1, 0.4));
    }
}

void WorldGenerator::GenerateGems(int start, int end)
{
}

void WorldGenerator::GenerateWebs(int start, int end)
{
}
#pragma endregion Shinies

// Biomes Part 2
#pragma region Biomes Part 2
#pragma endregion Biomes Part 2

// Fixes
#pragma region Fixes
void WorldGenerator::GenerateAnthillCaves(std::vector<int> positions)
{
}

constexpr int CORRECTION_RADIUS = 16;
void WorldGenerator::FixGravitatingSand(const std::vector<int>& surface)
{
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = surface[x] - CORRECTION_RADIUS; y < surface[x] + CORRECTION_RADIUS; ++y)
        {
            if (IsTile(x, y, Tile::Type::Sand))
            {
                while (IsTile(x, y + 1, Tile::Type::Air))
                {
                    SetTile(x, ++y, Tile::Type::Sand);
                }
            }
        }
    }
}

void WorldGenerator::FixDirtWalls(const std::vector<int>& surface)
{
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = surface[x] - CORRECTION_RADIUS; y < surface[x] + CORRECTION_RADIUS; ++y)
        {
            if (!IsTile(x, y, Tile::Type::Air))
            {
                break;
            }
            else if (!IsWall(x, y, Tile::Wall::Air))
            {
                do
                {
                    SetWall(x, y, Tile::Wall::Air);
                    y++;
                } while (!IsWall(x, y, Tile::Wall::Air) && IsTile(x, y, Tile::Type::Air));
                break;
            }
        }
    }
}

void WorldGenerator::FixWaterOnSand(const std::vector<int>& surface)
{
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = surface[x] - CORRECTION_RADIUS; y < surface[x] + CORRECTION_RADIUS; ++y)
        {
            if (IsTile(x, y, Tile::Type::Sand))
            {
                SetLiquid(x, y, Tile::Liquid::None);
                while (IsLiquid(x, y - 1, Tile::Liquid::Water))
                {
                    SetLiquid(x, --y, Tile::Liquid::None);
                }
                break;
            }
        }
    }
}
#pragma endregion Fixes

// Biomes Part 3
#pragma region Biomes Part 3
#pragma endregion Biomes Part 3

// Clean Up World
#pragma region Clean Up
void WorldGenerator::SmoothWorld()
{
}

void WorldGenerator::SettleLiquids()
{
}

void WorldGenerator::AddWaterfalls()
{
}
#pragma endregion Clean Up

// Finalize World
World WorldGenerator::Finish()
{
    return World{std::move(m_tiles), m_width, m_height};
}