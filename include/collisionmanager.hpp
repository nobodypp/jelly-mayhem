#pragma once

#include "enemymanager.hpp"
#include "projectilemanager.hpp"
#include "player.hpp"
#include "damagetextmanager.hpp"
#include "perkmanager.hpp"


class CollisionManager
{
    private:
        static constexpr float minEnemiesDistance = 30.f;
        static constexpr float dodgeDistance = 60.f;
        EnemyManager& enemies;
        ProjectileManager& projectiles;
        DamageTextManager& texts;
        PerkManager& perks;

        void BottleCollisions(Player& player);
        void StarCollisions(Player& player);
        void meleeCollisions(Player& player);
        void enemiesAntiCrowd();
        float distanceBetweenTwoRects(const sf::FloatRect a, const sf::FloatRect b);

    public:
        CollisionManager(EnemyManager& enemies, ProjectileManager& projectiles, DamageTextManager& texts, PerkManager& perks);
        void handleCollisions(Player& player);
};