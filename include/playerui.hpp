#pragma once

#include "drawable.hpp"

class PlayerUI : public Drawable
{
    private:
        sf::Vector2f bottleBarSize;
        sf::Vector2f bottleBarMargin;
        sf::Color bottleBarPrimaryColor;
        sf::Color bottleBarSecondaryColor;
        sf::Time maxBottleTime;
        sf::Time bottleTime;
        sf::RectangleShape bottlePrimaryBar;
        sf::RectangleShape bottleSecondaryBar;
        bool bottleBarActive;

    public:
        PlayerUI();
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow& window) override;
        sf::Time getBottleTime();
        void resetBottleTime();
        void activateBottleBar();
};