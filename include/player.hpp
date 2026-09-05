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
        static constexpr sf::Vector2f maxVelocity = {200.f, 200.f};
        static constexpr sf::Vector2f throwHandAbsPosition = {46.f, 36.f};
        static constexpr sf::Angle dyingRotation = sf::degrees(400);
        static constexpr int maxHealth = 500;
        static constexpr float bottleThrowFps = 18;
        
        AssetManager& assets;
        ProjectileManager& projectiles;
        PerkManager& perks;
        AudioManager& audio;

        Animation legsRunning;
        Animation bottleThrow;
        Animation bottleHit;

        sf::Sprite corpseSprite;
        sf::Sprite handsSprite;
        sf::Sprite legsSprite;

        sf::Vector2f velocity{0.f, 0.f};
        bool isRunning = false;

        HealthBar health;

        void movement(sf::Time deltaTime);

        enum class State
        {
            Idle,
            Aiming, 
            Throwing, 
            Hitting, 
            Dying, 
            Destroy
        };
        State currentState = State::Idle;

        std::size_t chargingSoundId;
        std::size_t blockSoundId;

        sf::Vector2f mousePos;

    public:
        Player(AssetManager& assets, ProjectileManager& projectiles, PerkManager& perks, AudioManager& audio);
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow& window) override;
        sf::FloatRect getBounds() override;
        bool shootBottle(sf::Vector2f mouseWorldPos, sf::Time bottleTime);
        void cancelShooting();
        bool startAiming();
        void startBlocking();
        void inflictDamage(int damage);
        int getMeleeDamage();
        bool isHitting();
        int succesfullParry();
        bool isAlive();
        bool isDying();
        void reset();
};