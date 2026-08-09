#include "enemymanager.hpp"


EnemyManager::EnemyManager(TextureManager& textures, Player& player, ProjectileManager& projectiles, RandomGenerator& randomizer)
    : textures(textures), 
      projectiles(projectiles),
      player(player), 
      killCount(0), 
      spawningCooldown(sf::seconds(3.f)),
      timeToNextSpawn(spawningCooldown), 
      randomizer(randomizer)
{
    // for (int i = 0; i < jelliesPopulationSize; i++)
    // {
    //     addRandomJelly();
    // }
}

void EnemyManager::update(sf::Time deltaTime)
{
    timeToNextSpawn -= deltaTime;
    if (timeToNextSpawn <= sf::Time::Zero && jellies.size() < jelliesPopulationSize)
    {
        timeToNextSpawn = spawningCooldown;
        if (jellies.size() >= 2)  addChildJelly();
        else addRandomJelly();
    }
    for (auto& jelly : jellies)
    {
        jelly->setTargetPosition(player.getBounds().getCenter());
    }
    updateVector(jellies, deltaTime);
    
}

void EnemyManager::render(sf::RenderWindow& window)
{
    renderVector(jellies, window);
}

std::size_t EnemyManager::jelliesCount() { return jellies.size(); }

Jelly& EnemyManager::jellyAt(std::size_t i) { return *jellies.at(i); }

void EnemyManager::addJelly(Chromosome chromosome)
{
    sf::Vector2f distance = sf::Vector2f({spawnDistance, 0}).rotatedBy(sf::degrees(rand() % 360));
    jellies.push_back(std::make_unique<Jelly>(player.getBounds().position + distance, textures, projectiles, chromosome));
}

void EnemyManager::addRandomJelly() { addJelly(Chromosome(randomizer)); }

void EnemyManager::addChildJelly()
{
    Chromosome parent1 = rouletteWheelParent();
    Chromosome parent2 = rouletteWheelParent();
    
    Chromosome child = parent1.crossover(parent2);

    if (randomizer.randomFloat() > mutationRatio) child.applyMutation();

    addJelly(child);
}

Chromosome EnemyManager::rouletteWheelParent()
{
    int sumDamage = 0;
    for (auto& jelly: jellies) sumDamage += jelly->chromosome.getDamageInflicted();
    int randomDamagePosition = randomizer.randomInt(0, sumDamage);

    sumDamage = 0;
    for (auto& jelly : jellies)
    {
        if (sumDamage > randomDamagePosition)
        {
            return jelly->chromosome;
        }
        sumDamage += jelly->chromosome.getDamageInflicted();
    }
    return jellies.at(jellies.size() - 1)->chromosome;
}