#include "player.hpp"
#include <iostream>

Player::Player(AssetManager& assets, ProjectileManager& projectiles, PerkManager& perks, AudioManager& audio)
    : assets(assets),
      perks(perks), 
      audio(audio),
      projectiles(projectiles), 
      corpseSprite(assets.playerCorpseDefaultTexture),
      legsSprite(assets.playerLegsDefaultTexture),
      handsSprite(assets.playerHandsDefaultTexture),
      legsRunning(&assets.legsRunningFrames, 14), 
      bottleThrow(&assets.bottleThrowingFrames, bottleThrowFps), 
      bottleHit(&assets.bottleHitFrames, 20), 
      health(maxHealth, assets)
{
    // Sprites positions and origins (anchors)
    corpseSprite.setOrigin(corpseSprite.getLocalBounds().getCenter());
    legsSprite.setOrigin(corpseSprite.getOrigin());
    handsSprite.setOrigin(corpseSprite.getOrigin());

    corpseSprite.setPosition({0.f, 0.f});
}

void Player::update(sf::Time deltaTime)
{
    if (!std::set{State::Dying, State::Destroy}.contains(currentState))
    {
        // Position and movement
        movement(deltaTime);

        corpseSprite.move(velocity * perks.getPlayerSpeedMultiplier() * deltaTime.asSeconds());

        // Right / left facing
        if (corpseSprite.getGlobalBounds().getCenter().x < mousePos.x) corpseSprite.setScale({1, 1});        
        else corpseSprite.setScale({-1, 1});
    }
    else if (currentState == State::Dying)
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
        case State::Throwing:
            if (bottleThrow.getCurrentCycle() >= 1) currentState = State::Idle;
            break;
        
        case State::Hitting:
            if (bottleHit.getCurrentCycle() >= 1) currentState = State::Idle;
            break;
    }

    // Animation update
    bottleThrow.setFps(bottleThrowFps * perks.getReloadSpeedMultiplier());

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
        case State::Throwing:
            handsSprite.setTexture(bottleThrow.getCurrentFrame());
            break;
        
        case State::Hitting:
            handsSprite.setTexture(bottleHit.getCurrentFrame());
            break;

        case State::Aiming:
            handsSprite.setTexture(assets.playerHandsDefaultTexture);
            break;
        
        case State::Idle:
            handsSprite.setTexture(assets.playerHandsDefaultTexture);
            break;
        
        case State::Dying:
            handsSprite.setTexture(assets.playerHandsDefaultTexture);
    }

    // Draw if player is not dead
    if (currentState != State::Destroy)
    {
        window.draw(legsSprite);
        window.draw(handsSprite);
        window.draw(corpseSprite);
    }

    // If death animation has ended, change State
    if (currentState == State::Dying && !corpseSprite.getGlobalBounds().findIntersection({
    window.getView().getCenter() - window.getView().getSize() / 2.f,
    window.getView().getSize()})) currentState = State::Destroy;

    // If alive, render health bar
    if (!std::set{State::Dying, State::Destroy}.contains(currentState))
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
    if (currentState == State::Aiming)
    {
        // Stop the charging sound if it was playing
        audio.stopSound(chargingSoundId);

        // If was aiming, shoot a bottle
        sf::Vector2f bottlePosition = corpseSprite.getGlobalBounds().position + throwHandAbsPosition * corpseSprite.getScale().x;
        projectiles.addBottle(bottlePosition, sf::Vector2f(mouseWorldPos), bottleTime);

        // Change State and reset animation
        currentState = State::Throwing;
        bottleThrow.restart();
        return true;
    }
    return false;
}

void Player::cancelShooting() { if (currentState == State::Aiming) currentState = State::Idle; }

bool Player::startAiming()
{
    if (currentState == State::Idle || currentState == State::Hitting)
    {
        currentState = State::Aiming;

        // If this shot is charged, play the sound
        if (perks.isNextBottleBoosted()) chargingSoundId = audio.addSound(assets.chargingHitSound);
        return true;
    }
    return false;
}

void Player::startBlocking()
{
    if (currentState == State::Idle)
    {
        currentState = State::Hitting;
        bottleHit.restart();

        blockSoundId = audio.addSound(assets.playerHitSound);
    }
}

void Player::inflictDamage(int damage)
{
    health.changeHealth(-damage);

    // If health below 0, change State
    if (health.getHealth() <= 0 && !std::set{State::Dying, State::Destroy}.contains(currentState))
    {
        currentState = State::Dying;
        
        // Stop all sounds and play dying sound
        audio.stopSound(chargingSoundId);
        audio.stopSound(blockSoundId);
        audio.addSound(assets.playerDieSound);
    }
}

int Player::getMeleeDamage() { return 40; }

bool Player::isHitting() { return currentState == State::Hitting; }

int Player::succesfullParry()
{
    int healing = static_cast<int>(baseHealing * perks.getHealingMultiplier());
    health.changeHealth(healing);
    return healing;
}

bool Player::isAlive() { return currentState != State::Destroy; }

bool Player::isDying() { return currentState == State::Dying; }

void Player::reset()
{
    health.resetHealth();
    velocity = {0.f, 0.f};
    isRunning = false;
    currentState = State::Idle;
    corpseSprite.setPosition({0.f, 0.f});
    corpseSprite.setRotation(sf::Angle::Zero);
}
