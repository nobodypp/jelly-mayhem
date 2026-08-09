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

}

void EnemyManager::update(sf::Time deltaTime)
{
    timeToNextSpawn -= deltaTime;
    if (timeToNextSpawn <= sf::Time::Zero && jellies.size() < jelliesPopulationSize)
    {
        timeToNextSpawn = spawningCooldown;
        if (jellies.size() >= 5)  addChildJelly();
        else addDefaultJelly();
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

void EnemyManager::addDefaultJelly() { addJelly(Chromosome(randomizer)); }

void EnemyManager::addChildJelly()
{
    Chromosome parent1 = rouletteWheelParent();
    Chromosome parent2 = rouletteWheelParent();
    
    Chromosome child = parent1.crossover(parent2);

    if (randomizer.randomFloat() < mutationRatio) child.applyMutation();

    addJelly(child);
}

Chromosome EnemyManager::rouletteWheelParent()
{
    int totalDamage = 0;

    for (const auto& jelly : jellies)
    {
        totalDamage += jelly->chromosome.getDamageInflicted();
    }

    // Jeśli wszyscy mają 0 damage, każdy ma taką samą szansę
    if (totalDamage == 0)
    {
        std::size_t index =
            randomizer.randomIndex(0, jellies.size() - 1);

        return jellies.at(index)->chromosome;
    }

    int randomDamage = randomizer.randomInt(1, totalDamage);

    int accumulatedDamage = 0;

    for (const auto& jelly : jellies)
    {
        accumulatedDamage += jelly->chromosome.getDamageInflicted();

        if (randomDamage <= accumulatedDamage)
        {
            return jelly->chromosome;
        }
    }

    // Nie powinno się wykonać
    return jellies.back()->chromosome;
}