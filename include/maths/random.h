
#pragma once

#include <cmath>
#include <random>

inline float rand_float()
{
    return (float)rand() / (float)RAND_MAX;
}

inline float sample_gaussian(float mean, float std)
{
    static std::default_random_engine generator;
    std::normal_distribution<float> distribution(mean, std);
    return distribution(generator);
}
