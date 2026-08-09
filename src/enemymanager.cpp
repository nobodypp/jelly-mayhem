#include "enemymanager.hpp"


EnemyManager::EnemyManager(TextureManager& textures, Player& player, ProjectileManager& projectiles)
    : textures(textures), 
      projectiles(projectiles),
      player(player), 
      killCount(0), 
      spawningCooldown(sf::seconds(3.f)),
      timeToNextSpawn(spawningCooldown)
{
    for (int i = 0; i < jelliesPopulationSize; i++)
    {
        addJelly();
    }
}

void EnemyManager::update(sf::Time deltaTime)
{
    timeToNextSpawn -= deltaTime;
    if (timeToNextSpawn <= sf::Time::Zero)
    {
        timeToNextSpawn = spawningCooldown;
        addJelly();
    }
    for (std::size_t i = 0; i < jellies.size();)
    {
        if (jellies[i].isDead())
        {
            killCount++;
            jellies.erase(jellies.begin() + i);
            // addJelly();
        }
        else
        {
            jellies[i].setTargetPosition(player.getBounds().getCenter());
            jellies[i].update(deltaTime);
            i++;
        }
    } 
}

void EnemyManager::render(sf::RenderWindow& window)
{
    for (auto& jelly: jellies) jelly.render(window);
}

std::size_t EnemyManager::jelliesCount() { return jellies.size(); }

Jelly& EnemyManager::jellyAt(std::size_t i) { return jellies.at(i); }

void EnemyManager::addJelly()
{
    sf::Vector2f distance = sf::Vector2f({spawnDistance, 0}).rotatedBy(sf::degrees(rand() % 360));
    jellies.emplace_back(player.getBounds().position + distance, textures, projectiles);
}