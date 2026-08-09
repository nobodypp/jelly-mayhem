#pragma once

#include "projectile.hpp"
#include "animation.hpp"
#include "texturemanager.hpp"
#include "chromosome.hpp"


class Star : public Projectile
{
    private:
        static constexpr float linearVelocity = 200.f;
        Animation fly;
        Animation explode;
        sf::Sprite sprite;
        sf::Vector2f velocity;
        bool shotTarget;
        bool inView;
        Chromosome* ownerChromosome;

    public:
        Star(sf::Vector2f position, sf::Vector2f targetPos, sf::Color color, Chromosome& ownerChromosome, TextureManager& textures);
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow& window) override;
        sf::FloatRect getBounds() override;
        int getDamage() override;
        bool isColliding() override;
        bool isAlive() override;
        void registerHit() override;

};