#include "world_generator.hpp"
#include "vector_2.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#pragma region Class Functions
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

std::size_t WorldGenerator::GetHeight() const
{
    return m_height;
}
#pragma endregion

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

std::vector<int> WorldGenerator::RandomTerrain(int minHeight, int maxHeight, double amplitude, int timer)
{
    constexpr int TREND_ADJUST = 2;
    constexpr int HEIGHT_CORRECTION_DELTA = 15;
    constexpr int VELOCITY_LERP_MULTIPLIER = 10;
    constexpr double LERP_TIME = 0.01;

    const int goalTimerOffset = timer / 4;

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
            goalTimer = m_random.GetInt(timer - goalTimerOffset, timer + goalTimerOffset);
            double change = m_random.GetDouble(-amplitude * (TREND_ADJUST + trend), amplitude * (TREND_ADJUST - trend));
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

    return std::move(terrainHeight);
}
#pragma endregion Main Functions

// Terrain, Dirt, Stone, and Sand
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

void WorldGenerator::GenerateLayers(const std::vector<int>& dirtTerrain, const std::vector<int>& stoneTerrain, int ash)
{
    for (int x = 0; x < m_width; ++x)
    {
        const int dirt = dirtTerrain[x];
        const int stone = stoneTerrain[x];
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
    constexpr int TUNNEL_SIZE_MIN = 30;
    constexpr int TUNNEL_SIZE_MAX = 50;
    constexpr double NOISE_OFFSET = 1.25;
    constexpr double TUNNEL_NOISE_SCALE = 1.5;
    constexpr int TUNNEL_OFFSET = 1;

    const int tunnelCount = m_random.GetInt(6, 10);
    const int r1 = static_cast<int>(m_random.Next());
    const int r2 = static_cast<int>(m_random.Next());

    for (int i = 0; i < tunnelCount; ++i)
    {
        int size = m_random.GetInt(TUNNEL_SIZE_MIN, TUNNEL_SIZE_MAX);
        int start = ComputeWithinUsableArea(surfaceTerrain, i, size, Tile::Type::Sand);

        const int spacing = 6;
        for (int x = start; x < start + size; ++x)
        {
            int above =
                static_cast<int>((m_random.GetNoise(x, r1) - NOISE_OFFSET) * TUNNEL_NOISE_SCALE - TUNNEL_OFFSET);
            int below =
                static_cast<int>((m_random.GetNoise(x, r2) + NOISE_OFFSET) * TUNNEL_NOISE_SCALE + TUNNEL_OFFSET);

            int height = surfaceTerrain[x] - spacing;
            if (x == start || x == start + size - 1)
            {
                above++;
                below--;
            }
            else
            {
                for (int y = height + below; y < surfaceTerrain[x]; ++y)
                {
                    SetWall(x, y, Tile::Wall::Dirt);
                }
            }
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
        }
    }
}

void WorldGenerator::GenerateSandDesert(const std::vector<int>& surfaceTerrain)
{
    constexpr int DESERT_SIZE_MIN = 30;
    constexpr int DESERT_SIZE_MAX = 70;
    constexpr int DESERT_MAX_OFFSET = 10;
    constexpr int DESERT_MAX_OFFSET_CORRECTION = 2;

    const int desertCount = m_random.GetInt(6, 10);

    for (int i = 0; i < desertCount; ++i)
    {
        int size = m_random.GetInt(DESERT_SIZE_MIN, DESERT_SIZE_MAX);
        int start = ComputeWithinUsableArea(surfaceTerrain, i, size);


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
                // Fill with Sand
                SetTile(x, y, Tile::Type::Sand);
            }
        }
    }
}

