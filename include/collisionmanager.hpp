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
        static constexpr float dodgeDistance = 30.f;
        EnemyManager& enemies;
        ProjectileManager& projectiles;
        DamageTextManager& texts;
        PerkManager& perks;

        void BottleCollisions();
        void StarCollisions(Player& player);
        void meleeCollisions(Player& player);
        void enemiesAntiCrowd();
        float distanceBetweenTwoRects(const sf::FloatRect a, const sf::FloatRect b);

        int killCount;

    public:
        CollisionManager(EnemyManager& enemies, ProjectileManager& projectiles, DamageTextManager& texts, PerkManager& perks);
        void handleCollisions(Player& player);
        int getKillCount();
};