#include "playerui.hpp"


PlayerUI::PlayerUI(AssetManager& assets)
    : bottleBarSize({100.f, 20.f}), 
      windowMargin({20.f, 20.f}), 
      bottleBarPrimaryColor(100, 100, 100), 
      bottleBarSecondaryColor(200, 200, 200), 
      maxBottleTime(sf::seconds(1.5)), 
      bottleTime(maxBottleTime),
      bottlePrimaryBar(bottleBarSize), 
      bottleSecondaryBar(bottleBarSize), 
      bottleBarActive(false), 
      font(assets.font), 
      killText(font), 
      deathScreenBackground(sf::PrimitiveType::TriangleStrip, 4), 
      deathScreenText(font)      
{
    // Bottle charge bar init
    bottlePrimaryBar.setFillColor(bottleBarPrimaryColor);
    bottleSecondaryBar.setFillColor(bottleBarSecondaryColor);

    // Kill count text
    killText.setCharacterSize(25);

    // Game over text
    deathScreenText.setCharacterSize(50);
    deathScreenText.setString("Geym O'vah");
    deathScreenText.setOrigin(deathScreenText.getLocalBounds().getCenter());
    deathScreenText.setFillColor(sf::Color::White);

    // Death screen background
    deathScreenBackground[0].color = sf::Color::Black;
    deathScreenBackground[1].color = sf::Color::Red;
    deathScreenBackground[2].color = sf::Color::Black;
    deathScreenBackground[3].color = sf::Color::Red;
    
}

void PlayerUI::update(sf::Time deltaTime)
{
    switch(currentState)
    {
        case GameState::PLAY:
            // If player is aiming, update bottle charge bar
            if (bottleBarActive) bottleTime += deltaTime;
            else bottleTime = sf::Time::Zero;
            bottleTime = std::clamp(bottleTime, sf::Time::Zero, maxBottleTime);
            break;
    }
}

void PlayerUI::render(sf::RenderWindow& window)
{
    switch(currentState)
    {
        case GameState::PLAY:
        {
            // Bottle charge bar position and size
            sf::Vector2f leftBottomCorner = {0.f, static_cast<float> (window.getSize().y)};
            bottlePrimaryBar.setPosition({leftBottomCorner.x + windowMargin.x, leftBottomCorner.y - windowMargin.y - bottleBarSize.y});
            bottleSecondaryBar.setPosition(bottlePrimaryBar.getPosition());
            bottlePrimaryBar.setSize({bottleTime.asSeconds() / maxBottleTime.asSeconds() * bottleBarSize.x, bottleBarSize.y});
            
            // Kill count position and text
            killText.setPosition({windowMargin.x, windowMargin.y});
            killText.setString("Kills: " + std::to_string(killCount));

            window.draw(bottleSecondaryBar);
            window.draw(bottlePrimaryBar);
            window.draw(killText);
            break;
        }
        
        case GameState::LOSE_SCREEN:
        {
            // Update screen size
            deathScreenBackground[0].position = sf::Vector2f{0.f, 0.f};
            deathScreenBackground[1].position = sf::Vector2f{0.f, static_cast<float> (window.getSize().y)};
            deathScreenBackground[2].position = sf::Vector2f{static_cast<float> (window.getSize().x), 0.f};
            deathScreenBackground[3].position = sf::Vector2f{static_cast<float> (window.getSize().x), static_cast<float> (window.getSize().y)};

            // Game over text
            deathScreenText.setPosition(sf::Vector2f{window.getView().getSize().x / 2.f, window.getView().getSize().y * 0.3f});

            // Kill count
            killText.setOrigin(killText.getLocalBounds().getCenter());
            killText.setPosition(deathScreenText.getPosition() + sf::Vector2f{0.f, 100.f});

            window.draw(deathScreenBackground);
            window.draw(deathScreenText);
            window.draw(killText);
            break;
        }
    }
    
}

sf::Time PlayerUI::getBottleTime() { return bottleTime; }

void PlayerUI::activateBottleBar() { bottleBarActive = true; }

void PlayerUI::resetBottleTime() { bottleBarActive = false; }

void PlayerUI::updateKillCount(int kills) { killCount = kills; }

void PlayerUI::setGameState(GameState state) { currentState = state; }