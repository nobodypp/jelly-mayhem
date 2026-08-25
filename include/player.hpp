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
#include "perkmanager.hpp"
#include "audiomanager.hpp"
#include <set>


class Player : public Entity
{
    private:
        static constexpr int baseHealing = 20;
        static constexpr float acceleration = 400.f;
        static constexpr float speedFriction = 800.f;
        static constexpr float dyingSpeed = 500.f;
        
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
        bool isRunning;

        HealthBar health;

        sf::Vector2f throwHandAbsPosition;

        sf::Angle dyingRotation;

        void movement(sf::Time deltaTime);

        enum class state
        {
            IDLE,
            AIMING, 
            THROWING, 
            HITTING, 
            DYING, 
            DEAD
        };
        state currentState;

        ProjectileManager& projectiles;
        PerkManager& perks;
        AudioManager& audio;

        std::size_t chargingSoundId;
        std::size_t blockSoundId;


    public:
        Player(sf::Vector2u windowSize, AssetManager& assets, ProjectileManager& projectiles, PerkManager& perks, AudioManager& audio);
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow& window) override;
        sf::FloatRect getBounds() override;
        bool shootBottle(sf::Vector2f mouseWorldPos, sf::Time bottleTime);
        bool startAiming();
        void startBlocking();
        void inflictDamage(int damage);
        int getMeleeDamage();
        bool isHitting();
        int succesfullParry();
        bool isAlive();
        bool isDying();
};