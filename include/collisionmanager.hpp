#pragma once

#include "enemymanager.hpp"
#include "projectilemanager.hpp"
#include "player.hpp"
#include "damagetextmanager.hpp"


class CollisionManager
{
    private:
        EnemyManager& enemies;
        ProjectileManager& projectiles;
        DamageTextManager& texts;

    public:
        CollisionManager(EnemyManager& enemies, ProjectileManager& projectiles, DamageTextManager& texts);
        void handleCollisions(Player& player);
};