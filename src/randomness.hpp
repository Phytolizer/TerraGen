#ifndef TERRAGEN_RANDOMNESS_HPP
#define TERRAGEN_RANDOMNESS_HPP

#include <random>

namespace Random
{

double GetDouble(double min, double max, std::default_random_engine &eng);
int GetInt(int min, int max, std::default_random_engine &eng);

} // namespace Random

#endif // TERRAGEN_RANDOMNESS_HPP
