#include "randomness.hpp"
#include <random>

double RandFloat(double min, double max, std::default_random_engine &eng)
{
    return std::uniform_real_distribution<double>{min, max}(eng);
}

int RandInt(int min, int max, std::default_random_engine &eng)
{
    return std::uniform_int_distribution<>{min, max}(eng);
}