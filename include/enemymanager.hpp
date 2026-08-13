#pragma once

#include <vector>
#include "drawable.hpp"
#include "jelly.hpp"
#include "player.hpp"
#include "texturemanager.hpp"
#include "projectilemanager.hpp"
#include "manager.hpp"
#include "chromosome.hpp"
#include <cmath>

class EnemyManager : Manager
{
    private:
        float spawnDistance = 850.f;
        static constexpr int jelliesPopulationSize = 20;
        static constexpr float difficulty = 0.002;
        static constexpr float baseMutationRate = 0.05f;
        static constexpr float maxMutationRate = 0.3f;
        static constexpr int stagnationThreshold = 5;

        // std::size_t stagnationRespawns;
        float mutationRate;
        int bestFitness;

        sf::Time spawningCooldown;
        sf::Time timeToNextSpawn;
        std::vector<std::unique_ptr<Jelly>> jellies;

        ProjectileManager& projectiles;
        TextureManager& textures;
        Player& player;
        RandomGenerator& randomizer;

        float currentLevel;

        void addDefaultJelly();
        void addChildJelly();
        void addJelly(Chromosome chromosome);
        int getBestFitness();
        int getTotalFitness();
        Chromosome rouletteWheelParent();
    
    public:
        EnemyManager(TextureManager& textures, Player& player, ProjectileManager& projectiles, RandomGenerator& randomizer);
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow& window) override;
        std::size_t jelliesCount();
        Jelly& jellyAt(std::size_t i);
        int killCount;
};