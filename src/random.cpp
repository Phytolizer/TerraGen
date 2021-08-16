#include "random.hpp"
#include <random>

Random::Random(std::uint64_t seed) : m_eng{seed}, m_randomModifier{seed}
{
}

double Random::GetDouble(const double min, const double max)
{
    return std::uniform_real_distribution{min, max}(m_eng);
}

double Random::GetDouble(const Vector2<double> minMax)
{
    return GetDouble(minMax.x, minMax.y);
}

int Random::GetInt(const int min, const int max)
{
    return std::uniform_int_distribution{min, max}(m_eng);
}

int Random::GetInt(const double min, const double max)
{
    return GetInt(static_cast<int>(min), static_cast<int>(max));
}

int Random::GetInt(const Vector2<int> minMax)
{
    return GetInt(minMax.x, minMax.y);
}

double Random::GetNoise(const double x, const double y)
{
    return m_noise.GetNoise<double>(x, y);
}

double Random::GetNoise(const int x, const int y)
{
    return GetNoise(static_cast<double>(x), static_cast<double>(y));
}

std::uint64_t Random::Next()
{
    return m_randomModifier++;
}