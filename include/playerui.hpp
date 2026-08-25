#pragma once

#include "drawable.hpp"
#include "assetmanager.hpp"
#include "gamestate.hpp"
#include "perkmanager.hpp"
#include "animation.hpp"
#include "audiomanager.hpp"

class PlayerUI : public Drawable
{
    private:
        Animation bottleChargingAnimation;
        sf::Vector2f bottleBarSize;
        sf::Vector2f windowMargin;
        sf::Color bottleBarPrimaryColor;
        sf::Color bottleBarSecondaryColor;
        sf::Sprite bottleChargedBarSprite;
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

        PerkManager& perks;

        AudioManager& audio;

    public:
        PlayerUI(AssetManager& assets, PerkManager& perks, AudioManager& audio);
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow& window) override;
        sf::Time getBottleTime();
        void resetBottleTime();
        void activateBottleBar();
        void updateKillCount(int kills);
        void setGameState(GameState state);
};