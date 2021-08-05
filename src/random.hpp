#ifndef TERRAGEN_RANDOM_HPP
#define TERRAGEN_RANDOM_HPP

#include <cstdint>
#include <random>
#include <FastNoiseLite.h>

class Random
{
    std::default_random_engine m_eng;
    FastNoiseLite m_noise;
    std::uint64_t m_randomModifier;

  public:
    Random(std::uint64_t seed);
    double GetDouble(double min, double max);
    int GetInt(int min, int max);
    double GetNoise(int x, int y);
    std::uint64_t Next();
};

#endif // TERRAGEN_RANDOM_HPP
