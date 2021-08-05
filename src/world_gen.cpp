#include "world_gen.hpp"
#include "random.hpp"
#include "world_generator.hpp"
#include <FastNoiseLite.h>

namespace WorldGen
{
World Generate(const WorldSize size)
{
    constexpr std::uint64_t seed = 100;
    auto world = WorldGenerator{size, seed};

    /* Depth Contours
     * 00% - 06% Space
     * 06% - 18% Space
     * 18% - 24% Surface
     * 24% - 34% Underground
     * 34% - 69% Cavern (Water)
     * 69% - 89% Cavern (Lava)
     * 89% - 100% Underworld
     */
    int surfaceHeight = world.GenerateHeight(0.17, 0.19);
    int undergroundHeight = world.GenerateHeight(0.21, 0.23);
    int cavernHeight = world.GenerateHeight(0.33, 0.35);
    int underworldHeight = world.GenerateHeight(0.88, 0.90);
    auto surfaceHeights = world.GenerateTerrain(surfaceHeight, 1, 5);
    auto undergroundHeights = world.GenerateTerrain(undergroundHeight, 1.5, 5);
    auto cavernHeights = world.GenerateTerrain(cavernHeight, 0.25, 6);
    auto underworldHeights = world.GenerateTerrain(underworldHeight, 0.6, 0.5);
    world.GenerateLayers(surfaceHeights, cavernHeights, underworldHeights);
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
    /// Generate Caves
    //world.GenerateCaves(undergroundHeights);
    //world.GenerateEntranceCaves(surfaceHeights);
    //world.GenerateLargeCaves(cavernHeights);

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
