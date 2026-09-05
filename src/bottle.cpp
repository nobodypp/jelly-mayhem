#include "bottle.hpp"


Bottle::Bottle(sf::Vector2f position, sf::Vector2f mouseRelativePos, sf::Time flyingTime, AssetManager& assets, PerkManager& perks, AudioManager& audio)
    : sprite(assets.bottleTexture), 
      breakingAnimation(&assets.bottleBreakingFrames, 30),
      timeLeft(flyingTime),
      perks(&perks), 
      damageMultiplier(perks.claimBoostedBottle()), 
      audio(&audio), 
      assets(&assets)
{
    // Set sprite position and velocity
    velocity = (mouseRelativePos - position).normalized() * linearVelocity;

    sprite.setOrigin(spriteOrigin);
    sprite.setPosition(position);
}

void Bottle::update(sf::Time deltaTime)
{
    timeLeft -= deltaTime;

    switch (currentState)
    {
        case State::Flying:
            // Move and rotate the sprite
            sprite.move(velocity * deltaTime.asSeconds());
            sprite.rotate(rotationVelocity * deltaTime.asSeconds());

            // State transition
            if (timeLeft <= sf::Time::Zero)
            {
                currentState = State::Breaking;
                breakingAnimation.restart();
                sprite.setOrigin(sf::Vector2f(breakingAnimation.getCurrentFrame().getSize()) / 2.f);

                audio->addSound(assets->bottleBreakSound);
            }
            break;
        
        case State::Breaking:
            breakingAnimation.update(deltaTime);

            // State transition
            if (breakingAnimation.getCurrentCycle() >= 1) currentState = State::Destroy;
            break;
    }
}

void Bottle::render(sf::RenderWindow& window)
{
    if (currentState == State::Breaking) sprite.setTexture(breakingAnimation.getCurrentFrame(), true);
    if (currentState != State::Destroy) window.draw(sprite);
}

void Bottle::registerHit() { damageDealt = true; }

bool Bottle::isColliding() { return (currentState == State::Breaking && breakingAnimation.getCurrentFrameNumber() == 0 && damageDealt == false); }

sf::FloatRect Bottle::getBounds()
{
    sf::FloatRect bounds = sprite.getGlobalBounds();

    sf::Vector2f center = bounds.getCenter();
    sf::Vector2f size = bounds.size * perks->getBottleBoundsScale();

    return sf::FloatRect(center - size / 2.f, size);
}

int Bottle::getDamage() { return baseDamage * damageMultiplier; }

bool Bottle::isAlive() { return currentState != State::Destroy; }