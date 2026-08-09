#include "jelly.hpp"


Jelly::Jelly(sf::Vector2f position, TextureManager& textures, ProjectileManager& projectiles, Chromosome chromosome)
    : chromosome(chromosome),
      walking(&textures.jellyWalking, 10), 
      death(&textures.jellyDying, 20),
      shooting(&textures.jellyShooting, 10),
      biting(&textures.jellyBiting, 10),
      knockback(&textures.jellyKnockback, 8),
      defaultTexture(&textures.jellyDefault),
      sprite(walking.getCurrentFrame()), 
      health(chromosome.getHealth(), textures), 
      projectiles(&projectiles), 
      currentState(WALKING), 
      hasShot(false), 
      defaultCooldownTime(sf::seconds(1.f))
{
    sprite.setOrigin(sprite.getLocalBounds().size / 2.f);
    sprite.setPosition(position);
    sprite.setColor(chromosome.getColor());
}

void Jelly::setTargetPosition(sf::Vector2f targetPos) { targetPosition = targetPos; }

void Jelly::update(sf::Time deltaTime)
{
    health.attachToPosistion(sprite.getGlobalBounds());

    switch (currentState)
    {
        case WALKING:
            sprite.move((targetPosition - sprite.getPosition()).normalized() * walkingSpeed * deltaTime.asSeconds());
            if ((targetPosition - sprite.getPosition()).length() <= shootingDistance)
            {
                currentState = SHOOTING;
                shooting.restart();
                hasShot = false;
            }
            if ((targetPosition - sprite.getPosition()).length() <= bitingDistance)
            {
                currentState = BITING;
                biting.restart();
                hasBiten = false;
            }
            break;

        case SHOOTING:
            if (shooting.getCurrentFrameNumber() == shootFrame && !hasShot)
            {
                sf::Vector2f starPositon = sprite.getPosition() + shootingTexturePosition * sprite.getScale().x;
                projectiles->addStar(starPositon, targetPosition, sprite.getColor(), chromosome);
                hasShot = true;
            }
            if (shooting.getCurrentCycle() >= 1)
            {
                currentState = COOLDOWN;
                currentCooldown = defaultCooldownTime;
            }
            break;
        
        case BITING:
            if (currentState == BITING)
            {
                sprite.move((targetPosition - sprite.getPosition()).normalized() * bitingSpeed * deltaTime.asSeconds());
                if (biting.getCurrentCycle() >= 1)
                {
                    currentState = COOLDOWN;
                    currentCooldown = defaultCooldownTime;
                }
            }
            break;
        
        case COOLDOWN:
            currentCooldown -= deltaTime;
            if (currentCooldown < sf::Time::Zero)
            {
                currentState = WALKING;
                walking.restart();
            }
            break;

        case KNOCKBACK:
            sprite.move(knockbackVelocity * deltaTime.asSeconds());
            if (knockback.getCurrentCycle() >= 1)
            {
                currentState = COOLDOWN;
                currentCooldown = defaultCooldownTime;
            }
            break;
        
        case DYING:
            if (death.getCurrentCycle() >= 1) currentState = DEAD;
            break;
    }
}


void Jelly::render(sf::RenderWindow& window)
{
    if (targetPosition.x < sprite.getPosition().x) sprite.setScale({-1, 1});
    else sprite.setScale({1, 1});

    switch (currentState)
    {
        case WALKING:
            sprite.setTexture(walking.getCurrentFrame());
            break;
        case DYING:
            sprite.setTexture(death.getCurrentFrame());
            break;
        case SHOOTING:
            sprite.setTexture(shooting.getCurrentFrame());
            break;
        case BITING:
            sprite.setTexture(biting.getCurrentFrame());
            break;
        case COOLDOWN:
            sprite.setTexture(*defaultTexture);
            break;
        case KNOCKBACK:
            sprite.setTexture(knockback.getCurrentFrame());
            break;
    }

    if (currentState != DEAD) window.draw(sprite);

    if (currentState != DYING && currentState != DEAD) health.render(window);
}

sf::FloatRect Jelly::getBounds() { return sprite.getGlobalBounds(); }

bool Jelly::inflictDamage(int damage)
{
    health.changeHealth(-damage);
    if (health.GetHealth() <= 0 && currentState != DYING && currentState != DEAD)
    {
        currentState = DYING;
        death.restart();
        return true;
    }
    return false;
}

bool Jelly::isAlive() { return currentState != DEAD; }

bool Jelly::isColliding() { return currentState == BITING && !hasBiten; }

void Jelly::registerHit() { hasBiten = true; }

int Jelly::getDamage() { return chromosome.getBiteDamage(); }

void Jelly::registerKnockback(sf::Vector2f playerPosition)
{
    if (currentState != DEAD && currentState != DYING)
    {
        knockbackVelocity = (sprite.getPosition() - playerPosition).normalized() * knockbackSpeed;
        currentState = KNOCKBACK;
        knockback.restart();
    }
}