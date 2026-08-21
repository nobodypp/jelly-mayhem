#include "player.hpp"
#include <iostream>

Player::Player(sf::Vector2u windowSize, AssetManager& assets, ProjectileManager& projectiles)
    : assets(assets),
      corpseSprite(assets.playerCorpseDefaultTexture),
      legsSprite(assets.playerLegsDefaultTexture),
      handsSprite(assets.playerHandsDefaultTexture),
      velocity(0.f, 0.0f), 
      maxVelocity(200.f, 200.f), 
      isRunning(false), 
      health(500, assets),
      throwHandAbsPosition(46, 36), 
      legsRunning(&assets.legsRunningFrames, 14), 
      bottleThrow(&assets.bottleThrowingFrames, 18), 
      bottleHit(&assets.bottleHitFrames, 20), 
      projectiles(projectiles), 
      currentState(IDLE), 
      dyingRotation(sf::degrees(400)), 
      dieSound(assets.playerDieSound), 
      blockSound(assets.playerHitSound)
{
    // Sprites positions and origins (anchors)
    corpseSprite.setOrigin(corpseSprite.getLocalBounds().getCenter());
    legsSprite.setOrigin(corpseSprite.getOrigin());
    handsSprite.setOrigin(corpseSprite.getOrigin());

    corpseSprite.setPosition({(windowSize.x - corpseSprite.getGlobalBounds().size.x) / 2.0f, (windowSize.y - corpseSprite.getGlobalBounds().size.y) / 2.0f});
    legsSprite.setPosition(corpseSprite.getPosition());
    handsSprite.setPosition(corpseSprite.getPosition());
}

void Player::update(sf::Time deltaTime)
{
    // Position and movement update
    if (!std::set{DYING, DEAD}.contains(currentState))
    {
        handleInput(deltaTime);

        corpseSprite.move(velocity * deltaTime.asSeconds());
        legsSprite.move(velocity * deltaTime.asSeconds());
        handsSprite.move(velocity * deltaTime.asSeconds());
    }
    else if (currentState == DYING)
    {
        corpseSprite.move(sf::Vector2f{0, dyingSpeed} * deltaTime.asSeconds());
        legsSprite.move(sf::Vector2f{0, dyingSpeed} * deltaTime.asSeconds());
        handsSprite.move(sf::Vector2f{0, dyingSpeed} * deltaTime.asSeconds());

        corpseSprite.rotate(dyingRotation * deltaTime.asSeconds());
        legsSprite.rotate(dyingRotation * deltaTime.asSeconds());
        handsSprite.rotate(dyingRotation * deltaTime.asSeconds());
    }

    // State transitions
    switch (currentState)
    {
        case THROWING:
            if (bottleThrow.getCurrentCycle() >= 1) currentState = IDLE;
            break;
        
        case HITTING:
            if (bottleHit.getCurrentCycle() >= 1) currentState = IDLE;
            break;
    }

    // Animation update
    legsRunning.update(deltaTime);
    bottleThrow.update(deltaTime);
    bottleHit.update(deltaTime);
}

void Player::render(sf::RenderWindow& window)
{
    // Right / left facing
    if (!std::set{DYING, DEAD}.contains(currentState))
    {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        if (corpseSprite.getGlobalBounds().getCenter().x < mousePos.x)
        {
            corpseSprite.setScale({1, 1});
            legsSprite.setScale({1, 1});
            handsSprite.setScale({1, 1});
        }
        else 
        {
            corpseSprite.setScale({-1, 1});
            legsSprite.setScale({-1, 1});
            handsSprite.setScale({-1, 1});
        }
    }
    else
    {
        corpseSprite.setScale({1, 1});
        legsSprite.setScale({1, 1});
        handsSprite.setScale({1, 1});
    }

    // Legs texture
    if (isRunning) legsSprite.setTexture(legsRunning.getCurrentFrame());
    else legsSprite.setTexture(assets.playerLegsDefaultTexture);

    // Hands texture
    switch (currentState)
    {
        case THROWING:
            handsSprite.setTexture(bottleThrow.getCurrentFrame());
            break;
        
        case HITTING:
            handsSprite.setTexture(bottleHit.getCurrentFrame());
            break;

        case AIMING:
            handsSprite.setTexture(assets.playerHandsDefaultTexture);
            break;
        
        case IDLE:
            handsSprite.setTexture(assets.playerHandsDefaultTexture);
            break;
        
        case DYING:
            handsSprite.setTexture(assets.playerHandsDefaultTexture);
    }

    if (currentState != DEAD)
    {
        window.draw(legsSprite);
        window.draw(handsSprite);
        window.draw(corpseSprite);
    }

    // If death animation has ended, change state
    if (currentState == DYING && !corpseSprite.getGlobalBounds().findIntersection({
    window.getView().getCenter() - window.getView().getSize() / 2.f,
    window.getView().getSize()})) currentState = DEAD;

    // Render health bar
    if (!std::set{DYING, DEAD}.contains(currentState))
    {
        health.attachToPosistion(corpseSprite.getGlobalBounds());
        health.render(window);
    }
}

void Player::handleInput(sf::Time deltaTime)
{
    bool slowDown = true;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    {
        velocity += sf::Vector2f{-acceleration, 0} * deltaTime.asSeconds();
        slowDown = false;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    {
        velocity += sf::Vector2f{acceleration, 0} * deltaTime.asSeconds();
        slowDown = false;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
    {
        velocity += sf::Vector2f{0, -acceleration} * deltaTime.asSeconds();
        slowDown = false;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
    {
        velocity += sf::Vector2f{0, acceleration} * deltaTime.asSeconds();
        slowDown = false;
    }
    
    // If not moving, apply friction
    if (slowDown)
    {
        isRunning = false;

        if (velocity.length() > 0.f)
        {
            velocity -= velocity.normalized() * speedFriction * deltaTime.asSeconds();
        }
    }
    else
    {
        // If started running, restart animation
        if (!isRunning) legsRunning.restart();
        isRunning = true;
    }
    
    // Velocity limit
    velocity.x = std::clamp(velocity.x, -maxVelocity.x, maxVelocity.x);
    velocity.y = std::clamp(velocity.y, -maxVelocity.y, maxVelocity.y);
}

sf::FloatRect Player::getBounds() { return corpseSprite.getGlobalBounds(); }

bool Player::shootBottle(sf::Vector2f mouseWorldPos, sf::Time bottleTime)
{
    if (currentState == AIMING)
    {
        sf::Vector2f bottlePosition = corpseSprite.getGlobalBounds().position + throwHandAbsPosition * corpseSprite.getScale().x;
        projectiles.addBottle(bottlePosition, sf::Vector2f(mouseWorldPos), bottleTime);
        currentState = THROWING;
        bottleThrow.restart();
        return true;
    }
    return false;
}

bool Player::startAiming()
{
    if (currentState == IDLE)
    {
        currentState = AIMING;
        return true;
    }
    return false;
}

void Player::startBlocking()
{
    currentState = HITTING;
    bottleHit.restart();

    blockSound.play();
}

void Player::inflictDamage(int damage)
{
    health.changeHealth(-damage);

    // If health below 0, change state
    if (health.GetHealth() <= 0)
    {
        currentState = DYING;
        dieSound.play();
    }
}

int Player::getMeleeDamage() { return 40; }

bool Player::isHitting() { return currentState == HITTING; }

void Player::succesfullParry() { health.changeHealth(20); }

bool Player::isAlive() { return currentState != DEAD || dieSound.getStatus() == sf::SoundSource::Status::Playing; }

bool Player::isDying() { return currentState == DYING; }