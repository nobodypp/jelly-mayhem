#include "jelly.hpp"


Jelly::Jelly(sf::Vector2f position, AssetManager& assets, ProjectileManager& projectiles, Chromosome chromosome, float level)
    : chromosome(chromosome),
      walkingAnimation(&assets.jellyWalkingFrames, 10), 
      deathAnimation(&assets.jellyDyingFrames, 20),
      shootingAnimatin(&assets.jellyShootingFrames, 10),
      bitingAnimation(&assets.jellyBitingFrames, 10),
      knockbackAnimation(&assets.jellyKnockbackFrames, 8),
      defaultTexture(&assets.jellyDefault),
      sprite(walkingAnimation.getCurrentFrame()), 
      health(chromosome.getHealth() * level, assets), 
      walkingSpeed(chromosome.getWalkingSpeed() * level),
      bitingSpeed(chromosome.getBitingSpeed() * level), 
      bitingDistance(chromosome.getBitingDistance()), 
      shootingDistance(chromosome.getShootingDistance()),
      projectiles(&projectiles), 
      currentState(WALKING), 
      hasShot(false), 
      defaultCooldownTime(sf::seconds(1.f)), 
      level(level), 
      biteSound(assets.jellyBiteSound), 
      dieSound(assets.jellyDieSound), 
      knockbackSound(assets.jellyKnockbackSound), 
      shootSound(assets.jellyShootSound)
{
    sprite.setOrigin(sprite.getLocalBounds().getCenter());
    sprite.setPosition(position);
    sprite.setColor(chromosome.getColor());
}

void Jelly::setTargetPosition(sf::Vector2f targetPos)
{
    targetPosition = targetPos;

    // If too far from the target, destroy
    // if ((targetPosition - sprite.getPosition()).length() > autoRemoveDistance) {currentState = DEAD;}

}

void Jelly::update(sf::Time deltaTime)
{
    // Update health bar
    health.attachToPosistion(sprite.getGlobalBounds());

    // Update animations
    walkingAnimation.update(deltaTime);
    deathAnimation.update(deltaTime);
    shootingAnimatin.update(deltaTime);
    bitingAnimation.update(deltaTime);
    knockbackAnimation.update(deltaTime);

    switch (currentState)
    {
        case WALKING:
            sprite.move((targetPosition - sprite.getPosition()).normalized() * walkingSpeed * deltaTime.asSeconds());

            // State transtitions
            if ((targetPosition - sprite.getPosition()).length() <= shootingDistance)
            {
                currentState = SHOOTING;
                shootingAnimatin.restart();
                hasShot = false;
                
                shootSound.play();
            }
            if ((targetPosition - sprite.getPosition()).length() <= bitingDistance)
            {
                currentState = BITING;
                bitingAnimation.restart();
                hasBiten = false;

                biteSound.play();
            }
            break;

        case SHOOTING:
            // If correct animation frame, create projectile
            if (shootingAnimatin.getCurrentFrameNumber() >= shootFrame && !hasShot)
            {
                sf::Vector2f starPositon = sprite.getPosition() + shootingTexturePosition * sprite.getScale().x;
                projectiles->addStar(starPositon, targetPosition, sprite.getColor(), chromosome, level);
                hasShot = true;
            }

            // State transition
            if (shootingAnimatin.getCurrentCycle() >= 1)
            {
                currentState = COOLDOWN;
                currentCooldown = defaultCooldownTime;
            }
            break;
        
        case BITING:
            sprite.move((targetPosition - sprite.getPosition()).normalized() * bitingSpeed * deltaTime.asSeconds());

            // State transition
            if (bitingAnimation.getCurrentCycle() >= 1)
            {
                currentState = COOLDOWN;
                currentCooldown = defaultCooldownTime;
            }
            break;
        
        case COOLDOWN:
            currentCooldown -= deltaTime;

            // State transition
            if (currentCooldown < sf::Time::Zero)
            {
                currentState = WALKING;
                walkingAnimation.restart();
            }
            break;

        case KNOCKBACK:
            sprite.move(knockbackVelocity * deltaTime.asSeconds());

            // State transition
            if (knockbackAnimation.getCurrentCycle() >= 1)
            {
                currentState = COOLDOWN;
                currentCooldown = defaultCooldownTime;
            }
            break;
        
        case DYING:
            // State transition
            if (deathAnimation.getCurrentCycle() >= 1) currentState = DEAD;
            break;
    }
}


void Jelly::render(sf::RenderWindow& window)
{
    // Set facing left / right
    if (targetPosition.x < sprite.getPosition().x) sprite.setScale({-1, 1});
    else sprite.setScale({1, 1});

    // Set texture
    switch (currentState)
    {
        case WALKING:
            sprite.setTexture(walkingAnimation.getCurrentFrame());
            break;
        case DYING:
            sprite.setTexture(deathAnimation.getCurrentFrame());
            break;
        case SHOOTING:
            sprite.setTexture(shootingAnimatin.getCurrentFrame());
            break;
        case BITING:
            sprite.setTexture(bitingAnimation.getCurrentFrame());
            break;
        case COOLDOWN:
            sprite.setTexture(*defaultTexture);
            break;
        case KNOCKBACK:
            sprite.setTexture(knockbackAnimation.getCurrentFrame());
            break;
    }

    if (currentState != DEAD) window.draw(sprite);

    if (std::set{DYING, DEAD}.contains(currentState)) health.render(window);

    autoRemoveDistance = std::sqrt(window.getSize().x * window.getSize().x + window.getSize().y * window.getSize().y) / 1.2;
}

sf::FloatRect Jelly::getBounds() { return sprite.getGlobalBounds(); }

bool Jelly::inflictDamage(int damage)
{
    health.changeHealth(-damage);

    // If health below 0, die
    if (health.GetHealth() <= 0 && currentState != DYING && currentState != DEAD)
    {
        shootSound.stop();
        biteSound.stop();
        knockbackSound.stop();
        dieSound.play();

        currentState = DYING;
        deathAnimation.restart();
        return true;

    }

    return false;
}

bool Jelly::isAlive() { return currentState != DEAD || dieSound.getStatus() == sf::SoundSource::Status::Playing; }

bool Jelly::isColliding() { return currentState == BITING && !hasBiten; }

void Jelly::registerHit()
{
    hasBiten = true;
    chromosome.changeDamageInflicted(getDamage());
}

int Jelly::getDamage() { return chromosome.getBiteDamage() * level; }

void Jelly::registerKnockback(sf::Vector2f playerPosition)
{
    // If not dead, change state
    if (std::set{DYING, DEAD}.contains(currentState))
    {
        knockbackVelocity = (sprite.getPosition() - playerPosition).normalized() * knockbackSpeed;
        currentState = KNOCKBACK;
        knockbackAnimation.restart();

        biteSound.stop();
        knockbackSound.play();
    }
}

void Jelly::move(sf::Vector2f translation) { sprite.move(translation); }

Chromosome Jelly::getChromosome() { return chromosome; }