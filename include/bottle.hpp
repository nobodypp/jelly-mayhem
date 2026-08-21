#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "animation.hpp"
#include "assetmanager.hpp"
#include "projectile.hpp"
#include "perkmanager.hpp"


class Bottle : public Projectile
{
    private:
        static constexpr float linearVelocity = 400.f;
        static constexpr sf::Vector2f spriteOrigin = {4.f, 6.f};
        static constexpr int baseDamage = 10;
        sf::Sprite sprite;
        sf::Vector2f velocity;
        sf::Angle rotationVelocity;
        Animation breakingAnimation;
        sf::Time timeLeft;
        sf::Clock clock;
        bool damageDealt;
        sf::Sound breakingSound;
        enum class state
        {
            FLYING, 
            BREAKING,
            DESTROY
        };
        state currentState;
        PerkManager* perks;

        float damageMultiplier;

    public:
        Bottle(sf::Vector2f position, sf::Vector2f mouseRelativePos, sf::Time flyingTime, AssetManager& assets, PerkManager& perks);
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow& window) override;
        sf::FloatRect getBounds() override;
        int getDamage() override;
        bool isColliding() override;
        bool isAlive() override;
        void registerHit() override;
};