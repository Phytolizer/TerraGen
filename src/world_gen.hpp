#ifndef TERRAGEN_WORLD_GEN_HPP
#define TERRAGEN_WORLD_GEN_HPP

#include "tile.hpp"
#include "world.hpp"
#include "world_size.hpp"
#include <random>
#include <vector>

namespace WorldGen
{

World Generate(WorldSize size);

} // namespace WorldGen
#endif // TERRAGEN_WORLD_GEN_HPP
