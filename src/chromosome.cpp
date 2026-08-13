#include "chromosome.hpp"


Chromosome::Chromosome(RandomGenerator& randomizer)
    : damageInflicted(0), 
      randomizer(&randomizer), 
    //   health(intToArray<healthArraySize>(randomizer.randomInt(45, 55))), 
    //   starDamage(intToArray<starDamageArraySize>(randomizer.randomInt(5, 15))), 
    //   biteDamage(intToArray<biteDamageArraySize>(randomizer.randomInt(15, 25))), 
    //   walkingSpeed(intToArray<speedArraySize>(randomizer.randomInt(50, 150))), 
    //   bitingSpeed(intToArray<speedArraySize>(randomizer.randomInt(200, 300))), 
    //   starSpeed(intToArray<speedArraySize>(randomizer.randomInt(150, 250))), 
      redColor(intToArray<colorArraySize>(randomizer.randomInt(0, 255))), 
      greenColor(intToArray<colorArraySize>(randomizer.randomInt(0, 255))), 
      blueColor(intToArray<colorArraySize>(randomizer.randomInt(0, 255))), 
      bitingDistance(intToArray<bitingDistanceArraySize>(randomizer.randomInt(300, 350))), 
      shootingDistance(intToArray<shootingDistanceArraySize>(randomizer.randomInt(40, 60)))
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
}

Chromosome Chromosome::crossover(Chromosome secondParent)
{
    Chromosome child(*randomizer);
    // child.health = crossoverArrays(health, secondParent.health);
    // child.starDamage = crossoverArrays(starDamage, secondParent.starDamage);
    // child.biteDamage = crossoverArrays(biteDamage, secondParent.biteDamage);
    // child.walkingSpeed = crossoverArrays(walkingSpeed, secondParent.walkingSpeed);
    // child.bitingSpeed = crossoverArrays(bitingSpeed, secondParent.bitingSpeed);
    // child.starSpeed = crossoverArrays(starSpeed, secondParent.starSpeed);
    for(auto& stat : stats)
    {
        child.stats.at(stat.first) = crossoverArrays(stat.second, secondParent.stats.at(stat.first));
    }
    child.redColor = crossoverArrays(redColor, secondParent.redColor);
    child.greenColor = crossoverArrays(greenColor, secondParent.greenColor);
    child.blueColor = crossoverArrays(blueColor, secondParent.blueColor);
    child.bitingDistance = crossoverArrays(bitingDistance, secondParent.bitingDistance);
    child.shootingDistance = crossoverArrays(shootingDistance, secondParent.shootingDistance);
    return child;
}

void Chromosome::applyMutation()
{
    for(auto& stat: stats)
    {
        mutateArray(stat.second);
    }
    // health = mutateArray(health);
    // starDamage = mutateArray(starDamage);
    // biteDamage = mutateArray(biteDamage);
    // walkingSpeed = mutateArray(walkingSpeed);
    // bitingSpeed = mutateArray(bitingSpeed);
    // starSpeed = mutateArray(starSpeed);
    redColor = mutateArray(redColor);
    blueColor = mutateArray(blueColor);
    greenColor = mutateArray(greenColor);
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
        statSum += arrayToInt(stat.second);
    }
    if (statSum == 0) return 1.f / stats.size();
    return static_cast<float>(arrayToInt(stats.at(statName))) / static_cast<float>(statSum);
}

int Chromosome::getHealth() { return std::max(1, static_cast<int>(getProportionalStat("health") * 500.f)); }

int Chromosome::getStarDamage() { return std::max(1, static_cast<int>(getProportionalStat("starDamage") * 200.f)); }

int Chromosome::getBiteDamage() { return std::max(1, static_cast<int>(getProportionalStat("biteDamage") * 200.f)); }

float Chromosome::getWalkingSpeed() { return std::max(1.f, getProportionalStat("walkingSpeed") * 400.f); }

float Chromosome::getBitingSpeed() { return std::max(1.f, getProportionalStat("bitingSpeed") * 400.f) + getWalkingSpeed(); }

float Chromosome::getStarSpeed() { return std::max(1.f, getProportionalStat("starSpeed") * 400.f); }

sf::Color Chromosome::getColor() { return sf::Color(arrayToInt(redColor), arrayToInt(greenColor), arrayToInt(blueColor)); }

float Chromosome::getBitingDistance() { return std::max(1.f, static_cast<float> (arrayToInt(bitingDistance))); }

float Chromosome::getShootingDistance() { return std::max(1.f, static_cast<float> (arrayToInt(shootingDistance))) + getBitingDistance(); }