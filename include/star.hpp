#pragma once

#include "projectile.hpp"
#include "animation.hpp"
#include "assetmanager.hpp"
#include "chromosome.hpp"
#include "perkmanager.hpp"
#include "audiomanager.hpp"


class Star : public Projectile
{
    private:
        static constexpr sf::Vector2f spriteOrigin = {27.f, 17.f};

        float linearVelocity;
        Animation flyAnimation;
        Animation explodeAnimation;
        sf::Sprite sprite;
        sf::Vector2f velocity;
        Chromosome* ownerChromosome;
        PerkManager* perks;
        float level;

        enum class state
        {
            FLYING,
            EXPLODING, 
            DESTROY
        };
        state currentState;

        bool wasCloseToPlayer;

        AudioManager* audio;
        AssetManager* assets;

    public:
        Star(sf::Vector2f position, sf::Vector2f targetPos, sf::Color color, Chromosome& ownerChromosome, float level, AssetManager& assets, PerkManager& perks, AudioManager& audio);
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow& window) override;
        sf::FloatRect getBounds() override;
        int getDamage() override;
        bool isColliding() override;
        bool isAlive() override;
        void registerHit() override;
        void registerProximityToPlayer();

};