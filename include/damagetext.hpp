#pragma once

#include "drawable.hpp"
#include "texturemanager.hpp"
#include <cstdlib>


class DamageText: public Drawable
{
    private:
        static constexpr float spawnRadius = 40.f;
        sf::Time absoluteTime;
        sf::Time timeLeft;
        sf::Text text;

    public:
        DamageText(TextureManager& textures, int damage, bool positive, sf::FloatRect targetBounds, std::string description = "");
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow& window) override;
        bool isAlive();
};