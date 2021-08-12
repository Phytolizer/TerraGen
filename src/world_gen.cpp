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
     * 00% - 06% Sky
     * 06% - 18% Overworld
     * 18% - 24% Dirt
     * 24% - 34% Underground
     * 34% - 69% Cavern (Water)
     * 69% - 89% Cavern (Lava)
     * 89% - 100% Underworld
     */
    const int surfaceLayer = world.RandomHeight(0.25, 0.26);
    const int cavernLayer = world.RandomHeight(0.36, 0.38);
    const int underworldLayer = world.m_height - 200;

    auto surfaceTerrain = world.RandomTerrain(surfaceLayer - 125, surfaceLayer - 5, 5);
    auto dirtHeights = world.RandomTerrain(surfaceLayer, surfaceLayer + 10, 2);
    auto rockHeights = world.RandomTerrain(cavernLayer - 5, cavernLayer + 5, 2.5);

    world.GenerateLayers(surfaceTerrain, rockHeights, underworldLayer);
    /// Add Tunnels
    world.GenerateSurfaceTunnels(surfaceTerrain);
    /// Add Sand
    world.GenerateSand(surfaceTerrain, surfaceLayer, rockHeights);
    /// Add Anthills (Add Cave Entrances Later)
    world.GenerateAnthills(surfaceTerrain);
    /// Mix Stone into Dirt
    world.GenerateSurfaceStone(surfaceTerrain, dirtHeights);
    world.GenerateUndergroundStone(dirtHeights, rockHeights);
    /// Mix Dirt into Stone
    world.GenerateCavernDirt(rockHeights, underworldLayer);
    /// Add Clay
    world.GenerateClay(surfaceTerrain, dirtHeights, rockHeights);
    
    /* BIOMES
     * Forest
     * Corruption/Crimson (mutually exclusive)
     * Ocean
     * Desert
     * Big desert (+caves)
     */
     
    /// Generate Caves
    //world.GenerateCaves(undergroundHeights);
    //world.GenerateEntranceCaves(surfaceHeights);
    //world.GenerateLargeCaves(cavernHeights);

    return world.Finish();
}
} // namespace WorldGen
