#include "healthbar.hpp"


HealthBar::HealthBar(int maxHealth, AssetManager& assets)
    : maxHealth(maxHealth), 
      health(maxHealth),
      primaryColor({230, 10, 10}), 
      secondaryColor({128, 0, 0}), 
      width(100), 
      height(14), 
      gap(0), 
      primaryBar({width, height}), 
      secodnaryBar({width, height}), 
      font(&assets.font), 
      text(*font)
{
    // Set bar traits
    primaryBar.setFillColor(primaryColor);
    secodnaryBar.setFillColor(secondaryColor);
    primaryBar.setOrigin(primaryBar.getSize() / 2.f);
    secodnaryBar.setOrigin(primaryBar.getSize() / 2.f);

    // Set text tratis
    text.setCharacterSize(12);
    text.setFillColor(sf::Color{230, 230, 230});
    text.setString(std::to_string(health) + " / " + std::to_string(maxHealth));
    text.setOrigin(text.getLocalBounds().size / 2.f);
}

int HealthBar::getHealth() { return health; }

void HealthBar::resetHealth() { health = maxHealth; }

void HealthBar::render(sf::RenderWindow& window)
{
    // Update health percentage
    primaryBar.setSize({width * static_cast<float>(health) / maxHealth, static_cast<float>(height)});

    window.draw(secodnaryBar);
    window.draw(primaryBar);

    // Update and render text
    text.setString(std::to_string(health) + " / " + std::to_string(maxHealth));
    text.setPosition(primaryBar.getPosition());
    
    window.draw(text);
}

void HealthBar::changeHealth(int change) { health = std::clamp(health + change, 0, maxHealth); }

void HealthBar::attachToPosistion(sf::FloatRect spriteBounds)
{
    primaryBar.setPosition({spriteBounds.getCenter().x, 
        spriteBounds.position.y - gap - height});
    secodnaryBar.setPosition(primaryBar.getPosition());
}