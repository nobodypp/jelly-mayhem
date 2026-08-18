#pragma once

#include "drawable.hpp"
#include "texturemanager.hpp"
#include "gamestate.hpp"

class PlayerUI : public Drawable
{
    private:
        sf::Vector2f bottleBarSize;
        sf::Vector2f windowMargin;
        sf::Color bottleBarPrimaryColor;
        sf::Color bottleBarSecondaryColor;
        sf::Time maxBottleTime;
        sf::Time bottleTime;
        sf::RectangleShape bottlePrimaryBar;
        sf::RectangleShape bottleSecondaryBar;
        bool bottleBarActive;

        int killCount;
        sf::Font& font;
        sf::Text killText;

        GameState currentState;

        sf::VertexArray deathScreenBackground;
        sf::Text deathScreenText;

    public:
        PlayerUI(TextureManager& textures);
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow& window) override;
        sf::Time getBottleTime();
        void resetBottleTime();
        void activateBottleBar();
        void updateKillCount(int kills);
        void setGameState(GameState state);
};