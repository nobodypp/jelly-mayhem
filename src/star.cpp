#include "star.hpp"


Star::Star(sf::Vector2f position, sf::Vector2f targetPos, sf::Color color, Chromosome& ownerChromosome, float level, AssetManager& assets, PerkManager& perks, AudioManager& audio)
    : flyAnimation(&assets.starFlyFrames, 10), 
      explodeAnimation(&assets.starExplodeFrames, 10), 
      sprite(flyAnimation.getCurrentFrame()), 
      ownerChromosome(&ownerChromosome), 
      perks(&perks),
      linearVelocity(ownerChromosome.getStarSpeed() * level), 
      currentState(State::FLYING), 
      wasCloseToPlayer(false), 
      audio(&audio), 
      assets(&assets), 
      damage(ownerChromosome.getStarDamage() * level)
{
    // Velocity vector
    velocity = (targetPos - position).normalized() * linearVelocity;

    // Sprite initial traits
    sprite.setOrigin(spriteOrigin);
    sprite.setPosition(position);
    sprite.setRotation(velocity.angle());
    sprite.setColor(color);
}

void Star::update(sf::Time deltaTime)
{
    switch(currentState)
    {
        case State::FLYING:
            sprite.move(velocity * deltaTime.asSeconds());
            flyAnimation.update(deltaTime);
            break;
        
        case State::EXPLODING:
            explodeAnimation.update(deltaTime);

            // State transition
            if (explodeAnimation.getCurrentCycle() >= 1) currentState = State::DESTROY;
            break;
    }
}

void Star::render(sf::RenderWindow& window)
{
    // Destroy object if out of view
    if (!sprite.getGlobalBounds().findIntersection({
    window.getView().getCenter() - window.getView().getSize() / 2.f,
    window.getView().getSize()}))
    {
        currentState = State::DESTROY;

        // If before was close to player, it means it was dodged
        if (wasCloseToPlayer) perks->registerStarDodged();
    }

    switch(currentState)
    {
        case State::FLYING:
            sprite.setTexture(flyAnimation.getCurrentFrame());
            break;

        case State::EXPLODING:
            sprite.setTexture(explodeAnimation.getCurrentFrame());
            break;
    }

    if (currentState != State::DESTROY) window.draw(sprite);
}

sf::FloatRect Star::getBounds() { return sprite.getGlobalBounds(); }

int Star::getDamage() { return damage; }

void Star::registerHit()
{
    currentState = State::EXPLODING;
    explodeAnimation.restart();
    if (ownerChromosome != nullptr) ownerChromosome->changeDamageInflicted(getDamage());
    audio->addSound(assets->starHitSound);
}

bool Star::isAlive() { return currentState != State::DESTROY; }

bool Star::isColliding() { return currentState == State::FLYING; }

void Star::registerProximityToPlayer() { wasCloseToPlayer = true; }
