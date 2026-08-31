#include "playerui.hpp"


PlayerUI::PlayerUI(AssetManager& assets, PerkManager& perks, AudioManager& audio)
    : bottleChargingAnimation(&assets.bottleBarChargedFrames, 10.f), 
      bottleBarSize(bottleChargingAnimation.getCurrentFrame().getSize()), 
      bottlePrimaryBar(bottleBarSize), 
      bottleSecondaryBar(bottleBarSize), 
      bottleTime(maxBottleTime),
      bottleBarActive(false), 
      killText(assets.font), 
      deathScreenBackground(sf::PrimitiveType::TriangleStrip, 4), 
      deathScreenText(assets.font), 
      perks(perks), 
      audio(audio), 
      bottleChargedBarSprite(bottleChargingAnimation.getCurrentFrame()), 
      gameTime(sf::Time::Zero), 
      timeText(assets.font), 
      announcementText(assets.font), 
      announcementTimeLeft(sf::Time::Zero), 
      retryButton({400.f, 100.f}, "Get good", 30.f, assets, audio)
{
    // Bottle charge bar init
    bottlePrimaryBar.setFillColor(bottleBarPrimaryColor);
    bottleSecondaryBar.setFillColor(bottleBarSecondaryColor);

    // Kill count text
    killText.setCharacterSize(25);
    killText.setFillColor(sf::Color::White);
    killText.setOutlineColor(sf::Color::Black);
    killText.setOutlineThickness(1.f);

    // Game over text
    deathScreenText.setCharacterSize(50);
    deathScreenText.setString("Geym O'vah");
    deathScreenText.setOrigin(deathScreenText.getLocalBounds().getCenter());
    deathScreenText.setFillColor(sf::Color::White);
    deathScreenText.setOutlineColor(sf::Color::Black);
    deathScreenText.setOutlineThickness(1.f);

    // Death screen background
    deathScreenBackground[0].color = sf::Color::Black;
    deathScreenBackground[1].color = sf::Color::Red;
    deathScreenBackground[2].color = sf::Color::Black;
    deathScreenBackground[3].color = sf::Color::Red;

    // Pause screen background
    pauseBackground.setFillColor(sf::Color{0, 0, 0, 80});

    // Time text
    timeText.setCharacterSize(25);
    timeText.setFillColor(sf::Color::White);
    timeText.setOutlineColor(sf::Color::Black);
    timeText.setOutlineThickness(1.f);

    // Perk announcement text
    announcementText.setCharacterSize(25);
    announcementText.setFillColor(sf::Color::White);
    announcementText.setOutlineColor(sf::Color::Black);
    announcementText.setOutlineThickness(1.f);
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

            // Update time counter
            gameTime += deltaTime;

            // If a new announcement is pending, show it
            if (announcementTimeLeft > sf::Time::Zero) announcementTimeLeft = std::max(sf::Time::Zero, announcementTimeLeft - deltaTime);
            else if (const std::string text = perks.getNextAnnouncement(); text != "")
            {
                announcementText.setString(text);
                announcementTimeLeft = announcementDefaultTime;
            }
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
            killText.setOrigin({0.f, 0.f});
            killText.setPosition({windowMargin.x, windowMargin.y});
            killText.setString("Kills: " + std::to_string(killCount));
            window.draw(killText);

            // Time text
            sf::Vector2f rightTopCorner = {static_cast<float> (window.getView().getSize().x), 0.f};
            std::string seconds = std::to_string(static_cast<int>(gameTime.asSeconds()) % 60);
            if (seconds.length() == 1) seconds = "0" + seconds;
            std::string minutes = std::to_string(static_cast<int>(gameTime.asSeconds()) / 60);
            if (minutes.length() == 1) minutes = "0" + minutes;
            timeText.setOrigin({0.f, 0.f});
            timeText.setString(minutes + ":" + seconds);
            timeText.setPosition(rightTopCorner + sf::Vector2f{-windowMargin.x - timeText.getGlobalBounds().size.x, windowMargin.y});
            window.draw(timeText);

            // Perk announcement
            if (announcementTimeLeft > sf::Time::Zero)
            {
                announcementText.setOrigin(announcementText.getLocalBounds().getCenter());
                announcementText.setPosition({window.getView().getSize().x / 2.f , windowMargin.y});

                // Transparency
                std::uint8_t a = static_cast<uint8_t>(announcementTimeLeft.asSeconds() / announcementDefaultTime.asSeconds() * 255);
                
                sf::Color color = announcementText.getFillColor();
                color.a = a;
                announcementText.setFillColor(color);

                color = announcementText.getOutlineColor();
                color.a = a;
                announcementText.setOutlineColor(color);

                window.draw(announcementText);
            }

            break;
        }

        case GameState::PAUSE:
        {
            // Black background
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

            // Retry button
            retryButton.setPosition(timeText.getPosition() + sf::Vector2f{0.f, 100.f});

            window.draw(deathScreenBackground);
            window.draw(deathScreenText);
            window.draw(killText);
            window.draw(timeText);
            retryButton.render(window);
            break;
        }
    }
    
}

sf::Time PlayerUI::getBottleTime() { return bottleTime; }

void PlayerUI::activateBottleBar() { bottleBarActive = true; }

void PlayerUI::resetBottleTime() { bottleBarActive = false; }

void PlayerUI::updateKillCount(int kills) { killCount = kills; }

void PlayerUI::setGameState(GameState state)
{
    // State transitions

    if (currentState == GameState::PLAY && state == GameState::LOSE_SCREEN)
    {
        retryButton.resetWasClicked();
    }
    if (currentState == GameState::LOSE_SCREEN && state == GameState::PLAY)
    {
        gameTime = sf::Time::Zero;
        bottleBarActive = false;
    }

    currentState = state;
}

void PlayerUI::mouseClicked(sf::Vector2f mousePos)
{
    switch (currentState)
    {
        case GameState::LOSE_SCREEN:
            retryButton.mouseClicked(mousePos);
            break;
    }
}

void PlayerUI::mouseReleased(sf::Vector2f mousePos)
{
    switch (currentState)
    {
        case GameState::LOSE_SCREEN:
            retryButton.mouseReleased(mousePos);
            break;
    }
}

bool PlayerUI::getRetry() { return retryButton.getWasClicked(); }
