#include "playerui.hpp"


PlayerUI::PlayerUI(TextureManager& textures)
    : bottleBarSize({100.f, 20.f}), 
      windowMargin({20.f, 20.f}), 
      bottleBarPrimaryColor(100, 100, 100), 
      bottleBarSecondaryColor(200, 200, 200), 
      maxBottleTime(sf::seconds(1.5)), 
      bottleTime(maxBottleTime),
      bottlePrimaryBar(bottleBarSize), 
      bottleSecondaryBar(bottleBarSize), 
      bottleBarActive(false), 
      font(textures.font), 
      killText(font)
      
{
    bottlePrimaryBar.setFillColor(bottleBarPrimaryColor);
    bottleSecondaryBar.setFillColor(bottleBarSecondaryColor);

    killText.setCharacterSize(25);
}

void PlayerUI::update(sf::Time deltaTime)
{
    if (bottleBarActive) bottleTime += deltaTime;
    else bottleTime = sf::Time::Zero;
    bottleTime = std::clamp(bottleTime, sf::Time::Zero, maxBottleTime);
}

void PlayerUI::render(sf::RenderWindow& window)
{
    sf::Vector2f leftBottomCorner = {0.f, static_cast<float> (window.getSize().y)};
    bottlePrimaryBar.setPosition({leftBottomCorner.x + windowMargin.x, leftBottomCorner.y - windowMargin.y - bottleBarSize.y});
    bottleSecondaryBar.setPosition(bottlePrimaryBar.getPosition());
    bottlePrimaryBar.setSize({bottleTime.asSeconds() / maxBottleTime.asSeconds() * bottleBarSize.x, bottleBarSize.y});

    killText.setPosition({windowMargin.x, windowMargin.y});
    killText.setString("Kills: " + std::to_string(killCount));

    window.draw(bottleSecondaryBar);
    window.draw(bottlePrimaryBar);
    window.draw(killText);
}

sf::Time PlayerUI::getBottleTime() { return bottleTime; }

void PlayerUI::activateBottleBar() { bottleBarActive = true; }

void PlayerUI::resetBottleTime() { bottleBarActive = false; }

void PlayerUI::updateKillCount(int kills) { killCount = kills; }