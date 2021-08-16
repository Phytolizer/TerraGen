#ifndef TERRAGEN_RANDOM_HPP
#define TERRAGEN_RANDOM_HPP

#include "vector_2.hpp"
#include <FastNoiseLite.h>
#include <cstdint>
#include <random>

class Random
{
    std::default_random_engine m_eng;
    FastNoiseLite m_noise;
    std::uint64_t m_randomModifier;

  public:
    explicit Random(std::uint64_t seed);
    double GetDouble(double min, double max);
    double GetDouble(Vector2<double>);
    int GetInt(int min, int max);
    int GetInt(double min, double max);
    int GetInt(Vector2<int>);
    double GetNoise(double x, double y);
    double GetNoise(int x, int y);
    std::uint64_t Next();
};

#endif    // TERRAGEN_RANDOM_HPP
