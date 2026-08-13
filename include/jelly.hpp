#pragma once


#include "drawable.hpp"
#include "healthbar.hpp"
#include "animation.hpp"
#include "texturemanager.hpp"
#include "projectilemanager.hpp"
#include "entity.hpp"
#include "chromosome.hpp"

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
        static constexpr float autoRemoveDistance = 2000.f;
        Animation walking;
        Animation death;
        Animation biting;
        Animation shooting;
        Animation knockback;
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
        
    public:
        Jelly(sf::Vector2f position, TextureManager& textures, ProjectileManager& projectiles, Chromosome chromosome, float level);
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