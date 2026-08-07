#include "damagetextmanager.hpp"


DamageTextManager::DamageTextManager(TextureManager& textures)
    : textures(textures)
{}

void DamageTextManager::update(sf::Time deltaTime)
{
    for (std::size_t i = 0; i < texts.size();)
    {
        texts[i].update(deltaTime);

        if (!texts[i].isAlive()) texts.erase(texts.begin() + i);
        else ++i;
    } 
}

void DamageTextManager::render(sf::RenderWindow& window)
{
    for (auto& text : texts) text.render(window);
}