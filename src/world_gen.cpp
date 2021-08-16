#include "world_gen.hpp"
#include "random.hpp"
#include "world_generator.hpp"

namespace WorldGen
{
World Generate(const WorldSize size)
{
    constexpr std::uint64_t SEED = 999;
    auto world = WorldGenerator{size, SEED};

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
    const int underworldLayer = static_cast<int>(world.GetHeight()) - 200;

    constexpr Vector2<int> SURFACE_OFFSET   = Vector2<int> {-125, -5};
    constexpr int SURFACE_AMPLITUDE         = 5;
    constexpr int SURFACE_TIMER             = 80;
    constexpr Vector2<int> DIRT_OFFSET      = Vector2<int> {-4, 16};
    constexpr int DIRT_AMPLITUDE            = 2;
    constexpr int DIRT_TIMER                = 20;
    constexpr Vector2<int> ROCK_OFFSET      = Vector2<int> {-20, 20};
    constexpr int ROCK_AMPLITUDE            = 3;
    constexpr int ROCK_TIMER                = 20;

    auto surfaceTerrain =
        world.RandomTerrain(surfaceLayer + SURFACE_OFFSET.x, surfaceLayer + SURFACE_OFFSET.y, SURFACE_AMPLITUDE, SURFACE_TIMER);
    auto dirtHeights =
        world.RandomTerrain(surfaceLayer + DIRT_OFFSET.x, surfaceLayer + DIRT_OFFSET.y, DIRT_AMPLITUDE, DIRT_TIMER);
    auto rockHeights =
        world.RandomTerrain(cavernLayer + ROCK_OFFSET.x, cavernLayer + ROCK_OFFSET.y, ROCK_AMPLITUDE, ROCK_TIMER);

    world.GenerateDepthLevels(surfaceLayer, cavernLayer, underworldLayer);
    world.GenerateLayers(surfaceTerrain, rockHeights, underworldLayer);
    /// Add Tunnels with walls
    world.GenerateSurfaceTunnels(surfaceTerrain);
    /// Add Sand
    world.GenerateSandDesert(surfaceTerrain);
    world.GenerateSandPiles(surfaceLayer, rockHeights);
    /// Add Anthills (Mountains with Caves)
    auto anthillCavePos = world.GenerateAnthills(surfaceTerrain);
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
    world.GenerateCaves(dirtHeights);
    world.GenerateEntranceCaves(surfaceTerrain);
    world.GenerateLargeCaves(rockHeights);

    /// Add Clay
    world.GenerateClay(surfaceTerrain, dirtHeights, rockHeights);
    /// Add Grass
    world.GenerateGrass(surfaceTerrain, surfaceLayer);
    /// Add Mud (Long, veiny stretches of mud. Thin and wiggly)
    world.GenerateMud((surfaceLayer + cavernLayer) / 2, underworldLayer);
    /// Add Silt (Scattered Patches in cavern layer)
    world.GenerateSilt(cavernLayer, underworldLayer);

    /* BIOMES PART 1
     * Ice (Two diagonal lines going to almost lava level. Convert stone to ice and dirt/clay/sand/mud to snow and silt
     * to slush) Jungle (Feathered uneven edges to underworld. Convert everthing to mud. Keep stone near surface with
     * falloff. Remove lava. Generate New Stone below cavern. Jungle walls from underground to lava level. Any mud
     * touching air becomes jungle grass) Marble (Horizontal walk) Granite (Scattered spheres?) Full Desert (Choose
     * location, create large sphere of sand partially underground. Clear any dirt/grass above. Generate sandstone /
     * caves) Sky Islands (Stretched semicircle of clouds. Highest point - 1 gets filled with dirt/grass. Generate
     * Building. Add a few small cloud patches above) 1.3 Glowing Mushroom Biomes (Horizontal walk underground. Clear a
     * large area under walk. Convert stone to mud and add mushroom grass) 1.4 Glowing Mushroom Biomes (Generate large
     * blob of mud with minor falloff -- Keep caves from top and near falloff. Carve out round holes inside with flat
     * bottoms.)
     */

    /// Add Metals
    world.GenerateMetals(surfaceLayer + SURFACE_OFFSET.x, surfaceLayer + SURFACE_OFFSET.y, cavernLayer, underworldLayer);
    /// Add Gems
    world.GenerateGems(surfaceLayer, underworldLayer);
    /// Add Webs
    world.GenerateWebs(surfaceLayer, underworldLayer);

    /* BIOMES PART 2
     * Underworld
     * Lakes
     * Dungeon
     * Corruption/Crimson (mutually exclusive)
     * Ocean
     */

    /// Anthill Caves (Mountain Caves)
    world.GenerateAnthillCaves(anthillCavePos);
    /// Gravitating Sand Fix
    world.FixGravitatingSand(surfaceTerrain);
    /// Dirt Walls Fix (Remove dirt walls with no tiles above them)
    world.FixDirtWalls(surfaceTerrain);
    /// Water on Sand Fix
    world.FixWaterOnSand(surfaceTerrain);

    /* BIOMES PART 3
     * Pyramids (Chance)
     * Living Wood Trees (Chance)
     * Altars
     * Flood Jungle Surface (Add a thin line of water to jungle surface)
     * Jungle Temple
     * Bee Hives
     */

    /// Smooth World (Hammer blocks to curve the world)
    world.SmoothWorld();
    /// Settle Liquids
    world.SettleLiquids();
    /// Add Waterfalls
    world.AddWaterfalls();

    return world.Finish();
}
}    // namespace WorldGen
