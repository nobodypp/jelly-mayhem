#include "playerui.hpp"


PlayerUI::PlayerUI(AssetManager& assets, PerkManager& perks, AudioManager& audio, GameState state)
    : bottleChargingAnimation(&assets.bottleBarChargedFrames, 10.f), 
      bottleBarSize(bottleChargingAnimation.getCurrentFrame().getSize()), 
      bottlePrimaryBar(bottleBarSize), 
      bottleSecondaryBar(bottleBarSize), 
      killText(assets.font), 
      deathScreenText(assets.font), 
      perks(perks), 
      audio(audio), 
      bottleChargedBarSprite(bottleChargingAnimation.getCurrentFrame()), 
      timeText(assets.font), 
      announcementText(assets.font), 
      loseButtons{
        Button(buttonSize, "Get good", 25.f, assets, audio), 
        Button(buttonSize, "Rage quit", 25.f, assets, audio)
      },
      pauseButtons{
        Button(buttonSize, "Resume", 20.f, assets, audio), 
        Button(buttonSize, "Perks", 20.f, assets, audio), 
        Button(buttonSize, "Options", 20.f, assets, audio), 
        Button(buttonSize, "Quit", 20.f, assets, audio)
      }, 
      previousButton({50.f, 50.f}, "<", 5.f, assets, audio), 
      nextButton({50.f, 50.f}, ">", 5.f, assets, audio), 
      perkName(assets.font), 
      perkObjective(assets.font), 
      perkReward(assets.font), 
      perkBackground({1000.f, 300.f}, "", 30.f, assets, audio), 
      returnButton(buttonSize, "Back", 25.f, assets, audio), 
      volumeText(assets.font)
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
    pauseBackground.setFillColor(sf::Color{0, 0, 0, 150});

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

    // Perk menu
    perkName.setFillColor(sf::Color::Black);
    perkObjective.setFillColor(sf::Color::Black);
    perkReward.setFillColor(sf::Color::Black);
    perkName.setLineAlignment(sf::Text::LineAlignment::Center);
    perkObjective.setLineAlignment(sf::Text::LineAlignment::Center);
    perkReward.setLineAlignment(sf::Text::LineAlignment::Center);

    // Volume bar
    volumePrimaryBar.setFillColor(sf::Color(200, 200, 200));
    volumeSecondaryBar.setFillColor(sf::Color(50, 50, 50));
    volumeButton.setFillColor(sf::Color(230, 230, 230));
    volumeButton.setOutlineColor(sf::Color::Black);
    volumeButton.setOutlineThickness(1.f);
    volumeButton.setOrigin(volumeButton.getLocalBounds().getCenter());
    volumeText.setString("Volume");
    volumeText.setCharacterSize(30);
    volumeText.setOrigin(volumeText.getLocalBounds().getCenter());
}

void PlayerUI::update(sf::Time deltaTime)
{
    switch(currentState)
    {
        case GameState::Play:
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
        
        case GameState::Pause:
            switch (pauseState)
            {
                case PauseScreenState::Menu:
                    if (pauseButtons.at(index(PauseButtonId::Perks)).getWasClicked())
                    {
                        pauseState = PauseScreenState::Perks;
                        returnButton.resetWasClicked();
                        perkId = 0;
                    }
                    else if (pauseButtons.at(index(PauseButtonId::Options)).getWasClicked())
                    {
                        pauseState = PauseScreenState::Options;
                        isVolumeBarClicked = false;
                        returnButton.resetWasClicked();
                    }
                    break;
                
                case PauseScreenState::Perks:
                    if (returnButton.getWasClicked()) changeGameState(GameState::Pause);
                    break;
                
                case PauseScreenState::Options:
                    if (returnButton.getWasClicked()) changeGameState(GameState::Pause);

                    const auto bounds = volumeSecondaryBar.getGlobalBounds();
                    const float volume = static_cast<float>(audio.getVolume()) / 100.f;

                    volumeButton.setPosition({
                        bounds.position.x + bounds.size.x * volume,
                        bounds.getCenter().y
                    });
                    break;
            }
            break;
    }
}

