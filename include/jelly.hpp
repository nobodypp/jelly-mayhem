#pragma once


#include "drawable.hpp"
#include "healthbar.hpp"
#include "animation.hpp"
#include "texturemanager.hpp"
#include "projectilemanager.hpp"
#include "entity.hpp"
#include <cstdlib>

class Jelly : public Entity
{
    private:
        static constexpr float walkingSpeed = 100.f;
        static constexpr float bitingSpeed = 350.f;
        static constexpr float shootingDistance = 370.f;
        static constexpr float bitingDistance = 320.f;
        static constexpr sf::Vector2f shootingTexturePosition = {20.f, 7.f};
        static constexpr int shootFrame = 5;
        static constexpr float knockbackSpeed = 300.f;
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
        
    public:
        Jelly(sf::Vector2f position, TextureManager& textures, ProjectileManager& projectiles);
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
};