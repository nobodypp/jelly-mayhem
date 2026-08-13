#pragma once

#include <random>
#include <cstddef>

class RandomGenerator
{
private:
    std::random_device rd;
    std::mt19937 gen;

public:
    RandomGenerator()
        : gen(rd())
    {}

    explicit RandomGenerator(std::mt19937::result_type seed)
        : gen(seed)
    {}

    std::size_t randomIndex(std::size_t min, std::size_t max)
    {
        std::uniform_int_distribution<std::size_t> dist(min, max);
        return dist(gen);
    }

    int randomInt(int min, int max)
    {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(gen);
    }

    float randomFloat()
    {
        std::uniform_real_distribution<float> dist(0.f, 1.f);
        return dist(gen);
    }
};