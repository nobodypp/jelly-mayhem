#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "animation.hpp"
#include "texturemanager.hpp"
#include "projectile.hpp"


class Bottle : public Projectile
{
    private:
        static constexpr float linearVelocity = 400.f;
        sf::Sprite sprite;
        sf::Vector2f velocity;
        sf::Angle rotationVelocity;
        Animation breakingAnimation;
        sf::Time timeLeft;
        sf::Clock clock;
        bool damageDealt;

    public:
        Bottle(sf::Vector2f position, sf::Vector2f mouseRelativePos, sf::Time flyingTime, TextureManager& textureManager);
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow& window) override;
        sf::FloatRect getBounds() override;
        int getDamage() override;
        bool isColliding() override;
        bool isAlive() override;
        void registerHit() override;
        enum state
        {
            FLYING, 
            BREAKING,
            DESTROY
        };

    private:
        enum state currentState;
};