#pragma once

#include "assetmanager.hpp"
#include "bottle.hpp"
#include "star.hpp"
#include "manager.hpp"


class ProjectileManager : public Manager
{
    private:
        AssetManager& textureManager;
        std::vector<std::unique_ptr<Bottle>> bottles;
        std::vector< std::unique_ptr<Star>> stars;

    public:
        ProjectileManager(AssetManager& textureManager);
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow& window) override;

        template<typename... Args>
        void addBottle(Args&&... args) { bottles.push_back(std::make_unique<Bottle>(std::forward<Args>(args)..., textureManager)); }
        std::size_t bottleCount();
        Bottle& bottleAt(std::size_t i);

        template<typename... Args>
        void addStar(Args&&... args) { stars.push_back(std::make_unique<Star>(std::forward<Args>(args)..., textureManager)); }
        std::size_t starsCount();
        Star& starAt(std::size_t i);
};