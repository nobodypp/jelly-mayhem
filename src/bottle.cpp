#include "bottle.hpp"


Bottle::Bottle(sf::Vector2f position, sf::Vector2f mouseRelativePos, sf::Time flyingTime, AssetManager& textureManager)
    : sprite(textureManager.bottleTexture), 
      rotationVelocity(sf::degrees(360)), 
      breakingAnimation(&textureManager.bottleBreaking, 30),
      timeLeft(flyingTime),
      currentState(FLYING), 
      damageDealt(false)
{
    velocity = (mouseRelativePos - position).normalized() * linearVelocity;

    sprite.setOrigin({4.f, 6.f});
    sprite.setPosition(position);
}

void Bottle::update(sf::Time deltaTime)
{
    timeLeft -= deltaTime;
    if (timeLeft <= sf::Time::Zero && currentState == FLYING)
    {
        currentState = BREAKING;
        breakingAnimation.restart();
        sprite.setOrigin(sf::Vector2f(breakingAnimation.getCurrentFrame().getSize()) / 2.f);
    }
    else if (currentState == BREAKING)
    {
        breakingAnimation.update(deltaTime);
        if (breakingAnimation.getCurrentCycle() >= 1) currentState = DESTROY;
    }

    if (currentState == FLYING)
    {
        sprite.move(velocity * deltaTime.asSeconds());
        sprite.rotate(rotationVelocity * deltaTime.asSeconds());
    }
}

void Bottle::render(sf::RenderWindow& window)
{
    if (currentState == BREAKING) sprite.setTexture(breakingAnimation.getCurrentFrame(), true);
    if (currentState == FLYING || breakingAnimation.getCurrentCycle() < 1) window.draw(sprite);
}

void Bottle::registerHit() { damageDealt = true; }

bool Bottle::isColliding() { return (currentState == BREAKING && breakingAnimation.getCurrentFrameNumber() == 0 && damageDealt == false); }

sf::FloatRect Bottle::getBounds() { return sprite.getGlobalBounds(); }

int Bottle::getDamage() { return 10; }

bool Bottle::isAlive() { return currentState != DESTROY; }