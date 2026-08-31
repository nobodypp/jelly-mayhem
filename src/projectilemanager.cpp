#include "projectilemanager.hpp"


ProjectileManager::ProjectileManager(AssetManager& assets, PerkManager& perks, AudioManager& audio)
    : assets(assets), 
      perks(perks), 
      audio(audio)
{}

void ProjectileManager::update(sf::Time deltaTime)
{
    updateVector(bottles, deltaTime);
    updateVector(stars, deltaTime);
}

void ProjectileManager::render(sf::RenderWindow& window)
{
    renderVector(bottles, window);
    renderVector(stars, window);
}

std::size_t ProjectileManager::bottleCount() { return bottles.size(); }

Bottle& ProjectileManager::bottleAt(std::size_t i) { return *bottles.at(i); }

std::size_t ProjectileManager::starsCount() { return stars.size(); }

Star& ProjectileManager::starAt(std::size_t i) { return *stars.at(i); }

void ProjectileManager::reset()
{
    bottles.clear();
    stars.clear();
}
