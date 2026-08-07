#pragma once

#include <SFML/Graphics.hpp>
#include "drawable.hpp"
#include "texturemanager.hpp"


class HealthBar : public Drawable
{
    private: 
        int maxHealth;
        int health;
        sf::Color primaryColor;
        sf::Color secondaryColor;
        float width;
        float height;
        float gap;
        sf::RectangleShape primaryBar;
        sf::RectangleShape secodnaryBar;
        sf::Font* font;
        sf::Text text;

    public:
        HealthBar(int maxHealth, TextureManager& textures);
        int GetHealth();
        void ResetHealth();
        void render(sf::RenderWindow& window) override;
        void changeHealth(int change);
        void attachToPosistion(sf::FloatRect spriteBounds);
};