void WorldGenerator::GenerateSandPiles(int dirtLevel, const std::vector<int>& rockHeights)
{
    constexpr double SAND_PILE_SCALE = 1.6;
    constexpr double SAND_PILE_CUTOFF = 0.85;
    constexpr int SAND_PILE_OVERCORRECTION = 40;
    constexpr int SAND_PILE_MAX_OFFSET = 5;
    constexpr int SAND_PILE_PROXIMITY_THRESHOLD = 30;

    int mid = dirtLevel;
    const auto& end = rockHeights;
    
    for (int x = 0; x < m_width; ++x)
    {
        int bottom = end[x] + SAND_PILE_OVERCORRECTION;
        for (int y = mid; y < bottom; ++y)
        {
            double noise = m_random.GetNoise(x * SAND_PILE_SCALE, y * SAND_PILE_SCALE);
            if (y - mid <= SAND_PILE_MAX_OFFSET)
            {
                double proximity = SAND_PILE_MAX_OFFSET - (y - mid);
                noise -= proximity / SAND_PILE_PROXIMITY_THRESHOLD;
            }
            else if (bottom - y <= SAND_PILE_MAX_OFFSET)
            {
                double proximity = SAND_PILE_MAX_OFFSET + 1 - (bottom - y);
                noise -= proximity / SAND_PILE_PROXIMITY_THRESHOLD;
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
    constexpr int ANTHILL_SIZE_MIN = 40;
    constexpr int ANTHILL_SIZE_MAX = 60;

    int anthillCount = AnthillCount(m_size);

    std::vector<int> anthillCavePositions(static_cast<size_t>(anthillCount * 2));

    for (int i = 0; i < anthillCount; ++i)
    {
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
        anthillCavePositions[static_cast<size_t>(i) * 2] = mid;
        anthillCavePositions[i * 2 + 1] = surfaceTerrain[mid] - size / 4;
    }

    return std::move(anthillCavePositions);
}

void WorldGenerator::GenerateSurfaceStone(const std::vector<int>& start, const std::vector<int>& end)
{
    constexpr double SURFACE_STONE_SCALE = 10;
    constexpr double SURFACE_STONE_CUTOFF = 0.75;

    const int offset = static_cast<int>(m_random.Next());

    for (int x = 0; x < m_width; ++x)
    {
        for (int y = start[x]; y < end[x]; ++y)
        {
            double noise = m_random.GetNoise(x * SURFACE_STONE_SCALE, y * SURFACE_STONE_SCALE + offset);
            if (noise > SURFACE_STONE_CUTOFF)
            {
                if (IsTile(x, y, Tile::Type::Dirt))
                {
                    SetTile(x, y, Tile::Type::Stone);
                }
            }
        }
    }
}

void WorldGenerator::GenerateUndergroundStone(const std::vector<int>& start, const std::vector<int>& end)
{
    constexpr double UNDERGROUND_STONE_SCALE = 22;
    constexpr double UNDERGROUND_STONE_CUTOFF = 0.4;
    
    const int offset = static_cast<int>(m_random.Next());

    for (int x = 0; x < m_width; ++x)
    {
        for (int y = start[x]; y < end[x]; ++y)
        {
            const double noiseScale1 = m_random.GetNoise(x * UNDERGROUND_STONE_SCALE, y * UNDERGROUND_STONE_SCALE + offset);
            const double noiseScale2 =
                m_random.GetNoise(x * UNDERGROUND_STONE_SCALE / 2, (y * UNDERGROUND_STONE_SCALE / 2 + offset) / 2);
            const double noiseScale4 =
                m_random.GetNoise(x * UNDERGROUND_STONE_SCALE / 4, (y * UNDERGROUND_STONE_SCALE / 4 + offset) / 4);

            const double noise = noiseScale1 + noiseScale2 + noiseScale4;
            if (noise > UNDERGROUND_STONE_CUTOFF)
            {
                SetTile(x, y, Tile::Type::Stone);
            }
        }
    }
}

void WorldGenerator::GenerateCavernDirt(const std::vector<int>& start, int end)
{
    constexpr double CAVERN_DIRT_SCALE = 16;
    constexpr double CAVERN_DIRT_CUTOFF = 0.65;

    const int offset = static_cast<int>(m_random.Next());

    for (int x = 0; x < m_width; ++x)
    {
        for (int y = start[x]; y < end; ++y)
        {
            const double noiseScale1 = m_random.GetNoise(x * CAVERN_DIRT_SCALE, y * CAVERN_DIRT_SCALE + offset);
            const double noiseScale2 =
                m_random.GetNoise(x * CAVERN_DIRT_SCALE / 2, y * CAVERN_DIRT_SCALE / 2 + offset) / 2;
            const double noiseScale4 =
                m_random.GetNoise(x * CAVERN_DIRT_SCALE / 4, y * CAVERN_DIRT_SCALE / 4 + offset) / 4;

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

// Small, Large, and Entrance Caves
#pragma region Caves
void WorldGenerator::GenerateCaves(const std::vector<int>& undergroundStart)
{
    constexpr double CAVE_SCALE = 5;
    constexpr double CAVE_SCALE_HORIZONTAL = 7.5;
    constexpr double CAVE_SCALE_VERTICAL = 3.3;
    constexpr double CAVE_CUTOFF = 0.65;

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

void WorldGenerator::GenerateLargeCaves(const std::vector<int>& cavernStart)
{
    constexpr double LARGE_CAVE_SCALE = 4;
    constexpr double LARGE_CAVE_CUTOFF = 0.7;

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
void WorldGenerator::GenerateClay(
    const std::vector<int>& start, const std::vector<int>& mid, const std::vector<int>& end)
{
    constexpr double CLAY_SCALE = 7;
    constexpr double CLAY_CUTOFF1 = 0.8;
    constexpr double CLAY_CUTOFF2 = 0.9;
    constexpr int START_OFFSET = 5;
    constexpr int MID_OFFSET = 10;
    constexpr int END_OFFSET = 30;

    for (int x = 0; x < m_width; ++x)
    {
        for (int y = start[x] + START_OFFSET; y < mid[x] + MID_OFFSET; ++y)
        {
            double n = m_random.GetNoise(x * CLAY_SCALE, y * CLAY_SCALE);
            if (n < CLAY_CUTOFF1)
            {
                continue;
            }
            int pos = static_cast<int>(x + m_width * y);
            switch (m_tiles[pos].m_type)
            {
            case Tile::Type::Dirt:
            case Tile::Type::Stone:
                SetTile(x, y, Tile::Type::Clay);
            default:
                break;
            };
        }
        for (int y = mid[x] + MID_OFFSET; y < end[x] + END_OFFSET; ++y)
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

void WorldGenerator::GenerateGrass(const std::vector<int>& start, int end)
{
    constexpr double CHANCE_OF_GRASS = 0.025;

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

void WorldGenerator::GenerateMud(int start, int end)
{
    constexpr double MUD_SCALE_X = 6;
    constexpr double MUD_SCALE_Y = 2;
    constexpr double MUD_CUTOFF = 0.94;

    const int r = m_random.Next();
    
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = start; y < end; y++)
        {
            const double noise = m_random.GetNoise(x * MUD_SCALE_X, y * MUD_SCALE_Y + r);
            if (MUD_CUTOFF < noise && !IsTile(x, y, Tile::Type::Air))
            {
                SetTile(x, y, Tile::Type::Mud);
            }
        }
    }
}

void WorldGenerator::GenerateSilt(int start, int end)
{
    constexpr double SILT_SCALE = 7;
    constexpr double SILT_CUTOFF = 0.87;

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
void WorldGenerator::FillBlobAtRandomPosition(Vector2<int> horizontal, Vector2<int> vertical, Tile::Type type, Vector2<double> size, Vector2<double> variation)
{
    const int x = m_random.GetInt(horizontal);
    const int y = m_random.GetInt(vertical);
    const double s = m_random.GetDouble(size);
    const double v = m_random.GetDouble(variation);
    FillBlob(x, y, type, s, v);
}

void WorldGenerator::GenerateMetals(int surface, int underground, int cavern, int underworld)
{
    const Tile::Type copperType = m_random.Next() % 2 == 1 ? Tile::Type::Copper : Tile::Type::Tin;
    const Tile::Type ironType = m_random.Next() % 2 == 1 ? Tile::Type::Iron : Tile::Type::Lead;
    const Tile::Type silverType = m_random.Next() % 2 == 1 ? Tile::Type::Silver : Tile::Type::Tungsten;
    const Tile::Type goldType = m_random.Next() % 2 == 1 ? Tile::Type::Gold : Tile::Type::Platinum;

    const Vector2<int> worldWidth = Vector2<int> {0, static_cast<int>(m_width)};
    const int cavernRadius = (underground + cavern) / 2 - underground;
    const Vector2<int> surfaceHeight = Vector2<int> {surface, underground};
    const Vector2<int> undergroundHeight = Vector2<int> {underground, cavern + cavernRadius};
    const Vector2<int> cavernHeight = Vector2<int> {cavern - cavernRadius, underworld};

    constexpr double COPPER_SURFACE_AMOUNT                  = 6E-05;
    constexpr Vector2<double> COPPER_SURFACE_SIZE           = Vector2<double> {3, 6};
    constexpr Vector2<double> COPPER_SURFACE_VARIATION      = Vector2<double> {0.1, 0.4};
    constexpr double COPPER_UNDERGROUND_AMOUNT              = 8E-05;
    constexpr Vector2<double> COPPER_UNDERGROUND_SIZE       = Vector2<double> {3, 7};
    constexpr Vector2<double> COPPER_UNDERGROUND_VARIATION  = Vector2<double> {0.1, 0.4};
    constexpr double COPPER_CAVERN_AMOUNT                   = 0.0002;
    constexpr Vector2<double> COPPER_CAVERN_SIZE            = Vector2<double> {4, 9};
    constexpr Vector2<double> COPPER_CAVERN_VARIATION       = Vector2<double> {0.1, 0.4};
    
    const int copperSurfaceCount = static_cast<int>(static_cast<double>(m_tiles.size()) * COPPER_SURFACE_AMOUNT);
    const int copperUndergroundCount = static_cast<int>(static_cast<double>(m_tiles.size()) * COPPER_UNDERGROUND_AMOUNT);
    const int copperCavernCount = static_cast<int>(static_cast<double>(m_tiles.size()) * COPPER_CAVERN_AMOUNT);
    // COPPER
    for (int i = 0; i < copperSurfaceCount; ++i)
    {
        FillBlobAtRandomPosition(worldWidth, surfaceHeight, Tile::Type::Copper, COPPER_SURFACE_SIZE, COPPER_SURFACE_VARIATION);
    }
    for (int i = 0; i < copperUndergroundCount; ++i)
    {
        FillBlobAtRandomPosition(worldWidth, undergroundHeight, Tile::Type::Copper, COPPER_UNDERGROUND_SIZE, COPPER_UNDERGROUND_VARIATION);
    }
    for (int i = 0; i < copperCavernCount; ++i)
    {
        FillBlobAtRandomPosition(worldWidth, cavernHeight, Tile::Type::Copper, COPPER_CAVERN_SIZE, COPPER_CAVERN_VARIATION);
    }
    
    constexpr double IRON_SURFACE_AMOUNT                  = 8E-05;
    constexpr Vector2<double> IRON_SURFACE_SIZE           = Vector2<double> {3, 7};
    constexpr Vector2<double> IRON_SURFACE_VARIATION      = Vector2<double> {0.1, 0.4};
    constexpr double IRON_UNDERGROUND_AMOUNT              = 0.0002;
    constexpr Vector2<double> IRON_UNDERGROUND_SIZE       = Vector2<double> {3, 6};
    constexpr Vector2<double> IRON_UNDERGROUND_VARIATION  = Vector2<double> {0.1, 0.4};
    constexpr double IRON_CAVERN_AMOUNT                   = 2.6E-05;
    constexpr Vector2<double> IRON_CAVERN_SIZE            = Vector2<double> {4, 9};
    constexpr Vector2<double> IRON_CAVERN_VARIATION       = Vector2<double> {0.1, 0.4};

    const int ironSurfaceCount = static_cast<int>(static_cast<double>(m_tiles.size()) * IRON_SURFACE_AMOUNT);
    const int ironUndergroundCount = static_cast<int>(static_cast<double>(m_tiles.size()) * IRON_UNDERGROUND_AMOUNT);
    const int ironCavernCount = static_cast<int>(static_cast<double>(m_tiles.size()) * IRON_CAVERN_AMOUNT);
    // IRON
    for (int i = 0; i < ironSurfaceCount; ++i)
    {
        FillBlobAtRandomPosition(worldWidth, surfaceHeight, Tile::Type::Iron, IRON_SURFACE_SIZE, IRON_SURFACE_VARIATION);
    }
    for (int i = 0; i < ironUndergroundCount; ++i)
    {
        FillBlobAtRandomPosition(worldWidth, undergroundHeight, Tile::Type::Iron, IRON_UNDERGROUND_SIZE, IRON_UNDERGROUND_VARIATION);
    }
    for (int i = 0; i < ironCavernCount; ++i)
    {
        FillBlobAtRandomPosition(worldWidth, cavernHeight, Tile::Type::Iron, IRON_CAVERN_SIZE, IRON_CAVERN_VARIATION);
    }
    
    constexpr double SILVER_UNDERGROUND_AMOUNT              = 2.6E-05;
    constexpr Vector2<double> SILVER_UNDERGROUND_SIZE       = Vector2<double> {3, 6};
    constexpr Vector2<double> SILVER_UNDERGROUND_VARIATION  = Vector2<double> {0.1, 0.4};
    constexpr double SILVER_CAVERN_AMOUNT                   = 0.00015;
    constexpr Vector2<double> SILVER_CAVERN_SIZE            = Vector2<double> {4, 9};
    constexpr Vector2<double> SILVER_CAVERN_VARIATION       = Vector2<double> {0.1, 0.4};
    
    const int silverUndergroundCount = static_cast<int>(static_cast<double>(m_tiles.size()) * IRON_UNDERGROUND_AMOUNT);
    const int silverCavernCount = static_cast<int>(static_cast<double>(m_tiles.size()) * IRON_CAVERN_AMOUNT);
    // SILVER
    for (int i = 0; i < silverUndergroundCount; ++i)
    {
        FillBlobAtRandomPosition(worldWidth, undergroundHeight, Tile::Type::Silver, SILVER_UNDERGROUND_SIZE, SILVER_UNDERGROUND_VARIATION);
    }
    for (int i = 0; i < silverCavernCount; ++i)
    {
        FillBlobAtRandomPosition(worldWidth, cavernHeight, Tile::Type::Silver, SILVER_CAVERN_SIZE, SILVER_CAVERN_VARIATION);
    }

    constexpr double GOLD_CAVERN_AMOUNT                   = 0.00012;
    constexpr Vector2<double> GOLD_CAVERN_SIZE            = Vector2<double> {4, 8};
    constexpr Vector2<double> GOLD_CAVERN_VARIATION       = Vector2<double> {0.1, 0.4};
    
    const int goldCavernCount = static_cast<int>(static_cast<double>(m_tiles.size()) * GOLD_CAVERN_AMOUNT);
    // GOLD
    for (int i = 0; i < goldCavernCount; ++i)
    {
        FillBlobAtRandomPosition(worldWidth, cavernHeight, Tile::Type::Silver, GOLD_CAVERN_SIZE, GOLD_CAVERN_VARIATION);
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
void WorldGenerator::GenerateAnthillCaves(const std::vector<int>& positions)
{
}

void WorldGenerator::FixGravitatingSand(const std::vector<int>& surface)
{
    constexpr int CORRECTION_RADIUS = 16;

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
    constexpr int CORRECTION_RADIUS = 16;

    for (int x = 0; x < m_width; ++x)
    {
        for (int y = surface[x] - CORRECTION_RADIUS; y < surface[x] + CORRECTION_RADIUS; ++y)
        {
            if (!IsTile(x, y, Tile::Type::Air))
            {
                break;
            }
            if (!IsWall(x, y, Tile::Wall::Air))
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
    constexpr int CORRECTION_RADIUS = 16;
    
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