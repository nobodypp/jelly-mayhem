#include "star.hpp"


Star::Star(sf::Vector2f position, sf::Vector2f targetPos, sf::Color color, Chromosome& ownerChromosome, float level, AssetManager& assets, PerkManager& perks, AudioManager& audio)
    : flyAnimation(&assets.starFlyFrames, 10), 
      explodeAnimation(&assets.starExplodeFrames, 10), 
      sprite(flyAnimation.getCurrentFrame()), 
      ownerChromosome(&ownerChromosome), 
      perks(&perks),
      linearVelocity(ownerChromosome.getStarSpeed() * level), 
      currentState(state::FLYING), 
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
        case state::FLYING:
            sprite.move(velocity * deltaTime.asSeconds());
            flyAnimation.update(deltaTime);
            break;
        
        case state::EXPLODING:
            explodeAnimation.update(deltaTime);

            // State transition
            if (explodeAnimation.getCurrentCycle() >= 1) currentState = state::DESTROY;
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
        currentState = state::DESTROY;

        // If before was close to player, it means it was dodged
        if (wasCloseToPlayer) perks->starDodged();
    }

    switch(currentState)
    {
        case state::FLYING:
            sprite.setTexture(flyAnimation.getCurrentFrame());
            break;

        case state::EXPLODING:
            sprite.setTexture(explodeAnimation.getCurrentFrame());
            break;
    }

    if (currentState != state::DESTROY) window.draw(sprite);
}

sf::FloatRect Star::getBounds() { return sprite.getGlobalBounds(); }

int Star::getDamage() { return damage; }

void Star::registerHit()
{
    currentState = state::EXPLODING;
    explodeAnimation.restart();
    if (ownerChromosome != nullptr) ownerChromosome->changeDamageInflicted(getDamage());
    audio->addSound(assets->starHitSound);
}

bool Star::isAlive() { return currentState != state::DESTROY; }

bool Star::isColliding() { return currentState == state::FLYING; }

void Star::registerProximityToPlayer() { wasCloseToPlayer = true; }
