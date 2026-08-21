#include "bottle.hpp"


Bottle::Bottle(sf::Vector2f position, sf::Vector2f mouseRelativePos, sf::Time flyingTime, AssetManager& assets, PerkManager& perks)
    : sprite(assets.bottleTexture), 
      rotationVelocity(sf::degrees(360)), 
      breakingAnimation(&assets.bottleBreakingFrames, 30),
      timeLeft(flyingTime),
      currentState(state::FLYING), 
      damageDealt(false), 
      breakingSound(assets.bottleBreakSound), 
      perks(&perks), 
      damageMultiplier(perks.getRewardFromSingleKill())
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
        case state::FLYING:
            // Move and rotate the sprite
            sprite.move(velocity * deltaTime.asSeconds());
            sprite.rotate(rotationVelocity * deltaTime.asSeconds());

            // State transition
            if (timeLeft <= sf::Time::Zero)
            {
                currentState = state::BREAKING;
                breakingAnimation.restart();
                sprite.setOrigin(sf::Vector2f(breakingAnimation.getCurrentFrame().getSize()) / 2.f);

                breakingSound.play();
            }
            break;
        
        case state::BREAKING:
            breakingAnimation.update(deltaTime);

            // State transition
            if (breakingAnimation.getCurrentCycle() >= 1) currentState = state::DESTROY;
            break;
    }
}

void Bottle::render(sf::RenderWindow& window)
{
    if (currentState == state::BREAKING) sprite.setTexture(breakingAnimation.getCurrentFrame(), true);
    if (currentState != state::DESTROY) window.draw(sprite);
}

void Bottle::registerHit() { damageDealt = true; }

bool Bottle::isColliding() { return (currentState == state::BREAKING && breakingAnimation.getCurrentFrameNumber() == 0 && damageDealt == false); }

sf::FloatRect Bottle::getBounds()
{
    sf::FloatRect bounds = sprite.getGlobalBounds();

    sf::Vector2f center = bounds.getCenter();
    sf::Vector2f size = bounds.size * perks->getBottleBoundsScale();

    return sf::FloatRect(center - size / 2.f, size);
}

int Bottle::getDamage() { return baseDamage * damageMultiplier; }

bool Bottle::isAlive() { return currentState != state::DESTROY || breakingSound.getStatus() == sf::SoundSource::Status::Playing; }