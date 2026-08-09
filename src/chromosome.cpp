#include "chromosome.hpp"


Chromosome::Chromosome(RandomGenerator& randomizer, int health, int starDamage, int biteDamage, sf::Color color)
    : damageInflicted(0), 
      randomizer(&randomizer)
{
    this->health = intToArray<healthArraySize>(health);
    this->starDamage = intToArray<starDamageArraySize>(starDamage);
    this->biteDamage = intToArray<biteDamageArraySize>(biteDamage);

    if (color == sf::Color::Black)
    {
        redColor = intToArray<colorArraySize>(randomizer.randomInt(0, 255));
        greenColor = intToArray<colorArraySize>(randomizer.randomInt(0, 255));
        blueColor = intToArray<colorArraySize>(randomizer.randomInt(0, 255));
    }
    else
    {
        redColor = intToArray<colorArraySize>(color.r);
        greenColor = intToArray<colorArraySize>(color.g);
        blueColor = intToArray<colorArraySize>(color.b);
    }
}

int Chromosome::getDamageInflicted() { return damageInflicted; }

void Chromosome::changeDamageInflicted(int damage) { damageInflicted += damage; }

int Chromosome::getHealth() { return arrayToInt(health); }

int Chromosome::getStarDamage() { return arrayToInt(starDamage); }

int Chromosome::getBiteDamage() { return arrayToInt(biteDamage); }

sf::Color Chromosome::getColor() { return sf::Color(arrayToInt(redColor), arrayToInt(greenColor), arrayToInt(blueColor)); }

Chromosome Chromosome::crossover(Chromosome secondParent)
{
    return Chromosome(*randomizer, 
        arrayToInt(crossoverArrays(health, secondParent.health)), 
        arrayToInt(crossoverArrays(starDamage, secondParent.starDamage)), 
        arrayToInt(crossoverArrays(biteDamage, secondParent.biteDamage)), 
        sf::Color(arrayToInt(crossoverArrays(redColor, secondParent.redColor)), 
        arrayToInt(crossoverArrays(greenColor, secondParent.greenColor)), 
        arrayToInt(crossoverArrays(blueColor, secondParent.blueColor))));
}

void Chromosome::applyMutation()
{
    health = mutateArray(health);
    starDamage = mutateArray(starDamage);
    biteDamage = mutateArray(biteDamage);
    redColor = mutateArray(redColor);
    blueColor = mutateArray(blueColor);
    greenColor = mutateArray(greenColor);
}