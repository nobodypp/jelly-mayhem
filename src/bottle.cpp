#include "bottle.hpp"


Bottle::Bottle(sf::Vector2f position, sf::Vector2f mouseRelativePos, sf::Time flyingTime, AssetManager& assets)
    : sprite(assets.bottleTexture), 
      rotationVelocity(sf::degrees(360)), 
      breakingAnimation(&assets.bottleBreakingFrames, 30),
      timeLeft(flyingTime),
      currentState(FLYING), 
      damageDealt(false), 
      breakingSound(assets.bottleBreakSound)
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
        case FLYING:
            // Move and rotate the sprite
            sprite.move(velocity * deltaTime.asSeconds());
            sprite.rotate(rotationVelocity * deltaTime.asSeconds());

            // State transition
            if (timeLeft <= sf::Time::Zero)
            {
                currentState = BREAKING;
                breakingAnimation.restart();
                sprite.setOrigin(sf::Vector2f(breakingAnimation.getCurrentFrame().getSize()) / 2.f);

                breakingSound.play();
            }
            break;
        
        case BREAKING:
            breakingAnimation.update(deltaTime);

            // State transition
            if (breakingAnimation.getCurrentCycle() >= 1) currentState = DESTROY;
            break;
    }
}

void Bottle::render(sf::RenderWindow& window)
{
    if (currentState == BREAKING) sprite.setTexture(breakingAnimation.getCurrentFrame(), true);
    if (currentState != DESTROY) window.draw(sprite);
}

void Bottle::registerHit() { damageDealt = true; }

bool Bottle::isColliding() { return (currentState == BREAKING && breakingAnimation.getCurrentFrameNumber() == 0 && damageDealt == false); }

sf::FloatRect Bottle::getBounds() { return sprite.getGlobalBounds(); }

int Bottle::getDamage() { return 10; }

bool Bottle::isAlive() { return currentState != DESTROY || breakingSound.getStatus() == sf::SoundSource::Status::Playing; }