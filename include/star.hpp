#pragma once

#include "projectile.hpp"
#include "animation.hpp"
#include "assetmanager.hpp"
#include "chromosome.hpp"


class Star : public Projectile
{
    private:
        float linearVelocity;
        Animation fly;
        Animation explode;
        sf::Sprite sprite;
        sf::Vector2f velocity;
        bool shotTarget;
        bool inView;
        Chromosome* ownerChromosome;
        float level;

    public:
        Star(sf::Vector2f position, sf::Vector2f targetPos, sf::Color color, Chromosome& ownerChromosome, float level, AssetManager& assets);
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow& window) override;
        sf::FloatRect getBounds() override;
        int getDamage() override;
        bool isColliding() override;
        bool isAlive() override;
        void registerHit() override;

};