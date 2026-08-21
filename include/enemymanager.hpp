#pragma once

#include <vector>
#include "drawable.hpp"
#include "jelly.hpp"
#include "player.hpp"
#include "assetmanager.hpp"
#include "projectilemanager.hpp"
#include "manager.hpp"
#include "chromosome.hpp"
#include "perkmanager.hpp"
#include <cmath>

class EnemyManager : Manager
{
    private:
        float spawnDistance = 850.f;
        static constexpr int jelliesPopulationSize = 20;
        static constexpr float difficulty = 0.002;
        static constexpr float baseMutationRate = 0.05f;
        static constexpr float maxMutationRate = 0.4f;
        static constexpr int stagnationThreshold = 5;

        float mutationRate;
        int bestFitness;

        sf::Time spawningCooldown;
        sf::Time timeToNextSpawn;
        std::vector<std::unique_ptr<Jelly>> jellies;

        ProjectileManager& projectiles;
        AssetManager& assets;
        Player& player;
        RandomGenerator& randomizer;

        float currentLevel;

        void addDefaultJelly();
        void addChildJelly();
        void addJelly(Chromosome chromosome);
        int getBestFitness();
        int getTotalFitness();
        Chromosome rouletteWheelParent();

        int killCount;

        PerkManager& perks;
    
    public:
        EnemyManager(AssetManager& assets, Player& player, ProjectileManager& projectiles, RandomGenerator& randomizer, PerkManager& perks);
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow& window) override;
        std::size_t jelliesCount();
        Jelly& jellyAt(std::size_t i);
        int getKillCount();
};