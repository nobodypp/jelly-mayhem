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
      currentState(State::WALKING), 
      hasShot(false), 
      defaultCooldownTime(sf::seconds(1.f)), 
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
    if ((targetPosition - sprite.getPosition()).length() > autoRemoveDistance) {currentState = State::DEAD;}

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
        case State::WALKING:
            sprite.move((targetPosition - sprite.getPosition()).normalized() * walkingSpeed * deltaTime.asSeconds());

            // State transtitions
            if ((targetPosition - sprite.getPosition()).length() <= shootingDistance)
            {
                currentState = State::SHOOTING;
                shootingAnimatin.restart();
                hasShot = false;
                
                shootSoundId = audio->addSound(assets->jellyShootSound);
            }
            if ((targetPosition - sprite.getPosition()).length() <= bitingDistance)
            {
                currentState = State::BITING;
                bitingAnimation.restart();
                hasBiten = false;

                biteSoundId = audio->addSound(assets->jellyBiteSound);
            }
            break;

        case State::SHOOTING:
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
                currentState = State::COOLDOWN;
                currentCooldown = defaultCooldownTime;
            }
            break;
        
        case State::BITING:
            {
                sf::Vector2f distance = targetPosition - sprite.getPosition();
                if (bitingSpeed * deltaTime.asSeconds() > distance.length()) sprite.setPosition(targetPosition);
                else sprite.move(distance.normalized() * bitingSpeed * deltaTime.asSeconds());

                // State transition
                if (bitingAnimation.getCurrentCycle() >= 1)
                {
                    currentState = State::COOLDOWN;
                    currentCooldown = defaultCooldownTime;
                }
                
                break;
            }
        
        case State::COOLDOWN:
            currentCooldown -= deltaTime;

            // State transition
            if (currentCooldown < sf::Time::Zero)
            {
                currentState = State::WALKING;
                walkingAnimation.restart();
            }
            break;

        case State::KNOCKBACK:
            sprite.move(knockbackVelocity * deltaTime.asSeconds());

            // State transition
            if (knockbackAnimation.getCurrentCycle() >= 1)
            {
                currentState = State::COOLDOWN;
                currentCooldown = defaultCooldownTime;
            }
            break;
        
        case State::DYING:
            // State transition
            if (deathAnimation.getCurrentCycle() >= 1) currentState = State::DEAD;
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
        case State::WALKING:
            sprite.setTexture(walkingAnimation.getCurrentFrame());
            break;
        case State::DYING:
            sprite.setTexture(deathAnimation.getCurrentFrame());
            break;
        case State::SHOOTING:
            sprite.setTexture(shootingAnimatin.getCurrentFrame());
            break;
        case State::BITING:
            sprite.setTexture(bitingAnimation.getCurrentFrame());
            break;
        case State::COOLDOWN:
            sprite.setTexture(*defaultTexture);
            break;
        case State::KNOCKBACK:
            sprite.setTexture(knockbackAnimation.getCurrentFrame());
            break;
    }

    if (currentState != State::DEAD) window.draw(sprite);

    if (!std::set{State::DYING, State::DEAD}.contains(currentState)) health.render(window);

    autoRemoveDistance = std::sqrt(window.getView().getSize().x * window.getView().getSize().x + window.getView().getSize().y * window.getView().getSize().y) / 1.2;
}

sf::FloatRect Jelly::getBounds() { return sprite.getGlobalBounds(); }

bool Jelly::inflictDamage(int damage)
{
    health.changeHealth(-damage);

    // If health below 0, die
    if (health.getHealth() <= 0 && currentState != State::DYING && currentState != State::DEAD)
    {
        audio->stopSound(shootSoundId);
        audio->stopSound(biteSoundId);
        audio->stopSound(knockbackSoundId);
        audio->addSound(assets->jellyDieSound);

        currentState = State::DYING;
        deathAnimation.restart();

        return true;
    }

    return false;
}

bool Jelly::isAlive() { return currentState != State::DEAD; }

bool Jelly::isColliding() { return currentState == State::BITING && !hasBiten; }

void Jelly::registerHit()
{
    hasBiten = true;
    chromosome.changeDamageInflicted(getDamage());
}

int Jelly::getDamage() { return chromosome.getBiteDamage() * level; }

void Jelly::registerKnockback(sf::Vector2f playerPosition)
{
    // If not dead, change State
    if (!std::set{State::DYING, State::DEAD}.contains(currentState))
    {
        knockbackVelocity = (sprite.getPosition() - playerPosition).normalized() * knockbackSpeed;
        currentState = State::KNOCKBACK;
        knockbackAnimation.restart();

        audio->stopSound(biteSoundId);
        knockbackSoundId = audio->addSound(assets->jellyKnockbackSound);
    }
}

void Jelly::move(sf::Vector2f translation) { sprite.move(translation); }

Chromosome Jelly::getChromosome() { return chromosome; }

bool Jelly::isDuringKnockback() { return currentState == State::KNOCKBACK; }

bool Jelly::isDying() { return currentState == State::DYING; }