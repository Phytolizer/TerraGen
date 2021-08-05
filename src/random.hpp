#ifndef TERRAGEN_RANDOM_HPP
#define TERRAGEN_RANDOM_HPP

#include <cstdint>
#include <random>
#include <FastNoiseLite.h>

class Random
{
    std::default_random_engine m_eng;
    FastNoiseLite m_noise;

  public:
    Random(std::uint64_t seed);
    double GetDouble(double min, double max);
    int GetInt(int min, int max);
    double GetNoise(int x, int y);
};

#endif // TERRAGEN_RANDOM_HPP
