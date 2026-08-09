#pragma once

#include <random>

class RandomGenerator
{
    private:
        std::random_device rd;
        std::mt19937 gen{rd()};

    public:
        std::size_t randomIndex(std::size_t min, std::size_t max)
        {
            std::uniform_int_distribution<std::size_t> dist(min, max);
            return dist(gen);
        }

        int randomInt(int min, int max)
        {
            std::uniform_int_distribution<int> distribution(min, max);
            return distribution(gen);
        }

        float randomFloat()
        {
            return std::uniform_real_distribution<float>(0.f, 1.f)(gen);
        }
};