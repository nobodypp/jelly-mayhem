#include "jelly.hpp"


Jelly::Jelly(sf::Vector2f position, AssetManager& assets, ProjectileManager& projectiles, Chromosome chromosome, float level, PerkManager& perks, AudioManager& audio)
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
      level(level), 
      perks(&perks), 
      audio(&audio), 
      assets(&assets)
{
    sprite.setOrigin(sprite.getLocalBounds().getCenter());
    sprite.setPosition(position);
    sprite.setColor(chromosome.getColor());
}

void Jelly::setTargetPosition(sf::Vector2f targetPos)
{
    targetPosition = targetPos;

    // If too far from the target, destroy
    if ((targetPosition - sprite.getPosition()).length() > autoRemoveDistance) {currentState = State::Destroy;}

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
        case State::Walking:
            sprite.move((targetPosition - sprite.getPosition()).normalized() * walkingSpeed * deltaTime.asSeconds());

            // State transtitions
            if ((targetPosition - sprite.getPosition()).length() <= shootingDistance)
            {
                currentState = State::Shooting;
                shootingAnimatin.restart();
                hasShot = false;
                
                shootSoundId = audio->addSound(assets->jellyShootSound);
            }
            if ((targetPosition - sprite.getPosition()).length() <= bitingDistance)
            {
                currentState = State::Biting;
                bitingAnimation.restart();
                hasBiten = false;

                biteSoundId = audio->addSound(assets->jellyBiteSound);
            }
            break;

        case State::Shooting:
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
                currentState = State::Cooldown;
                currentCooldown = defaultCooldownTime;
            }
            break;
        
        case State::Biting:
            {
                sf::Vector2f distance = targetPosition - sprite.getPosition();
                if (bitingSpeed * deltaTime.asSeconds() > distance.length()) sprite.setPosition(targetPosition);
                else sprite.move(distance.normalized() * bitingSpeed * deltaTime.asSeconds());

                // State transition
                if (bitingAnimation.getCurrentCycle() >= 1)
                {
                    currentState = State::Cooldown;
                    currentCooldown = defaultCooldownTime;
                }
                
                break;
            }
        
        case State::Cooldown:
            currentCooldown -= deltaTime;

            // State transition
            if (currentCooldown < sf::Time::Zero)
            {
                currentState = State::Walking;
                walkingAnimation.restart();
            }
            break;

        case State::Knockback:
            sprite.move(knockbackVelocity * deltaTime.asSeconds());

            // State transition
            if (knockbackAnimation.getCurrentCycle() >= 1)
            {
                currentState = State::Cooldown;
                currentCooldown = defaultCooldownTime;
            }
            break;
        
        case State::Dying:
            // State transition
            if (deathAnimation.getCurrentCycle() >= 1) currentState = State::Destroy;
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
        case State::Walking:
            sprite.setTexture(walkingAnimation.getCurrentFrame());
            break;
        case State::Dying:
            sprite.setTexture(deathAnimation.getCurrentFrame());
            break;
        case State::Shooting:
            sprite.setTexture(shootingAnimatin.getCurrentFrame());
            break;
        case State::Biting:
            sprite.setTexture(bitingAnimation.getCurrentFrame());
            break;
        case State::Cooldown:
            sprite.setTexture(*defaultTexture);
            break;
        case State::Knockback:
            sprite.setTexture(knockbackAnimation.getCurrentFrame());
            break;
    }

    if (currentState != State::Destroy) window.draw(sprite);

    if (!std::set{State::Dying, State::Destroy}.contains(currentState)) health.render(window);

    autoRemoveDistance = std::sqrt(window.getView().getSize().x * window.getView().getSize().x + window.getView().getSize().y * window.getView().getSize().y) / 1.2;
}

sf::FloatRect Jelly::getBounds() { return sprite.getGlobalBounds(); }

bool Jelly::inflictDamage(int damage)
{
    health.changeHealth(-damage);

    // If health below 0, die
    if (health.getHealth() <= 0 && currentState != State::Dying && currentState != State::Destroy)
    {
        audio->stopSound(shootSoundId);
        audio->stopSound(biteSoundId);
        audio->stopSound(knockbackSoundId);
        audio->addSound(assets->jellyDieSound);

        currentState = State::Dying;
        deathAnimation.restart();

        return true;
    }

    return false;
}

bool Jelly::isAlive() { return currentState != State::Destroy; }

bool Jelly::isColliding() { return currentState == State::Biting && !hasBiten; }

void Jelly::registerHit()
{
    hasBiten = true;
    chromosome.changeDamageInflicted(getDamage());
}

int Jelly::getDamage() { return chromosome.getBiteDamage() * level; }

void Jelly::registerKnockback(sf::Vector2f playerPosition)
{
    // If not dead, change State
    if (!std::set{State::Dying, State::Destroy}.contains(currentState))
    {
        knockbackVelocity = (sprite.getPosition() - playerPosition).normalized() * knockbackSpeed;
        currentState = State::Knockback;
        knockbackAnimation.restart();

        audio->stopSound(biteSoundId);
        knockbackSoundId = audio->addSound(assets->jellyKnockbackSound);
    }
}

void Jelly::move(sf::Vector2f translation) { sprite.move(translation); }

Chromosome Jelly::getChromosome() { return chromosome; }

bool Jelly::isDuringKnockback() { return currentState == State::Knockback; }

bool Jelly::isDying() { return currentState == State::Dying; }