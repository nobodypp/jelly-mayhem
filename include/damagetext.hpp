#pragma once

#include "drawable.hpp"
#include "assetmanager.hpp"
#include <cstdlib>


class DamageText: public Drawable
{
    private:
        static constexpr float spawnRadius = 40.f;
        static constexpr sf::Time absoluteTime = sf::seconds(1.5f);
        static constexpr unsigned int characterSize = 25;
        sf::Time timeLeft = absoluteTime;
        sf::Text text;

    public:
        DamageText(AssetManager& assets, int damage, bool positive, sf::FloatRect targetBounds, std::string description = "");
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow& window) override;
        bool isAlive();
};