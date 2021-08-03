#ifndef TERRAGEN_WORLD_GEN_HPP
#define TERRAGEN_WORLD_GEN_HPP

#include "world.hpp"
#include "world_size.hpp"
#include <FastNoiseLite.h>
#include <random>

namespace WorldGen
{

    namespace Details
    {
        class Generator
        {
            std::default_random_engine m_eng;
            long m_seed;
            std::size_t m_width;
            std::size_t m_height;

            FastNoiseLite m_noise;

            std::vector<Tile::Tile> m_tiles;

            void GenerateDepthContours();
            void GenerateBiomeDelimiters();
            void GenerateCaves();

          public:
            class Builder
            {
                long m_seed;
                std::size_t m_width;
                std::size_t m_height;

              public:
                Builder(long seed);

                Builder& WithSize(std::size_t width, std::size_t height);

                Generator Build();
            };

            Generator(int seed, std::size_t width, std::size_t height);

            World Generate();
        };
    }

World Generate(WorldSize size);

} // namespace WorldGen
#endif // TERRAGEN_WORLD_GEN_HPP
