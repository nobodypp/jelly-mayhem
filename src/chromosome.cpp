#include "chromosome.hpp"


Chromosome::Chromosome(RandomGenerator& randomizer, int health, int starDamage, int biteDamage, sf::Color color, int walkingSpeed, int bitingSpeed)
    : damageInflicted(0), 
      randomizer(&randomizer)
{
    this->health = intToArray<healthArraySize>(health);
    this->starDamage = intToArray<starDamageArraySize>(starDamage);
    this->biteDamage = intToArray<biteDamageArraySize>(biteDamage);
    this->walkingSpeed = intToArray<speedArraySize>(walkingSpeed);
    this->bitingSpeed = intToArray<speedArraySize>(bitingSpeed);

    redColor = intToArray<colorArraySize>(color.r);
    greenColor = intToArray<colorArraySize>(color.g);
    blueColor = intToArray<colorArraySize>(color.b);
}

int Chromosome::getDamageInflicted() { return damageInflicted; }

void Chromosome::changeDamageInflicted(int damage) { damageInflicted += damage; }

int Chromosome::getHealth() { return arrayToInt(health); }

int Chromosome::getStarDamage() { return arrayToInt(starDamage); }

int Chromosome::getBiteDamage() { return arrayToInt(biteDamage); }

sf::Color Chromosome::getColor() { return sf::Color(arrayToInt(redColor), arrayToInt(greenColor), arrayToInt(blueColor)); }

float Chromosome::getWalkingSpeed() { return static_cast<float> (arrayToInt(walkingSpeed)); }

float Chromosome::getBitingSpeed() { return static_cast<float> (arrayToInt(bitingSpeed)); }

Chromosome Chromosome::crossover(Chromosome secondParent)
{
    return Chromosome(*randomizer, 
        arrayToInt(crossoverArrays(health, secondParent.health)), 
        arrayToInt(crossoverArrays(starDamage, secondParent.starDamage)), 
        arrayToInt(crossoverArrays(biteDamage, secondParent.biteDamage)), 
        sf::Color(arrayToInt(crossoverArrays(redColor, secondParent.redColor)), 
        arrayToInt(crossoverArrays(greenColor, secondParent.greenColor)), 
        arrayToInt(crossoverArrays(blueColor, secondParent.blueColor))),
        arrayToInt(crossoverArrays(walkingSpeed, secondParent.walkingSpeed)),
        arrayToInt(crossoverArrays(bitingSpeed, secondParent.bitingSpeed)));
}

void Chromosome::applyMutation()
{
    health = mutateArray(health);
    starDamage = mutateArray(starDamage);
    biteDamage = mutateArray(biteDamage);
    redColor = mutateArray(redColor);
    blueColor = mutateArray(blueColor);
    greenColor = mutateArray(greenColor);
    walkingSpeed = mutateArray(walkingSpeed);
    bitingSpeed = mutateArray(bitingSpeed);
}