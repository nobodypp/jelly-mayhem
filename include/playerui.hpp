#pragma once

#include "drawable.hpp"
#include "assetmanager.hpp"
#include "gamestate.hpp"
#include "perkmanager.hpp"
#include "animation.hpp"
#include "audiomanager.hpp"
#include <array>


class PlayerUI : public Drawable
{
    private:
        class Button : public Drawable
        {
            private:
                static constexpr sf::Color primaryFillColor{200, 200, 200};
                static constexpr sf::Color secondaryFillcolor{150, 150, 150};
                static constexpr sf::Color textColor{30, 30, 30};

                sf::Vector2f size;
                sf::Vector2f position;

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
                      horizontalFill({size.x, size.y - 2 * circleRadius}), 
                      verticalFill({size.x - 2 * circleRadius, size.y}), 
                      text(assets.font), 
                      audio(&audio), 
                      assets(&assets)
                {
                    this->text.setString(text);
                    this->text.setOrigin(this->text.getLocalBounds().getCenter());
                    this->text.setFillColor(textColor);

                    if (circleRadius > std::min(size.x, size.y) / 2) std::cout << "Incorrect button circle radius!\n";
                }

                void render(sf::RenderWindow& window) override
                {
                    // Dependig on the isClicked state, change color
                    if (isClicked) leftTopCorner.setFillColor(secondaryFillcolor);
                    else leftTopCorner.setFillColor(primaryFillColor);
                    rightTopCorner.setFillColor(leftTopCorner.getFillColor());
                    leftBottomCorner.setFillColor(leftTopCorner.getFillColor());
                    rightBottomCorner.setFillColor(leftTopCorner.getFillColor());
                    horizontalFill.setFillColor(leftTopCorner.getFillColor());
                    verticalFill.setFillColor(leftTopCorner.getFillColor());

                    // Set size, origin and position
                    horizontalFill.setSize({size.x, size.y - 2 * leftBottomCorner.getRadius()});
                    verticalFill.setSize({size.x - 2 * leftBottomCorner.getRadius(), size.y});

                    horizontalFill.setOrigin(horizontalFill.getLocalBounds().getCenter());
                    verticalFill.setOrigin(verticalFill.getLocalBounds().getCenter());

                    horizontalFill.setPosition(position);
                    verticalFill.setPosition(position);
                    leftTopCorner.setPosition({position.x - size.x / 2.f, position.y - size.y / 2.f});
                    rightTopCorner.setPosition({position.x + size.x / 2.f - rightTopCorner.getRadius() * 2, position.y - size.y / 2.f});
                    leftBottomCorner.setPosition({position.x - size.x / 2.f, position.y + size.y / 2.f - leftBottomCorner.getRadius() * 2});
                    rightBottomCorner.setPosition({position.x + size.x / 2.f - rightBottomCorner.getRadius() * 2, position.y + size.y / 2.f - rightBottomCorner.getRadius() * 2});
                    text.setPosition(position);
                    
                    window.draw(leftTopCorner);
                    window.draw(rightTopCorner);
                    window.draw(leftBottomCorner);
                    window.draw(rightBottomCorner);
                    window.draw(horizontalFill);
                    window.draw(verticalFill);
                    window.draw(text);
                }

                void setPosition(sf::Vector2f position) { this->position = position; }

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
                    else isClicked = false;
                    return false;
                }

                void resetWasClicked() { wasClicked = false; }

                bool getWasClicked() { return wasClicked; }

                sf::Vector2f getPosition() { return position; }

                sf::Vector2f getSize() { return size; }

                void setSize(sf::Vector2f size) { this->size = size; }

        };

        static constexpr sf::Time announcementDefaultTime = sf::seconds(5.f);
        static constexpr sf::Vector2f windowMargin{20.f, 20.f};
        static constexpr sf::Color bottleBarPrimaryColor{100, 100, 100};
        static constexpr sf::Color bottleBarSecondaryColor{200, 200, 200};
        static constexpr sf::Time maxBottleTime = sf::seconds(1.5f);
        static constexpr sf::Vector2f buttonSize = {280.f, 80.f};

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

        int killCount = 0;
        sf::Text killText;

        GameState currentState = GameState::Play;

        sf::VertexArray deathScreenBackground;
        sf::Text deathScreenText;

        sf::Time gameTime;
        sf::Text timeText;

        sf::Text announcementText;
        sf::Time announcementTimeLeft;

        enum class LoseButtonId
        {
            Retry, 
            Quit, 

            Count
        };

        enum class PauseButtonId
        {
            Resume, 
            Perks, 
            Options, 
            Quit, 

            Count
        };

        enum class PauseScreenState
        {
            Menu, 
            Perks,
            Options
        };

        PauseScreenState pauseState;

        template<typename T>
        static constexpr std::size_t index(T value)
        {
            return static_cast<std::size_t>(value);
        }

        std::array<Button, static_cast<std::size_t>(LoseButtonId::Count)> loseButtons;
        std::array<Button, static_cast<std::size_t>(PauseButtonId::Count)> pauseButtons;

        Button perkBackground;
        sf::Text perkName;
        sf::Text perkObjective;
        sf::Text perkReward;
        Button previousButton;
        Button nextButton;
        std::size_t perkId;
        
        Button returnButton;

        sf::RectangleShape volumePrimaryBar;
        sf::RectangleShape volumeSecondaryBar;
        sf::Text volumeText;
        sf::CircleShape volumeButton;
        bool isVolumeBarClicked;

    public:
        PlayerUI(AssetManager& assets, PerkManager& perks, AudioManager& audio, GameState state);
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow& window) override;
        sf::Time getBottleTime();
        void resetBottleTime();
        void activateBottleBar();
        void updateKillCount(int kills);
        void changeGameState(GameState state);
        void mouseClicked(sf::Vector2f mousePos);
        void mouseReleased(sf::Vector2f mousePos);
        void mouseMoved(sf::Vector2f mousePos);
        bool getRetry();
        bool getQuit();
        bool getResume();
};