void PlayerUI::render(sf::RenderWindow& window)
{
    switch(currentState)
    {
        case GameState::Play:
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

        case GameState::Pause:
        {
            // Black background
            pauseBackground.setPosition({0, 0});
            pauseBackground.setSize(window.getView().getSize());
            window.draw(pauseBackground);


            switch (pauseState)
            {
                case PauseScreenState::Menu:
                {
                        // Render menu buttons
                    for (std::size_t i = 0; i < pauseButtons.size(); i++)
                    {
                        pauseButtons.at(i).setPosition(window.getView().getCenter() + sf::Vector2f(0.f, (i - pauseButtons.size() * 0.5f) * 150.f));
                        pauseButtons.at(i).render(window);
                    }
                    break;
                }
                
                case PauseScreenState::Perks:
                {
                    perkBackground.setSize(window.getView().getSize() * 0.7f);
                    perkBackground.setPosition(window.getView().getCenter());
                    perkBackground.render(window);

                    auto[name, objective, reward] = perks.getPerkInfo(perkId);
                    perkName.setString(name);
                    perkObjective.setString("Objective:\n" + objective);
                    perkReward.setString("Reward:\n" + reward);
                    perkName.setOrigin({perkName.getLocalBounds().getCenter().x, 0.f});
                    perkObjective.setOrigin(perkObjective.getLocalBounds().getCenter());
                    perkReward.setOrigin({perkReward.getLocalBounds().getCenter().x, perkReward.getGlobalBounds().size.y});
                    perkName.setPosition({perkBackground.getPosition().x, perkBackground.getPosition().y - perkBackground.getSize().y * 0.4f});
                    perkObjective.setPosition(perkBackground.getPosition());
                    perkReward.setPosition({perkBackground.getPosition().x, perkBackground.getPosition().y + perkBackground.getSize().y * 0.4f});
                    window.draw(perkName);
                    window.draw(perkObjective);
                    window.draw(perkReward);

                    previousButton.setPosition({perkBackground.getPosition().x - perkBackground.getSize().x * 0.5f - 100.f, perkBackground.getPosition().y});
                    nextButton.setPosition({perkBackground.getPosition().x + perkBackground.getSize().x * 0.5f + 100.f, perkBackground.getPosition().y});
                    previousButton.render(window);
                    nextButton.render(window);

                    returnButton.setPosition({perkBackground.getPosition().x, perkBackground.getPosition().y + perkBackground.getSize().y * 0.5f + 100.f});
                    returnButton.render(window);
                    break;
                }
                
                case PauseScreenState::Options:
                {
                    volumeText.setPosition({window.getView().getCenter().x, window.getView().getSize().y * 0.2f});
                    window.draw(volumeText);

                    volumeSecondaryBar.setSize({window.getView().getSize().x * 0.7f, 15.f});
                    volumeSecondaryBar.setOrigin(volumeSecondaryBar.getLocalBounds().getCenter());
                    volumeSecondaryBar.setPosition({window.getView().getCenter().x, window.getView().getSize().y * 0.4f});
                    window.draw(volumeSecondaryBar);
                    
                    volumePrimaryBar.setPosition(volumeSecondaryBar.getGlobalBounds().position);
                    volumePrimaryBar.setSize({volumeSecondaryBar.getSize().x * audio.getVolume() / 100.f, volumeSecondaryBar.getSize().y});
                    window.draw(volumePrimaryBar);

                    window.draw(volumeButton);

                    returnButton.setPosition({window.getView().getCenter().x, window.getView().getSize().y * 0.7f});
                    returnButton.render(window);
                    break;
                }
            }  
            break;
        }
        
        case GameState::LoseScreen:
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
            timeText.setPosition(killText.getPosition() + sf::Vector2f{0.f, 70.f});

            // Retry button
            loseButtons.at(index(LoseButtonId::Retry)).setPosition(timeText.getPosition() + sf::Vector2f{0.f, 150.f});

            // Quit button
            loseButtons.at(index(LoseButtonId::Quit)).setPosition(loseButtons.at(index(LoseButtonId::Retry)).getPosition() + sf::Vector2f{0.f, 150.f});

            window.draw(deathScreenBackground);
            window.draw(deathScreenText);
            window.draw(killText);
            window.draw(timeText);
            loseButtons.at(index(LoseButtonId::Retry)).render(window);
            loseButtons.at(index(LoseButtonId::Quit)).render(window);
            break;
        }
    }
    
}

