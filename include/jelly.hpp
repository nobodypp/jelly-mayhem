#pragma once


#include "drawable.hpp"
#include "healthbar.hpp"
#include "animation.hpp"
#include "assetmanager.hpp"
#include "projectilemanager.hpp"
#include "entity.hpp"
#include "chromosome.hpp"
#include <set>

class Jelly : public Entity
{
    private:
        Chromosome chromosome;
        float walkingSpeed;
        float bitingSpeed;
        float bitingDistance;
        float shootingDistance;
        static constexpr sf::Vector2f shootingTexturePosition = {20.f, 7.f};
        static constexpr int shootFrame = 5;
        static constexpr float knockbackSpeed = 300.f;
        float autoRemoveDistance = 2500.f;
        Animation walkingAnimation;
        Animation deathAnimation;
        Animation bitingAnimation;
        Animation shootingAnimatin;
        Animation knockbackAnimation;
        sf::Sprite sprite;
        HealthBar health;
        ProjectileManager* projectiles;
        sf::Vector2f targetPosition;
        sf::Time currentCooldown;
        sf::Time defaultCooldownTime;
        sf::Texture* defaultTexture;
        sf::Vector2f knockbackVelocity;
        enum state
        {
            WALKING, 
            SHOOTING,
            BITING,
            COOLDOWN,
            KNOCKBACK,
            DYING, 
            DEAD
        };
        enum state currentState;
        bool hasShot;
        bool hasBiten;
        float level;

        sf::Sound biteSound;
        sf::Sound dieSound;
        sf::Sound knockbackSound;
        sf::Sound shootSound;
        
    public:
        Jelly(sf::Vector2f position, AssetManager& assets, ProjectileManager& projectiles, Chromosome chromosome, float level);
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow& window) override;
        sf::FloatRect getBounds() override;
        bool isAlive() override;
        void setTargetPosition(sf::Vector2f targetPos);
        bool inflictDamage(int damage);
        bool isColliding();
        void registerHit();
        void registerKnockback(sf::Vector2f playerPosition);
        int getDamage();
        void move(sf::Vector2f translation);
        Chromosome getChromosome();
};