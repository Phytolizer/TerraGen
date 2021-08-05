#ifndef TERRAGEN_WORLD_GEN_HPP
#define TERRAGEN_WORLD_GEN_HPP

#include "world.hpp"
#include "world_size.hpp"
#include <FastNoiseLite.h>
#include <random>

namespace WorldGen
{
World Generate(WorldSize size);
}

#endif // TERRAGEN_WORLD_GEN_HPP
