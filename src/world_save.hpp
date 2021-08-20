#ifndef TERRAGEN_WORLD_SAVE_HPP
#define TERRAGEN_WORLD_SAVE_HPP

#include "world.hpp"

namespace WorldSave
{
void SaveWorld(World world);
void SaveSectionHeaders(World world, std::ofstream& file);
void SaveHeaderFlags(World world, std::ofstream file);
void SaveTiles(World world, std::ofstream file);
void SaveChests(World world, std::ofstream file);
void SaveSigns(World world, std::ofstream file);
void SaveNPCs(World world, std::ofstream file);
void SaveMobs(World world, std::ofstream file);
void SaveTileEntities(World world, std::ofstream file);
void SavePressurePlate(World world, std::ofstream file);
void SaveTownManager(World world, std::ofstream file);
void SaveBestiary(World world, std::ofstream file);
void SaveCreativePowers(World world, std::ofstream file);
}    // namespace WorldSave

#endif    // TERRAGEN_WORLD_GEN_HPP
