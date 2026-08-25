#include "player.hpp"
#include <iostream>

Player::Player(sf::Vector2u windowSize, AssetManager& assets, ProjectileManager& projectiles, PerkManager& perks)
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
      currentState(state::IDLE), 
      dyingRotation(sf::degrees(400)), 
      dieSound(assets.playerDieSound), 
      blockSound(assets.playerHitSound), 
      charginShotSound(assets.charginHitSound),
      perks(perks)
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
    if (!std::set{state::DYING, state::DEAD}.contains(currentState))
    {
        movement(deltaTime);

        corpseSprite.move(velocity * perks.getPlayerSpeedMultiplier() * deltaTime.asSeconds());
        legsSprite.setPosition(corpseSprite.getPosition());
        handsSprite.setPosition(corpseSprite.getPosition());
    }
    else if (currentState == state::DYING)
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
        case state::THROWING:
            if (bottleThrow.getCurrentCycle() >= 1) currentState = state::IDLE;
            break;
        
        case state::HITTING:
            if (bottleHit.getCurrentCycle() >= 1) currentState = state::IDLE;
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
    if (!std::set{state::DYING, state::DEAD}.contains(currentState))
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
        case state::THROWING:
            handsSprite.setTexture(bottleThrow.getCurrentFrame());
            break;
        
        case state::HITTING:
            handsSprite.setTexture(bottleHit.getCurrentFrame());
            break;

        case state::AIMING:
            handsSprite.setTexture(assets.playerHandsDefaultTexture);
            break;
        
        case state::IDLE:
            handsSprite.setTexture(assets.playerHandsDefaultTexture);
            break;
        
        case state::DYING:
            handsSprite.setTexture(assets.playerHandsDefaultTexture);
    }

    if (currentState != state::DEAD)
    {
        window.draw(legsSprite);
        window.draw(handsSprite);
        window.draw(corpseSprite);
    }

    // If death animation has ended, change state
    if (currentState == state::DYING && !corpseSprite.getGlobalBounds().findIntersection({
    window.getView().getCenter() - window.getView().getSize() / 2.f,
    window.getView().getSize()})) currentState = state::DEAD;

    // Render health bar
    if (!std::set{state::DYING, state::DEAD}.contains(currentState))
    {
        health.attachToPosistion(corpseSprite.getGlobalBounds());
        health.render(window);
    }
}

void Player::movement(sf::Time deltaTime)
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
    if (currentState == state::AIMING)
    {
        // Stop the charging sound if it was playing
        charginShotSound.stop();

        // If was aiming, shoot a bottle
        sf::Vector2f bottlePosition = corpseSprite.getGlobalBounds().position + throwHandAbsPosition * corpseSprite.getScale().x;
        projectiles.addBottle(bottlePosition, sf::Vector2f(mouseWorldPos), bottleTime);

        // Change state and reset animation
        currentState = state::THROWING;
        bottleThrow.restart();
        return true;
    }
    return false;
}

bool Player::startAiming()
{
    if (currentState == state::IDLE || currentState == state::HITTING)
    {
        currentState = state::AIMING;

        // If this shot is charged, play the sound
        if (perks.isNextBottleBoosted()) charginShotSound.play();
        return true;
    }
    return false;
}

void Player::startBlocking()
{
    if (currentState == state::IDLE)
    {
        currentState = state::HITTING;
        bottleHit.restart();

        blockSound.play();
    }
}

void Player::inflictDamage(int damage)
{
    health.changeHealth(-damage);

    // If health below 0, change state
    if (health.GetHealth() <= 0 && !std::set{state::DYING, state::DEAD}.contains(currentState))
    {
        currentState = state::DYING;
        dieSound.play();
    }
}

int Player::getMeleeDamage() { return 40; }

bool Player::isHitting() { return currentState == state::HITTING; }

int Player::succesfullParry()
{
    int healing = static_cast<int>(baseHealing * perks.getHealingMultiplier());
    health.changeHealth(healing);
    return healing;
}

bool Player::isAlive() { return currentState != state::DEAD || dieSound.getStatus() == sf::SoundSource::Status::Playing; }

bool Player::isDying() { return currentState == state::DYING; }