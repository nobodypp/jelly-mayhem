#pragma once

#include "enemymanager.hpp"
#include "projectilemanager.hpp"
#include "player.hpp"
#include "damagetextmanager.hpp"


class CollisionManager
{
    private:
        static constexpr float minEnemiesDistance = 30.f;
        EnemyManager& enemies;
        ProjectileManager& projectiles;
        DamageTextManager& texts;

        void BottleCollisions();
        void StarCollisions(Player& player);
        void meleeCollisions(Player& player);
        void enemiesAntiCrowd();

    public:
        CollisionManager(EnemyManager& enemies, ProjectileManager& projectiles, DamageTextManager& texts);
        void handleCollisions(Player& player);
};