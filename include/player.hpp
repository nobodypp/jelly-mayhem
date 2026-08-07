#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <vector>
#include "animation.hpp"
#include "healthbar.hpp"
#include "bottle.hpp"
#include "texturemanager.hpp"
#include "projectilemanager.hpp"
#include "entity.hpp"


class Player : public Entity
{
    private:
        TextureManager textures;

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

        void handleInput(sf::Time deltaTime);

        enum state
        {
            IDLE,
            AIMING, 
            THROWING, 
            HITTING
        };
        enum state currentState;

    public:
        Player(sf::Vector2u windowSize, TextureManager& textures, ProjectileManager& projectiles);
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow& window) override;
        sf::FloatRect getBounds() override;
        bool mouseReleased(const sf::Event::MouseButtonReleased* event, sf::Vector2f mouseWorldPos, sf::Time bottleTime);
        bool mousePressed(const sf::Event::MouseButtonPressed* event);
        void inflictDamage(int damage);
        int getMeleeDamage();
        bool isHitting();
        void succesfullParry();
};