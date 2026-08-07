#pragma once

#include <vector>
#include "drawable.hpp"
#include "jelly.hpp"
#include "player.hpp"
#include "texturemanager.hpp"
#include "projectilemanager.hpp"
#include <cstdlib>

class EnemyManager : Drawable
{
    private:
        static constexpr float spawnDistance = 800.f;
        static constexpr int jelliesPopulationSize = 10;
        std::vector<Jelly> jellies;
        ProjectileManager& projectiles;
        TextureManager& textures;
        Player& player;

        void addJelly();
    
    public:
        EnemyManager(TextureManager& textures, Player& player, ProjectileManager& projectiles);
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow& window) override;
        std::size_t jelliesCount();
        Jelly& jellyAt(std::size_t i);
        int killCount;
};