#include "player.hpp"
#include <iostream>

Player::Player(sf::Vector2u windowSize, AssetManager& assets, ProjectileManager& projectiles, PerkManager& perks, AudioManager& audio)
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
      perks(perks), 
      audio(audio)
{
    // Sprites positions and origins (anchors)
    corpseSprite.setOrigin(corpseSprite.getLocalBounds().getCenter());
    legsSprite.setOrigin(corpseSprite.getOrigin());
    handsSprite.setOrigin(corpseSprite.getOrigin());

    corpseSprite.setPosition({(windowSize.x - corpseSprite.getGlobalBounds().size.x) / 2.0f, (windowSize.y - corpseSprite.getGlobalBounds().size.y) / 2.0f});
}

void Player::update(sf::Time deltaTime)
{
    if (!std::set{state::DYING, state::DEAD}.contains(currentState))
    {
        // Position and movement
        movement(deltaTime);

        corpseSprite.move(velocity * perks.getPlayerSpeedMultiplier() * deltaTime.asSeconds());

        // Right / left facing
        if (corpseSprite.getGlobalBounds().getCenter().x < mousePos.x) corpseSprite.setScale({1, 1});        
        else corpseSprite.setScale({-1, 1});
    }
    else if (currentState == state::DYING)
    {
        // During death animation, move towards the bottom of the screen and rotate
        corpseSprite.move(sf::Vector2f{0, dyingSpeed} * deltaTime.asSeconds());
        corpseSprite.rotate(dyingRotation * deltaTime.asSeconds());
    }
    // If dead / dying sprite should face right by default
    else corpseSprite.setScale({1, 1});

    // Legs and hands always copy the transformations of the corpse sprite
    legsSprite.setPosition(corpseSprite.getPosition());
    handsSprite.setPosition(corpseSprite.getPosition());
    legsSprite.setScale(corpseSprite.getScale());
    handsSprite.setScale(corpseSprite.getScale());
    legsSprite.setRotation(corpseSprite.getRotation());
    handsSprite.setRotation(corpseSprite.getRotation());

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
    // Calculate mouse position for other functions
    mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

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

    // Draw if player is not dead
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

    // If alive, render health bar
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
        audio.stopSound(chargingSoundId);

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

void Player::cancelShooting() { if (currentState == state::AIMING) currentState = state::IDLE; }

bool Player::startAiming()
{
    if (currentState == state::IDLE || currentState == state::HITTING)
    {
        currentState = state::AIMING;

        // If this shot is charged, play the sound
        if (perks.isNextBottleBoosted()) chargingSoundId = audio.addSound(assets.chargingHitSound);
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

        blockSoundId = audio.addSound(assets.playerHitSound);
    }
}

void Player::inflictDamage(int damage)
{
    health.changeHealth(-damage);

    // If health below 0, change state
    if (health.GetHealth() <= 0 && !std::set{state::DYING, state::DEAD}.contains(currentState))
    {
        currentState = state::DYING;
        
        // Stop all sounds and play dying sound
        audio.stopSound(chargingSoundId);
        audio.stopSound(blockSoundId);
        audio.addSound(assets.playerDieSound);
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

bool Player::isAlive() { return currentState != state::DEAD; }

bool Player::isDying() { return currentState == state::DYING; }