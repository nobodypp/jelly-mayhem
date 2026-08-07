#include "playerui.hpp"


PlayerUI::PlayerUI()
    : bottleBarSize({100.f, 20.f}), 
      bottleBarMargin({20.f, 20.f}), 
      bottleBarPrimaryColor(100, 100, 100), 
      bottleBarSecondaryColor(200, 200, 200), 
      maxBottleTime(sf::seconds(1.5)), 
      bottleTime(maxBottleTime),
      bottlePrimaryBar(bottleBarSize), 
      bottleSecondaryBar(bottleBarSize), 
      bottleBarActive(false)
      
{
    bottlePrimaryBar.setFillColor(bottleBarPrimaryColor);
    bottleSecondaryBar.setFillColor(bottleBarSecondaryColor);
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
    bottlePrimaryBar.setPosition({leftBottomCorner.x + bottleBarMargin.x, leftBottomCorner.y - bottleBarMargin.y - bottleBarSize.y});
    bottleSecondaryBar.setPosition(bottlePrimaryBar.getPosition());
    bottlePrimaryBar.setSize({bottleTime.asSeconds() / maxBottleTime.asSeconds() * bottleBarSize.x, bottleBarSize.y});

    window.draw(bottleSecondaryBar);
    window.draw(bottlePrimaryBar);
}

sf::Time PlayerUI::getBottleTime() { return bottleTime; }

void PlayerUI::activateBottleBar() { bottleBarActive = true; }

void PlayerUI::resetBottleTime() { bottleBarActive = false; }