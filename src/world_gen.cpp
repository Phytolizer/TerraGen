#include "world_gen.hpp"
#include "random.hpp"
#include "world_generator.hpp"
#include <FastNoiseLite.h>
#include <chrono>

namespace WorldGen
{
World Generate(const WorldSize size)
{
    const auto start = std::chrono::high_resolution_clock::now();
    constexpr int seed = 100;
    FastNoiseLite n;

    auto world = WorldGenerator{size, seed};

    /* Depth Contours
     * 06% Space
     * 18% Surface
     *      - 12% Air
     *      - 6% Ground
     * 10% Underground
     * 35% Cavern (Water)
     * 20% Cavern (Lava)
     * 11% Underworld
     */
    int surfaceHeight = world.GenerateHeight(0.08, 0.11);
    int undergroundHeight = world.GenerateHeight(0.18, 0.21);
    int cavernHeight = world.GenerateHeight(0.27, 0.31);
    int underworldHeight = world.GenerateHeight(0.85, 0.88);
    auto surfaceHeights = world.GenerateTerrain(surfaceHeight);
    auto undergroundHeights = world.GenerateTerrain(undergroundHeight);
    auto cavernHeights = world.GenerateTerrain(cavernHeight);
    auto underworldHeights = world.GenerateTerrain(underworldHeight);
    world.GenerateLayers(surfaceHeights, cavernHeights);
    /// Add Sand
    world.GenerateDeserts(surfaceHeights);
    world.GenerateSandPiles(undergroundHeight, cavernHeight);
    /// Mix Stone into Dirt
    world.GenerateSurface(surfaceHeights, undergroundHeights);
    world.GenerateUnderground(undergroundHeights, cavernHeights);
    /// Mix Dirt into Stone
    world.GenerateCavern(cavernHeights, underworldHeights);
    /// Add Clay
    world.GenerateClay(surfaceHeights, undergroundHeights, cavernHeights);

    /* BIOMES
     * Forest
     * Corruption/Crimson (mutually exclusive)
     * Ocean
     * Desert
     * Big desert (+caves)
     */
    return world.Finish();
}
} // namespace WorldGen
