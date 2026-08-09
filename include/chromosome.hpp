#pragma once

#include <iterator>
#include <array>
#include "random.hpp"
#include <SFML/Graphics/Color.hpp>
#include <cassert>


class Chromosome
{
    private:
        static constexpr std::size_t healthArraySize = 8;
        static constexpr std::size_t starDamageArraySize = 5;
        static constexpr std::size_t biteDamageArraySize = 5;
        static constexpr std::size_t colorArraySize = 7;
        RandomGenerator* randomizer;
        int damageInflicted;
        std::array<bool, healthArraySize> health;
        std::array<bool, starDamageArraySize> starDamage;
        std::array<bool, biteDamageArraySize> biteDamage;
        std::array<bool, colorArraySize> redColor;
        std::array<bool, colorArraySize> greenColor;
        std::array<bool, colorArraySize> blueColor;
        

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
    
    public:
        Chromosome(RandomGenerator& randomizer, int health = 50, int starDamage = 10, int biteDamage = 20, sf::Color color = sf::Color::Black);
        int getDamageInflicted();
        void changeDamageInflicted(int damage);
        int getHealth();
        int getStarDamage();
        int getBiteDamage();
        sf::Color getColor();
        Chromosome crossover(Chromosome secondParent);
        void applyMutation();
};