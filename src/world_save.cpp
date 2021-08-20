#include "world_save.hpp"
#include "SDL_stdinc.h"
#include <iostream>
#include <fstream>
#include <vector>

void WorldSave::SaveWorld(World world)
{
  //std::fstream binaryWriter; = fstream
  std::ofstream worldFile ("world.wld", std::ios::out | std::ios::binary);
//   SaveSectionHeaders(world, worldFile);
//   SaveHeaderFlags(world, worldFile);
//   SaveTiles(world, worldFile);
//   SaveChests(world, worldFile);
//   SaveSigns(world, worldFile);s
//   SaveNPCs(world, worldFile);
//   SaveMobs(world, worldFile);
//   SaveTileEntities(world, worldFile);
//   SavePressurePlate(world, worldFile);
//   SaveTownManager(world, worldFile);
//   SaveBestiary(world, worldFile);
//   SaveCreativePowers(world, worldFile);
  worldFile.close();
}

void WorldSave::SaveSectionHeaders(World world, std::ofstream& file)
{
    //file.write(world.VERSION, 0);
}

//void WorldSave::SaveTiles(std::vector<Tile> tiles, int width, int height)
//{
//
//}