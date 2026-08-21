#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <vector>
#include "animation.hpp"
#include "healthbar.hpp"
#include "bottle.hpp"
#include "assetmanager.hpp"
#include "projectilemanager.hpp"
#include "entity.hpp"
#include <set>


class Player : public Entity
{
    private:
        AssetManager assets;

        Animation legsRunning;
        Animation bottleThrow;
        Animation bottleHit;

        sf::Sprite corpseSprite;
        sf::Sprite handsSprite;
        sf::Sprite legsSprite;

        sf::FloatRect croppedBounds;

        sf::Vector2f velocity;
        const sf::Vector2f maxVelocity;
        static constexpr float acceleration = 400.f;
        static constexpr float speedFriction = 800.f;
        bool isRunning;

        HealthBar health;

        ProjectileManager& projectiles;

        sf::Vector2f throwHandAbsPosition;

        static constexpr float dyingSpeed = 500.f;
        sf::Angle dyingRotation;

        void handleInput(sf::Time deltaTime);

        enum state
        {
            IDLE,
            AIMING, 
            THROWING, 
            HITTING, 
            DYING, 
            DEAD
        };
        enum state currentState;

        sf::Sound dieSound;
        sf::Sound blockSound;

    public:
        Player(sf::Vector2u windowSize, AssetManager& assets, ProjectileManager& projectiles);
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow& window) override;
        sf::FloatRect getBounds() override;
        bool shootBottle(sf::Vector2f mouseWorldPos, sf::Time bottleTime);
        bool startAiming();
        void startBlocking();
        void inflictDamage(int damage);
        int getMeleeDamage();
        bool isHitting();
        void succesfullParry();
        bool isAlive();
        bool isDying();
};