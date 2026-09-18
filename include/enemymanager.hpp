#pragma once

#include <vector>
#include <cmath>
#include "drawable.hpp"
#include "jelly.hpp"
#include "player.hpp"
#include "assetmanager.hpp"
#include "projectilemanager.hpp"
#include "manager.hpp"
#include "chromosome.hpp"
#include "perkmanager.hpp"
#include "audiomanager.hpp"
#include "scoremanager.hpp"

class EnemyManager : Manager
{
    private:
        float spawnDistance = 910.f;
        static constexpr int jelliesPopulationSize = 20;
        static constexpr float difficulty = 0.002;
        static constexpr float baseMutationRate = 0.05f;
        static constexpr float maxMutationRate = 0.4f;
        static constexpr int stagnationThreshold = 5;
        static constexpr sf::Time spawningCooldown = sf::seconds(3.f);

        float mutationRate = baseMutationRate;
        int bestFitness = 0;

        sf::Time timeToNextSpawn = spawningCooldown;
        std::vector<std::unique_ptr<Jelly>> jellies;

        ProjectileManager& projectiles;
        AssetManager& assets;
        Player& player;
        RandomGenerator& randomizer;
        ScoreManager& scores;
        PerkManager& perks;
        AudioManager& audio;

        float currentLevel = 1.f;

        void addDefaultJelly();
        void addChildJelly();
        void addJelly(Chromosome chromosome);
        int getBestFitness();
        int getTotalFitness();
        Chromosome rouletteWheelParent();

    public:
        EnemyManager(AssetManager& assets, Player& player, ProjectileManager& projectiles, RandomGenerator& randomizer, PerkManager& perks, AudioManager& audio, ScoreManager& scores);
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow& window) override;
        std::size_t jelliesCount();
        Jelly& jellyAt(std::size_t i);
        void reset();
};