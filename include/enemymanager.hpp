#pragma once

#include <vector>
#include "drawable.hpp"
#include "jelly.hpp"
#include "player.hpp"
#include "texturemanager.hpp"
#include "projectilemanager.hpp"
#include "manager.hpp"
#include "random.hpp"
#include "chromosome.hpp"

class EnemyManager : Manager
{
    private:
        static constexpr float spawnDistance = 850.f;
        static constexpr int jelliesPopulationSize = 20;
        static constexpr float mutationRatio = 0.1f;
        sf::Time spawningCooldown;
        sf::Time timeToNextSpawn;
        std::vector<std::unique_ptr<Jelly>> jellies;
        ProjectileManager& projectiles;
        TextureManager& textures;
        Player& player;
        RandomGenerator& randomizer;

        void addDefaultJelly();
        void addChildJelly();
        void addJelly(Chromosome chromosome);
        Chromosome rouletteWheelParent();
    
    public:
        EnemyManager(TextureManager& textures, Player& player, ProjectileManager& projectiles, RandomGenerator& randomizer);
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow& window) override;
        std::size_t jelliesCount();
        Jelly& jellyAt(std::size_t i);
        int killCount;
};