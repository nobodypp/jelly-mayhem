#include "projectilemanager.hpp"


ProjectileManager::ProjectileManager(TextureManager& textureManager)
    : textureManager(textureManager)
{}

void ProjectileManager::update(sf::Time deltaTime)
{
    for (std::size_t i = 0; i < bottles.size();)
    {
        bottles[i].update(deltaTime);

        if (!bottles[i].isAlive()) bottles.erase(bottles.begin() + i);
        else ++i;
    } 

    for (std::size_t i = 0; i < stars.size();)
    {
        stars[i].update(deltaTime);

        if (!stars[i].isAlive()) stars.erase(stars.begin() + i);
        else ++i;
    } 
}

void ProjectileManager::render(sf::RenderWindow& window)
{
    for (auto& bottle: bottles) bottle.render(window);
    for (auto& star: stars) star.render(window);
}

std::size_t ProjectileManager::bottleCount() { return bottles.size(); }

Bottle& ProjectileManager::bottleAt(std::size_t i) { return bottles.at(i); }

std::size_t ProjectileManager::starsCount() { return stars.size(); }

Star& ProjectileManager::starAt(std::size_t i) { return stars.at(i); }