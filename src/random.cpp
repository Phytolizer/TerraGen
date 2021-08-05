#include "random.hpp"
#include <random>

Random::Random(std::uint64_t seed) : m_eng{seed}
{
}

double Random::GetDouble(const double min, const double max)
{
    return std::uniform_real_distribution{min, max}(m_eng);
}

int Random::GetInt(const int min, const int max)
{
    return std::uniform_int_distribution{min, max}(m_eng);
}

double Random::GetNoise(int x, int y)
{
    return 0;
}