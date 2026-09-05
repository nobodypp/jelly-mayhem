#pragma once


#include "drawable.hpp"
#include "healthbar.hpp"
#include "animation.hpp"
#include "assetmanager.hpp"
#include "projectilemanager.hpp"
#include "entity.hpp"
#include "chromosome.hpp"
#include "perkmanager.hpp"
#include "audiomanager.hpp"
#include <set>

class Jelly : public Entity
{
    private:
        static constexpr sf::Vector2f shootingTexturePosition = {20.f, 7.f};
        static constexpr int shootFrame = 5;
        static constexpr float knockbackSpeed = 150.f;
        static constexpr sf::Time defaultCooldownTime = sf::seconds(1.f);
        Chromosome chromosome;
        float walkingSpeed;
        float bitingSpeed;
        float bitingDistance;
        float shootingDistance;
        float autoRemoveDistance = 2500.f;
        Animation walkingAnimation;
        Animation deathAnimation;
        Animation bitingAnimation;
        Animation shootingAnimatin;
        Animation knockbackAnimation;
        sf::Sprite sprite;
        HealthBar health;
        sf::Vector2f targetPosition;
        sf::Time currentCooldown;
        sf::Texture* defaultTexture;
        sf::Vector2f knockbackVelocity;
        enum class State
        {
            Walking, 
            Shooting,
            Biting,
            Cooldown,
            Knockback,
            Dying, 
            Destroy
        };
        State currentState = State::Walking;
        bool hasShot = false;
        bool hasBiten;
        float level;

        ProjectileManager* projectiles;
        PerkManager* perks;
        AudioManager* audio;
        AssetManager* assets;

        std::size_t shootSoundId;
        std::size_t biteSoundId;
        std::size_t knockbackSoundId;
        
    public:
        Jelly(sf::Vector2f position, AssetManager& assets, ProjectileManager& projectiles, Chromosome chromosome, float level, PerkManager& perks, AudioManager& audio);
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
        bool isDuringKnockback();
        bool isDying();
};