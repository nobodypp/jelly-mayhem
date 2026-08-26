#include "playerui.hpp"


PlayerUI::PlayerUI(AssetManager& assets, PerkManager& perks, AudioManager& audio)
    : bottleChargingAnimation(&assets.bottleBarChargedFrames, 10.f), 
      bottleBarSize(bottleChargingAnimation.getCurrentFrame().getSize()), 
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
      deathScreenText(font), 
      perks(perks), 
      bottleChargedBarSprite(bottleChargingAnimation.getCurrentFrame()), 
      audio(audio), 
      gameTime(sf::Time::Zero), 
      timeText(assets.font)
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

    // Pause screen background
    pauseBackground.setFillColor(sf::Color{0, 0, 0, 80});

    // Time text
    timeText.setFillColor(sf::Color::White);
    timeText.setCharacterSize(25);
    
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

            bottleChargingAnimation.update(deltaTime);

            gameTime += deltaTime;
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
            sf::Vector2f leftBottomCorner = {0.f, static_cast<float> (window.getView().getSize().y)};
            bottleSecondaryBar.setPosition({leftBottomCorner.x + windowMargin.x, leftBottomCorner.y - windowMargin.y - bottleBarSize.y});
            window.draw(bottleSecondaryBar);

            if (perks.isNextBottleBoosted())
            {
                bottleChargedBarSprite.setPosition(bottleSecondaryBar.getPosition());
                bottleChargedBarSprite.setTexture(bottleChargingAnimation.getCurrentFrame());
                bottleChargedBarSprite.setTextureRect({{0, 0}, 
                    {static_cast<int>(bottleTime.asSeconds() / maxBottleTime.asSeconds() * bottleBarSize.x), static_cast<int>(bottleBarSize.y)}});
                window.draw(bottleChargedBarSprite);
            }
            else
            {
                bottlePrimaryBar.setPosition(bottleSecondaryBar.getPosition());
                bottlePrimaryBar.setSize({bottleTime.asSeconds() / maxBottleTime.asSeconds() * bottleBarSize.x, bottleBarSize.y});
                window.draw(bottlePrimaryBar);
            }
            
            // Kill count position and text
            killText.setPosition({windowMargin.x, windowMargin.y});
            killText.setString("Kills: " + std::to_string(killCount));
            window.draw(killText);

            // Time text
            sf::Vector2f rightTopCorner = {static_cast<float> (window.getView().getSize().x), 0.f};
            std::string seconds = std::to_string(static_cast<int>(gameTime.asSeconds()) % 60);
            if (seconds.length() == 1) seconds = "0" + seconds;
            std::string minutes = std::to_string(static_cast<int>(gameTime.asSeconds()) / 60);
            if (minutes.length() == 1) minutes = "0" + minutes;
            timeText.setString(minutes + ":" + seconds);
            timeText.setPosition(rightTopCorner + sf::Vector2f{-windowMargin.x - timeText.getGlobalBounds().size.x, windowMargin.y});
            window.draw(timeText);

            break;
        }

        case GameState::PAUSE:
        {
            pauseBackground.setPosition({0, 0});
            pauseBackground.setSize(window.getView().getSize());

            window.draw(pauseBackground);
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

            // Time
            timeText.setOrigin(timeText.getLocalBounds().getCenter());
            timeText.setPosition(killText.getPosition() + sf::Vector2f{0.f, 100.f});

            window.draw(deathScreenBackground);
            window.draw(deathScreenText);
            window.draw(killText);
            window.draw(timeText);
            break;
        }
    }
    
}

sf::Time PlayerUI::getBottleTime() { return bottleTime; }

void PlayerUI::activateBottleBar() { bottleBarActive = true; }

void PlayerUI::resetBottleTime() { bottleBarActive = false; }

void PlayerUI::updateKillCount(int kills) { killCount = kills; }

void PlayerUI::setGameState(GameState state) { currentState = state; }