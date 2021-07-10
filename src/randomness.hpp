#ifndef TERRAGEN_RANDOMNESS_HPP
#define TERRAGEN_RANDOMNESS_HPP

#include <random>

double RandFloat(double min, double max, std::default_random_engine &eng);
int RandInt(int min, int max, std::default_random_engine &eng);

#endif // TERRAGEN_RANDOMNESS_HPP
