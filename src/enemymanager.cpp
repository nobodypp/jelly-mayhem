#include "enemymanager.hpp"


EnemyManager::EnemyManager(TextureManager& textures, Player& player, ProjectileManager& projectiles, RandomGenerator& randomizer)
    : textures(textures), 
      projectiles(projectiles),
      player(player), 
      killCount(0), 
      spawningCooldown(sf::seconds(3.f)),
      timeToNextSpawn(spawningCooldown), 
      randomizer(randomizer), 
      currentLevel(1.0), 
    //   stagnationRespawns(0), 
      mutationRate(baseMutationRate), 
      bestFitness(0)
{}

void EnemyManager::update(sf::Time deltaTime)
{
    currentLevel += difficulty * deltaTime.asSeconds();

    for (auto& jelly : jellies)
    {
        jelly->setTargetPosition(player.getBounds().getCenter());
        if (!jelly->isAlive()) killCount++;
    }

    timeToNextSpawn -= deltaTime;
    if (timeToNextSpawn <= sf::Time::Zero && jellies.size() < jelliesPopulationSize)
    {
        timeToNextSpawn = spawningCooldown;
        if (jellies.size() >= 5)  addChildJelly();
        else addDefaultJelly();
        if (getBestFitness() > bestFitness + stagnationThreshold)
        {
            // stagnationRespawns = 0;
            mutationRate = std::max(baseMutationRate, mutationRate - 0.01f);
        }
        else 
        {
            // stagnationRespawns++;
            mutationRate = std::min(maxMutationRate, mutationRate + 0.01f);
        }
        bestFitness = getBestFitness();
        // std::cout << "Best: " << bestFitness << ",  total: " << getTotalFitness() << ", mutation: " << mutationRate << "\n";
    }

    updateVector(jellies, deltaTime);
    
}

void EnemyManager::render(sf::RenderWindow& window)
{
    renderVector(jellies, window);

    spawnDistance = std::sqrt(window.getSize().x * window.getSize().x + window.getSize().y * window.getSize().y) / 2.f;
}

std::size_t EnemyManager::jelliesCount() { return jellies.size(); }

Jelly& EnemyManager::jellyAt(std::size_t i) { return *jellies.at(i); }

void EnemyManager::addJelly(Chromosome chromosome)
{
    sf::Vector2f distance = sf::Vector2f({spawnDistance, 0}).rotatedBy(sf::degrees(randomizer.randomInt(0, 359)));
    jellies.push_back(std::make_unique<Jelly>(player.getBounds().position + distance, textures, projectiles, chromosome, currentLevel));
}

void EnemyManager::addDefaultJelly() { addJelly(Chromosome(randomizer)); }

void EnemyManager::addChildJelly()
{
    Chromosome parent1 = rouletteWheelParent();
    Chromosome parent2 = rouletteWheelParent();
    
    Chromosome child = parent1.crossover(parent2);

    if (randomizer.randomFloat() < mutationRate) child.applyMutation();

    addJelly(child);
}

Chromosome EnemyManager::rouletteWheelParent()
{
    int totalDamage = 0;

    for (const auto& jelly : jellies)
    {
        totalDamage += jelly->getChromosome().getDamageInflicted();
    }

    // Jeśli wszyscy mają 0 damage, każdy ma taką samą szansę
    if (totalDamage == 0)
    {
        std::size_t index =
            randomizer.randomIndex(0, jellies.size() - 1);

        return jellies.at(index)->getChromosome();
    }

    int randomDamage = randomizer.randomInt(1, totalDamage);

    int accumulatedDamage = 0;

    for (const auto& jelly : jellies)
    {
        accumulatedDamage += jelly->getChromosome().getDamageInflicted();

        if (randomDamage <= accumulatedDamage)
        {
            return jelly->getChromosome();
        }
    }

    // Nie powinno się wykonać
    return jellies.back()->getChromosome();
}

int EnemyManager::getBestFitness()
{
    int bestFitness = 0;
    for (auto& jelly : jellies) bestFitness = std::max(bestFitness, jelly->getChromosome().getDamageInflicted());
    return bestFitness;
}

int EnemyManager::getTotalFitness()
{
    int totalFitness = 0;
    for (auto& jelly : jellies) totalFitness += jelly->getChromosome().getDamageInflicted();
    return totalFitness;
}

int EnemyManager::getKillCount() { return killCount; }