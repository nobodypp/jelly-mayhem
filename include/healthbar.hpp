#pragma once

#include <SFML/Graphics.hpp>
#include "drawable.hpp"
#include "assetmanager.hpp"


class HealthBar : public Drawable
{
    private: 
        int maxHealth;
        int health;
        sf::Color primaryColor{230, 10, 10};
        sf::Color secondaryColor{128, 0, 0};
        float width = 100.f;
        float height = 14.f;
        sf::RectangleShape primaryBar{{width, height}};
        sf::RectangleShape secodnaryBar{{width, height}};
        sf::Text text;

    public:
        HealthBar(int maxHealth, AssetManager& assets);
        int getHealth();
        void resetHealth();
        void render(sf::RenderWindow& window) override;
        void changeHealth(int change);
        void attachToPosistion(sf::FloatRect spriteBounds);
};