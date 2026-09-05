#include "damagetext.hpp"


DamageText::DamageText(AssetManager& assets, int damage, bool positive, sf::FloatRect targetBounds, std::string description)
    : text(assets.font)
{
    // Set text position, size, string and color
    text.setCharacterSize(characterSize);
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
    std::uint8_t a = static_cast<std::uint8_t> (timeLeft.asSeconds() / absoluteTime.asSeconds() * 255);

    // Fil color transparency
    sf::Color color = text.getFillColor();
    color.a = a;
    text.setFillColor(color);

    // Outline color transparency
    color = text.getOutlineColor();
    color.a = a;
    text.setOutlineColor(color);

    window.draw(text);
}

bool DamageText::isAlive() { return timeLeft > sf::Time::Zero; }