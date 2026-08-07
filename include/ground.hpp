#pragma once

#include <SFML/Graphics.hpp>
#include "drawable.hpp"
#include "texturemanager.hpp"


class Ground : public Drawable
{
    private:
        sf::Sprite sprite;

    public:
        Ground(sf::Vector2u windowSize, TextureManager& textureManager);
        void render(sf::RenderWindow& window) override;
};