#include "chromosome.hpp"


Chromosome::Chromosome(RandomGenerator& randomizer)
    : randomizer(&randomizer)
{
    for(auto& stat : stats)
    {
        for (std::size_t i = 0; i < stat.second.size(); i++)
        {
            stat.second.at(i) = randomizer.randomInt(0, 1);
        }
    }
}

Chromosome Chromosome::crossover(Chromosome secondParent)
{
    Chromosome child(*randomizer);
    for(auto& stat : stats) child.stats.at(stat.first) = crossoverArrays(stat.second, secondParent.stats.at(stat.first));

    return child;
}

void Chromosome::applyMutation()
{
    for(auto& stat: stats) stat.second = mutateArray(stat.second);
}

int Chromosome::getDamageInflicted() { return damageInflicted; }

void Chromosome::changeDamageInflicted(int damage) { damageInflicted += damage; }

float Chromosome::getProportionalStat(Stat statName)
{
    int statSum = 0;

    for (auto& stat: stats)
    {
        statSum += arrayToInt(stat.second) + 1;
    }
    
    if (statSum == 0) return 1.f / stats.size();
    return static_cast<float>(arrayToInt(stats.at(statName)) + 1) / static_cast<float>(statSum);
}

float Chromosome::mapRange(float a1, float a2, float b1, float b2, float s) { return b1 + (s - a1) * (b2 - b1) / (a2 - a1); }

int Chromosome::getHealth() { return static_cast<int>(getProportionalStat(Stat::Health) * 300.f); }

int Chromosome::getStarDamage() { return static_cast<int>(getProportionalStat(Stat::StarDamage) * 60.f); }

int Chromosome::getBiteDamage() { return static_cast<int>(getProportionalStat(Stat::BiteDamage) * 120.f); }

float Chromosome::getWalkingSpeed() { return getProportionalStat(Stat::WalkingSpeed) * 600.f; }

float Chromosome::getBitingSpeed() { return getProportionalStat(Stat::BitingSpeed) * 1500.f + getWalkingSpeed(); }

float Chromosome::getStarSpeed() { return getProportionalStat(Stat::StarSpeed) * 1200.f; }

sf::Color Chromosome::getColor()
{
    uint8_t a = arrayToInt(stats.at(Stat::Health));
    uint8_t b = arrayToInt(stats.at(Stat::StarSpeed));
    uint8_t c = arrayToInt(stats.at(Stat::BitingSpeed));
    uint8_t d = arrayToInt(stats.at(Stat::BiteDamage));
    uint8_t e = arrayToInt(stats.at(Stat::WalkingSpeed));
    uint8_t f = arrayToInt(stats.at(Stat::StarDamage));
    return sf::Color{
        static_cast<std::uint8_t>((a << 4) | b),
        static_cast<std::uint8_t>((c << 4) | d),
        static_cast<std::uint8_t>((e << 4) | f)
    };
}