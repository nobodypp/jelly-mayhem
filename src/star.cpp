#include "star.hpp"


Star::Star(sf::Vector2f position, sf::Vector2f targetPos, sf::Color color, Chromosome& ownerChromosome, float level, AssetManager& assets)
    : flyAnimation(&assets.starFlyFrames, 10), 
      explodeAnimation(&assets.starExplodeFrames, 10), 
      sprite(flyAnimation.getCurrentFrame()), 
      ownerChromosome(&ownerChromosome), 
      linearVelocity(ownerChromosome.getStarSpeed() * level), 
      level(level), 
      hitSound(assets.starHitSound), 
      currentState(FLYING)
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
        case FLYING:
            sprite.move(velocity * deltaTime.asSeconds());
            flyAnimation.update(deltaTime);
            break;
        
        case EXPLODING:
            explodeAnimation.update(deltaTime);

            // State transition
            if (explodeAnimation.getCurrentCycle() >= 1) currentState = DESTROY;
            break;
    }
}

void Star::render(sf::RenderWindow& window)
{
    // Destroy object if out of view
    if (!sprite.getGlobalBounds().findIntersection({
    window.getView().getCenter() - window.getView().getSize() / 2.f,
    window.getView().getSize()})) currentState = DESTROY;

    switch(currentState)
    {
        case FLYING:
            sprite.setTexture(flyAnimation.getCurrentFrame());
            break;

        case EXPLODING:
            sprite.setTexture(explodeAnimation.getCurrentFrame());
            break;
    }

    if (currentState != DESTROY) window.draw(sprite);
}

sf::FloatRect Star::getBounds() { return sprite.getGlobalBounds(); }

int Star::getDamage() { return ownerChromosome->getStarDamage() * level; }

void Star::registerHit()
{
    currentState = EXPLODING;
    explodeAnimation.restart();
    if (ownerChromosome != nullptr) ownerChromosome->changeDamageInflicted(getDamage());
    hitSound.play();
}

bool Star::isAlive() { return currentState != DESTROY || hitSound.getStatus() == sf::SoundSource::Status::Playing; }

bool Star::isColliding() { return currentState == FLYING; }