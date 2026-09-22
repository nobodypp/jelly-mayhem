#pragma once

#include <iterator>
#include <array>
#include <SFML/Graphics/Color.hpp>
#include <cassert>
#include <map>
#include "random.hpp"


class Chromosome
{
    private:
        RandomGenerator* randomizer;

        enum class Stat
        {
            Health, 
            StarDamage, 
            BiteDamage, 
            WalkingSpeed, 
            BitingSpeed, 
            StarSpeed
        };
        static constexpr std::size_t statArraySize = 4;
        std::map<Stat, std::array<bool, statArraySize>> stats{
            {Stat::Health, std::array<bool, statArraySize>{}}, 
            {Stat::StarDamage, std::array<bool, statArraySize>{}}, 
            {Stat::BiteDamage, std::array<bool, statArraySize>{}}, 
            {Stat::WalkingSpeed, std::array<bool, statArraySize>{}}, 
            {Stat::BitingSpeed, std::array<bool, statArraySize>{}}, 
            {Stat::StarSpeed, std::array<bool, statArraySize>{}}
        };

        int damageInflicted = 0;
        
        template <std::size_t N>
        int arrayToInt(const std::array<bool, N>& array)
        {
            int result = 0;
            int power = 1;

            for (std::size_t i = 0; i < array.size(); ++i)
            {
                result += array.at(i) * power;
                power *= 2;
            }

            return result;
        }

        template <std::size_t N>
        std::array<bool, N> intToArray(int number)
        {
            assert(number >= 0);

            std::array<bool, N> array{};

            for (std::size_t i = 0; i < N && number > 0; ++i)
            {
                array.at(i) = number % 2;
                number /= 2;
            }

            return array;
        }

        template <std::size_t N>
        std::array<bool, N> crossoverArrays(
            const std::array<bool, N>& array1,
            const std::array<bool, N>& array2)
        {
            std::array<bool, N> outputArray{};

            std::size_t crossoverPoint =
                randomizer->randomIndex(0, N);

            for (std::size_t i = 0; i < N; ++i)
            {
                if (i < crossoverPoint)
                    outputArray[i] = array1[i];
                else
                    outputArray[i] = array2[i];
            }

            return outputArray;
        }

        template<std::size_t N>
        std::array<bool, N> mutateArray(std::array<bool, N> array)
        {
            std::size_t index = randomizer->randomIndex(0, array.size() - 1);
            array.at(index) = !array.at(index);
            return array;
        }

        float getProportionalStat(Stat statName);
        float mapRange(float a1, float a2, float b1, float b2, float s);
    
    public:
        Chromosome(RandomGenerator& randomizer);
        int getDamageInflicted();
        void changeDamageInflicted(int damage);
        Chromosome crossover(Chromosome secondParent);
        void applyMutation();
        int getHealth();
        int getStarDamage();
        int getBiteDamage();
        float getWalkingSpeed();
        float getBitingSpeed();
        float getStarSpeed();
        sf::Color getColor();
};