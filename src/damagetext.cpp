#include "damagetext.hpp"


DamageText::DamageText(AssetManager& assets, int damage, bool positive, sf::FloatRect targetBounds, std::string description)
    : text(assets.font), 
      absoluteTime(sf::seconds(1.f)),
      timeLeft(absoluteTime)
{
    text.setCharacterSize(20);
    text.setString(description + (damage < 0 ? "+" : "") +  std::to_string(-damage));
    text.setFillColor(positive ? sf::Color::Green : sf::Color::Red);
    sf::Vector2f deviation = sf::Vector2f(spawnRadius, 0.f).rotatedBy(sf::degrees(rand() % 360));
    sf::Vector2f targetPosition = targetBounds.getCenter();
    text.setPosition(targetBounds.getCenter() + deviation);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(1.f);
}

void DamageText::update(sf::Time deltaTime)
{
    timeLeft -= deltaTime;
    timeLeft = std::max(timeLeft, sf::Time::Zero);
}

void DamageText::render(sf::RenderWindow& window)
{
    sf::Color color = text.getFillColor();
    color.a = static_cast<int> (timeLeft.asSeconds() / absoluteTime.asSeconds() * 255);
    text.setFillColor(color);
    window.draw(text);
}

bool DamageText::isAlive() { return timeLeft > sf::Time::Zero; }