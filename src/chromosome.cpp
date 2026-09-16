#include "chromosome.hpp"


Chromosome::Chromosome(RandomGenerator& randomizer)
    : randomizer(&randomizer)
{
    stats.insert({"health", std::array<bool, statArraySize>{}});
    stats.insert({"starDamage", std::array<bool, statArraySize>{}});
    stats.insert({"biteDamage", std::array<bool, statArraySize>{}});
    stats.insert({"walkingSpeed", std::array<bool, statArraySize>{}});
    stats.insert({"bitingSpeed", std::array<bool, statArraySize>{}});
    stats.insert({"starSpeed", std::array<bool, statArraySize>{}});


    for(auto& stat : stats)
    {
        for (std::size_t i = 0; i < stat.second.size(); i++)
        {
            stat.second.at(i) = randomizer.randomInt(0, 1);
        }
    }

    for (std::size_t i = 0; i < paramArraySize; i++)
    {
        bitingDistance.at(i) = randomizer.randomInt(0, 1);
        shootingDistance.at(i) = randomizer.randomInt(0, 1);
    }
}

Chromosome Chromosome::crossover(Chromosome secondParent)
{
    Chromosome child(*randomizer);

    for(auto& stat : stats)
    {
        child.stats.at(stat.first) = crossoverArrays(stat.second, secondParent.stats.at(stat.first));
    }

    child.bitingDistance = crossoverArrays(bitingDistance, secondParent.bitingDistance);
    child.shootingDistance = crossoverArrays(shootingDistance, secondParent.shootingDistance);

    return child;
}

void Chromosome::applyMutation()
{
    for(auto& stat: stats) stat.second = mutateArray(stat.second);
    
    bitingDistance = mutateArray(bitingDistance);
    shootingDistance = mutateArray(shootingDistance);
}

int Chromosome::getDamageInflicted() { return damageInflicted; }

void Chromosome::changeDamageInflicted(int damage) { damageInflicted += damage; }

float Chromosome::getProportionalStat(std::string statName)
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

int Chromosome::getHealth() { return static_cast<int>(getProportionalStat("health") * 300.f); }

int Chromosome::getStarDamage() { return static_cast<int>(getProportionalStat("starDamage") * 60.f); }

int Chromosome::getBiteDamage() { return static_cast<int>(getProportionalStat("biteDamage") * 120.f); }

float Chromosome::getWalkingSpeed() { return getProportionalStat("walkingSpeed") * 600.f; }

float Chromosome::getBitingSpeed() { return getProportionalStat("bitingSpeed") * 1500.f + getWalkingSpeed(); }

float Chromosome::getStarSpeed() { return getProportionalStat("starSpeed") * 1200.f; }

sf::Color Chromosome::getColor()
{
    uint8_t a = arrayToInt(stats.at("health"));
    uint8_t b = arrayToInt(stats.at("starSpeed"));
    uint8_t c = arrayToInt(stats.at("bitingSpeed"));
    uint8_t d = arrayToInt(stats.at("biteDamage"));
    uint8_t e = arrayToInt(stats.at("walkingSpeed"));
    uint8_t f = arrayToInt(stats.at("starDamage"));
    return sf::Color{
        static_cast<std::uint8_t>((a << 4) | b),
        static_cast<std::uint8_t>((c << 4) | d),
        static_cast<std::uint8_t>((e << 4) | f)
    };
}

float Chromosome::getBitingDistance()
{
    float arraySum = static_cast<float>(arrayToInt(bitingDistance));
    return mapRange(0.f, std::pow(2, paramArraySize) - 1, minBitingDistance, maxBitingDistance, arraySum);
}

float Chromosome::getShootingDistance()
{
    float arraySum = static_cast<float>(arrayToInt(shootingDistance));
    return mapRange(0.f, std::pow(2, paramArraySize) - 1, minShootingDistance, maxShootingDistance, arraySum) + getBitingDistance();
}