sf::Time PlayerUI::getBottleTime() { return bottleTime; }

void PlayerUI::activateBottleBar() { bottleBarActive = true; }

void PlayerUI::resetBottleTime() { bottleBarActive = false; }

void PlayerUI::updateKillCount(int kills) { killCount = kills; }

void PlayerUI::changeGameState(GameState state)
{
    // State transitions
    if (currentState == GameState::Play && state == GameState::LoseScreen)
    {
        for (auto& button: loseButtons) button.resetWasClicked();
    }
    else if (currentState == GameState::LoseScreen && state == GameState::Play)
    {
        gameTime = sf::Time::Zero;
        bottleBarActive = false;
    }
    else if (state == GameState::Pause)
    {
        for (auto& button : pauseButtons) button.resetWasClicked();
        pauseState = PauseScreenState::Menu;
    }

    currentState = state;
}

void PlayerUI::mouseClicked(sf::Vector2f mousePos)
{
    switch (currentState)
    {
        case GameState::LoseScreen:
            for (auto& button: loseButtons) button.mouseClicked(mousePos);
            break;
        
        case GameState::Pause:
            switch (pauseState)
            {
                case PauseScreenState::Menu:
                    for (auto& button: pauseButtons) button.mouseClicked(mousePos);
                    break;
                
                case PauseScreenState::Perks:
                    previousButton.mouseClicked(mousePos);
                    nextButton.mouseClicked(mousePos);
                    returnButton.mouseClicked(mousePos);
                    break;
                
                case PauseScreenState::Options:
                    returnButton.mouseClicked(mousePos);

                    if (volumeSecondaryBar.getGlobalBounds().contains(mousePos)) isVolumeBarClicked = true;
                    break;
            }
            break;
    }
}

void PlayerUI::mouseReleased(sf::Vector2f mousePos)
{
    switch (currentState)
    {
        case GameState::LoseScreen:
            for (auto& button: loseButtons) button.mouseReleased(mousePos);
            break;
        
        case GameState::Pause:
            switch (pauseState)
            {
                case PauseScreenState::Menu:
                    for (auto& button: pauseButtons) button.mouseReleased(mousePos);
                    break;
                
                case PauseScreenState::Perks:
                    if (previousButton.mouseReleased(mousePos)) perkId--;
                    if (nextButton.mouseReleased(mousePos)) perkId++;
                    returnButton.mouseReleased(mousePos);
                    break;
                
                case PauseScreenState::Options:
                    returnButton.mouseReleased(mousePos);
                    isVolumeBarClicked = false;
                    break;
            }            
            break;
    }
}

void PlayerUI::mouseMoved(sf::Vector2f mousePos)
{
    if (currentState == GameState::Pause && pauseState == PauseScreenState::Options && isVolumeBarClicked)
    {
        sf::Vector2f pos{std::clamp(static_cast<float>(mousePos.x), volumeSecondaryBar.getGlobalBounds().position.x, volumeSecondaryBar.getGlobalBounds().position.x + volumeSecondaryBar.getGlobalBounds().size.x), volumeSecondaryBar.getGlobalBounds().getCenter().y};
        volumeButton.setPosition(pos);
        int volume = static_cast<int>((volumeButton.getPosition().x - volumeSecondaryBar.getGlobalBounds().position.x) * 100.f / volumeSecondaryBar.getGlobalBounds().size.x);
        audio.setVolume(volume);
    }
}

bool PlayerUI::getRetry() { return loseButtons.at(index(LoseButtonId::Retry)).getWasClicked(); }

bool PlayerUI::getQuit() { return loseButtons.at(index(LoseButtonId::Quit)).getWasClicked() || pauseButtons.at(index(PauseButtonId::Quit)).getWasClicked(); }

bool PlayerUI::getResume() { return pauseButtons.at(index(PauseButtonId::Resume)).getWasClicked(); }
