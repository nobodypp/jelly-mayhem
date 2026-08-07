#pragma once

#include "texturemanager.hpp"
#include "bottle.hpp"
#include "drawable.hpp"
#include "star.hpp"


class ProjectileManager : public Drawable
{
    private:
        TextureManager& textureManager;
        std::vector<Bottle> bottles;
        std::vector<Star> stars;

    public:
        ProjectileManager(TextureManager& textureManager);
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow& window) override;

        template<typename... Args>
        void addBottle(Args&&... args) { bottles.emplace_back(std::forward<Args>(args)..., textureManager); }
        std::size_t bottleCount();
        Bottle& bottleAt(std::size_t i);

        template<typename... Args>
        void addStar(Args&&... args) { stars.emplace_back(std::forward<Args>(args)..., textureManager); }
        std::size_t starsCount();
        Star& starAt(std::size_t i);
};