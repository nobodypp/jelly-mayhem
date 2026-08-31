#include "enemymanager.hpp"


EnemyManager::EnemyManager(AssetManager& assets, Player& player, ProjectileManager& projectiles, RandomGenerator& randomizer, PerkManager& perks, AudioManager& audio)
    : assets(assets), 
      projectiles(projectiles),
      player(player), 
      perks(perks), 
      audio(audio), 
      randomizer(randomizer), 
      killCount(0), 
      timeToNextSpawn(spawningCooldown),
      currentLevel(1.0), 
      mutationRate(baseMutationRate), 
      bestFitness(0), 
      spawnDistance(910.f)
{}

void EnemyManager::update(sf::Time deltaTime)
{
    // Increase difficulty over time
    currentLevel += difficulty * deltaTime.asSeconds();

    // Set each jelly's target, increase killCount if dead
    for (auto& jelly : jellies)
    {
        jelly->setTargetPosition(player.getBounds().getCenter());
        if (!jelly->isAlive()) killCount++;
    }

    // Spawn a new jelly if missing and cooldown time has passed
    timeToNextSpawn -= deltaTime;
    if (timeToNextSpawn <= sf::Time::Zero && jellies.size() < jelliesPopulationSize)
    {
        timeToNextSpawn = spawningCooldown;

        // The first jellies should be random, the next ones should be generated using GA
        if (jellies.size() >= 10)  addChildJelly();
        else addDefaultJelly();

        // Increase mutation rate if the jellies sucks
        if (getBestFitness() > bestFitness + stagnationThreshold)
        {
            mutationRate = std::max(baseMutationRate, mutationRate - 0.01f);
        }
        else 
        {
            mutationRate = std::min(maxMutationRate, mutationRate + 0.01f);
        }
        bestFitness = getBestFitness();
    }

    // Update all jellies
    updateVector(jellies, deltaTime);
    
}

void EnemyManager::render(sf::RenderWindow& window)
{
    // Render all jellies
    renderVector(jellies, window);

    // Jellies spawning radius should be proportional to the window size
    spawnDistance = std::sqrt(window.getView().getSize().x * window.getView().getSize().x + window.getView().getSize().y * window.getView().getSize().y) / 2.f;
}

std::size_t EnemyManager::jelliesCount() { return jellies.size(); }

Jelly& EnemyManager::jellyAt(std::size_t i) { return *jellies.at(i); }

void EnemyManager::addJelly(Chromosome chromosome)
{
    sf::Vector2f distance = sf::Vector2f({spawnDistance, 0}).rotatedBy(sf::degrees(randomizer.randomInt(0, 359)));
    jellies.push_back(std::make_unique<Jelly>(player.getBounds().position + distance, assets, projectiles, chromosome, currentLevel, perks, audio));
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
    // Calculate total damage (which is the fitness function)
    int totalDamage = 0;

    for (const auto& jelly : jellies)
    {
        totalDamage += jelly->getChromosome().getDamageInflicted();
    }

    // If all the jellies suck everyone has the same chance 
    if (totalDamage == 0)
    {
        std::size_t index =
            randomizer.randomIndex(0, jellies.size() - 1);

        return jellies.at(index)->getChromosome();
    }

    // Chance is proportional to damage
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

    // Shouldn't be executed
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

void EnemyManager::reset()
{
    jellies.clear();
    currentLevel = 1.0f;
    killCount = 0;
    bestFitness = 0;
    mutationRate = baseMutationRate;
    timeToNextSpawn = spawningCooldown;
}
