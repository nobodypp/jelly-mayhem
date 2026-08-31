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
        class Button
        {
            private:
                static constexpr sf::Color primaryFillColor{200, 200, 200};
                static constexpr sf::Color secondaryFillcolor{150, 150, 150};
                static constexpr sf::Color textColor{30, 30, 30};

                sf::Vector2f size;

                sf::CircleShape leftTopCorner;
                sf::CircleShape rightTopCorner;
                sf::CircleShape leftBottomCorner;
                sf::CircleShape rightBottomCorner;
                sf::RectangleShape horizontalFill;
                sf::RectangleShape verticalFill;

                sf::Text text;

                bool isClicked = false;
                bool wasClicked = false;

                bool isInsideButton(sf::Vector2f pos)
                {
                    sf::Vector2f position = text.getPosition();
                    return std::abs(pos.x - position.x) < size.x / 2 &&
                        std::abs(pos.y - position.y) < size.y / 2;
                }

                AssetManager* assets;
                AudioManager* audio;
            
            public:
                Button(sf::Vector2f size, std::string text, float circleRadius, AssetManager& assets, AudioManager& audio)
                    : size(size), 
                      leftTopCorner(circleRadius), 
                      rightTopCorner(circleRadius), 
                      leftBottomCorner(circleRadius), 
                      rightBottomCorner(circleRadius), 
                      horizontalFill(sf::Vector2f{size.x, size.y - 2 * circleRadius}), 
                      verticalFill(sf::Vector2f{size.x - 2 * circleRadius, size.y}), 
                      text(assets.font), 
                      audio(&audio), 
                      assets(&assets)
                {
                    horizontalFill.setOrigin(horizontalFill.getLocalBounds().getCenter());
                    verticalFill.setOrigin(verticalFill.getLocalBounds().getCenter());
                    this->text.setString(text);
                    this->text.setOrigin(this->text.getLocalBounds().getCenter());
                    this->text.setFillColor(textColor);

                    if (circleRadius > std::min(size.x, size.y) / 2) std::cout << "Correct button circle radius!\n";
                }

                void render(sf::RenderWindow& window)
                {
                    // If button lost focus, it is no longer being clicked
                    if (isClicked && (!isInsideButton(sf::Vector2f(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) || !sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))) isClicked = false;

                    // Dependig on the isClicked state, change color
                    if (isClicked) leftTopCorner.setFillColor(secondaryFillcolor);
                    else leftTopCorner.setFillColor(primaryFillColor);
                    rightTopCorner.setFillColor(leftTopCorner.getFillColor());
                    leftBottomCorner.setFillColor(leftTopCorner.getFillColor());
                    rightBottomCorner.setFillColor(leftTopCorner.getFillColor());
                    horizontalFill.setFillColor(leftTopCorner.getFillColor());
                    verticalFill.setFillColor(leftTopCorner.getFillColor());
                    
                    window.draw(leftTopCorner);
                    window.draw(rightTopCorner);
                    window.draw(leftBottomCorner);
                    window.draw(rightBottomCorner);
                    window.draw(horizontalFill);
                    window.draw(verticalFill);
                    window.draw(text);
                }

                void setPosition(sf::Vector2f position)
                {
                    horizontalFill.setPosition(position);
                    verticalFill.setPosition(position);
                    leftTopCorner.setPosition(sf::Vector2f{position.x - size.x / 2.f, position.y - size.y / 2.f});
                    rightTopCorner.setPosition(sf::Vector2f{position.x + size.x / 2.f - rightTopCorner.getRadius() * 2, position.y - size.y / 2.f});
                    leftBottomCorner.setPosition(sf::Vector2f{position.x - size.x / 2.f, position.y + size.y / 2.f - leftBottomCorner.getRadius() * 2});
                    rightBottomCorner.setPosition(sf::Vector2f{position.x + size.x / 2.f - rightBottomCorner.getRadius() * 2, position.y + size.y / 2.f - rightBottomCorner.getRadius() * 2});
                    text.setPosition(position);
                }

                void mouseClicked(sf::Vector2f mousePos)
                {
                    if (isInsideButton(mousePos))
                    {
                        isClicked = true;
                    }
                }

                bool mouseReleased(sf::Vector2f mousePos)
                {
                    if (isClicked && isInsideButton(mousePos))
                    {
                        wasClicked = true;
                        isClicked = false;
                        audio->addSound(assets->buttonSound);
                        return true;
                    }
                    return false;
                }

                void resetWasClicked() { wasClicked = false; }

                bool getWasClicked() { return wasClicked; }
        };

        static constexpr sf::Time announcementDefaultTime = sf::seconds(5.f);
        static constexpr sf::Vector2f windowMargin{20.f, 20.f};
        static constexpr sf::Color bottleBarPrimaryColor{100, 100, 100};
        static constexpr sf::Color bottleBarSecondaryColor{200, 200, 200};
        static constexpr sf::Time maxBottleTime = sf::seconds(1.5f);

        PerkManager& perks;
        AudioManager& audio;

        Animation bottleChargingAnimation;
        sf::Vector2f bottleBarSize;
        sf::Sprite bottleChargedBarSprite;
        sf::Time bottleTime;
        sf::RectangleShape bottlePrimaryBar;
        sf::RectangleShape bottleSecondaryBar;
        bool bottleBarActive;

        sf::RectangleShape pauseBackground;

        int killCount;
        sf::Text killText;

        GameState currentState;

        sf::VertexArray deathScreenBackground;
        sf::Text deathScreenText;

        sf::Time gameTime;
        sf::Text timeText;

        sf::Text announcementText;
        sf::Time announcementTimeLeft;

        Button retryButton;

        

    public:
        PlayerUI(AssetManager& assets, PerkManager& perks, AudioManager& audio);
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow& window) override;
        sf::Time getBottleTime();
        void resetBottleTime();
        void activateBottleBar();
        void updateKillCount(int kills);
        void setGameState(GameState state);
        void mouseClicked(sf::Vector2f mousePos);
        void mouseReleased(sf::Vector2f mousePos);
        bool getRetry();
};