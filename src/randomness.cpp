#include "randomness.hpp"
#include <random>

double Random::GetDouble(const double min, const double max, std::default_random_engine &eng)
{
    return std::uniform_real_distribution{min, max}(eng);
}

int Random::GetInt(const int min, const int max, std::default_random_engine &eng)
{
    return std::uniform_int_distribution{min, max}(eng);
}