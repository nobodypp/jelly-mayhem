#pragma once

#include <SFML/Graphics.hpp>
#include "drawable.hpp"
#include "assetmanager.hpp"


class Ground : public Drawable
{
    private:
        sf::Sprite sprite;

    public:
        Ground(sf::Vector2u windowSize, AssetManager& textureManager);
        void render(sf::RenderWindow& window) override;
};