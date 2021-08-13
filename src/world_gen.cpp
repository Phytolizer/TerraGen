#include "world_gen.hpp"
#include "world_generator.hpp"
#include <FastNoiseLite.h>
#include "random.hpp"

namespace WorldGen
{
World Generate(const WorldSize size)
{
    constexpr std::uint64_t seed = 100;
    auto world = WorldGenerator{size, seed};

    /* Depth Contours
     * 00% - 06% Sky
     * 06% - 18% Overworld
     * 18% - 25% Dirt
     * 25% World Surface Layer
     * 26% - 37% Underground
     * 37% World Cavern Layer
     * 37% - 70% Cavern (Water)
     * 70% - 90% Cavern (Lava)
     * 90% - 100% Underworld -- 200 Blocks tall always
     */
    const int surfaceLayer = world.RandomHeight(0.25, 0.26);
    const int cavernLayer = world.RandomHeight(0.36, 0.38);
    const int underworldLayer = world.m_height - 200;

    auto surfaceTerrain = world.RandomTerrain(surfaceLayer - 125, surfaceLayer - 5, 5);
    auto dirtHeights = world.RandomTerrain(surfaceLayer, surfaceLayer + 10, 2);
    auto rockHeights = world.RandomTerrain(cavernLayer - 5, cavernLayer + 5, 2.5);

    world.GenerateLayers(surfaceTerrain, rockHeights, underworldLayer);
    /// Add Tunnels with walls
    world.GenerateSurfaceTunnels(surfaceTerrain);
    /// Add Sand
    world.GenerateSand(surfaceTerrain, surfaceLayer, rockHeights);
    /// Add Anthills (Mountains with Caves)
    world.GenerateAnthills(surfaceTerrain);
    /// Mix Stone into Dirt
    world.GenerateSurfaceStone(surfaceTerrain, dirtHeights);
    world.GenerateUndergroundStone(dirtHeights, rockHeights);
    /// Mix Dirt into Stone
    world.GenerateCavernDirt(rockHeights, underworldLayer);
    
    /* CAVES
     * Small Holes (Scattered throughout, small, water and lava filled ones too)
     * Dirt Layer Caves (Tall winding narrow caves, some filled with watter)
     * Rock Layer Caves (Large expansive caves, some water some lava)
     * Surface Caves (Caves from the surface -- entrance caves -- large, winding)
     */
    //world.GenerateCaves(undergroundHeights);
    //world.GenerateEntranceCaves(surfaceHeights);
    //world.GenerateLargeCaves(cavernHeights);

    /// Add Clay
    world.GenerateClay(surfaceTerrain, dirtHeights, rockHeights);
    /// Add Mud (Long, veiny stretches of mud. Thin and wiggly)
    //world.GenerateMud();
    /// Add Silt (Scattered Patches in cavern layer)
    //world.GenerateSilt();

    /* BIOMES PART 1
     * Ice (Two diagonal lines going to almost lava level. Convert stone to ice and dirt/clay/sand/mud to snow and silt to slush)
     * Jungle (Feathered uneven edges to underworld. Convert everthing to mud. Keep stone near surface with falloff. Remove lava.
     *         Generate New Stone below cavern. Jungle walls from underground to lava level. Any mud touching air becomes jungle grass)
     * Marble (Horizontal walk)
     * Granite (Scattered spheres?)
     * Full Desert (Choose location, create large sphere of sand partially underground. Clear any dirt/grass above. Generate sandstone / caves)
     * Sky Islands (Stretched semi circle of clouds. Highest point - 1 gets filled with dirt/grass. Generate Building. Add a few small cloud patches above)
     * 1.3 Glowing Mushroom Biomes (Horizontal walk underground. Clear a large area under walk. Convert stone to mud and add mushroom grass)
     * 1.4 Glowing Mushroom Biomes (Generate large blob of mud with minor falloff -- Keep caves from top and near falloff. Carve out round holes inside with flat bottoms.)
     */
    

    /// Add Metals
    //world.GenerateMetals();
    /// Add Gems
    //world.GenerateGems();
    /// Add Webs
    //world.GenerateWebs();

    /* BIOMES PART 2
     * Underworld
     * Lakes
     * Dungeon
     * Corruption/Crimson (mutually exclusive)
     * Ocean
     */


    /// Anthill Caves (Mountain Caves)
    //world.GenerateAnthillCaves();
    /// Gravitating Sand Fix
    //world.FixGravitatingSand();
    /// Dirt Walls Fix (Remove dirt walls with no tiles above them)
    //world.FixDirtWalls();
    /// Water on Sand Fix
    //world.FixWaterOnSand();

    /* BIOMES PART 3
     * Pyramids (Chance)
     * Living Wood Trees (Chance)
     * Altars
     * Floor Jungle Surface (Add a thin line of water to jungle surface)
     * Jungle Temple
     * Bee Hives
     */


    /// Smooth World (Hammer blocks to curve the world)
    //world.SmoothWorld();
    /// Settle Liquids
    //world.SettleLiquids();
    /// Add Waterfalls
    //world.AddWaterfalls();

    return world.Finish();
}
} // namespace WorldGen
