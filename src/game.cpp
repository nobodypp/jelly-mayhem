#include "game.hpp"

GameManager::GameManager()
  : gameWindow(sf::VideoMode({winWidth, winHeight}), ""), 
    perks(assets, audio),
	projectiles(assets, perks, audio),
    player(assets, projectiles, perks, audio),
	ground(assets), 
	playerView(gameWindow.getDefaultView()), 
	uiView(playerView),
	enemies(assets, player, projectiles, randomizer, perks, audio), 
	texts(assets),
	collisions(enemies, projectiles, texts, perks), 
	ui(assets, perks, audio), 
	currentState(GameState::PLAY), 
	windowZoom(1.f)
{}

void GameManager::GameLoop()
{
	while ( gameWindow.isOpen() )
	{
		// Get current delta time
		sf::Time deltaTime = clock.restart();

		handleEvents();

		update(deltaTime);
		updateViews();

		gameWindow.clear();
		render();
		gameWindow.display();
	}
}

void GameManager::update(sf::Time deltaTime)
{
	ui.setGameState(currentState);

	switch (currentState)
	{
		case GameState::PLAY:
			gameWindow.setView(playerView);

			player.update(deltaTime);

			if (!player.isDying())
			{
				projectiles.update(deltaTime);
				enemies.update(deltaTime);
				texts.update(deltaTime);
				collisions.handleCollisions(player);
			}

			// Update UI
			ui.update(deltaTime);
			ui.updateKillCount(collisions.getKillCount());

			// State transition
			if (!player.isAlive()) currentState = GameState::LOSE_SCREEN;

			break;
		
		case GameState::LOSE_SCREEN:
			// State transition
			if (ui.getRetry())
			{
				currentState = GameState::PLAY;
				player.reset();
				enemies.reset();
				projectiles.reset();
				audio.stopAllSounds();
				texts.reset();
			}
			
			break;
	}
}

void GameManager::render()
{
	switch (currentState)
	{
		case GameState::PLAY:
			// Render level
			gameWindow.setView(playerView);

			ground.render(gameWindow);
			player.render(gameWindow);
			enemies.render(gameWindow);
			projectiles.render(gameWindow);
			texts.render(gameWindow);

			// Render UI
			gameWindow.setView(uiView);

			ui.render(gameWindow);
			break;
		
		case GameState::LOSE_SCREEN:
			gameWindow.setView(uiView);
			ui.render(gameWindow);
			break;
		
		case GameState::PAUSE:
			// Render level
			gameWindow.setView(playerView);

			ground.render(gameWindow);
			player.render(gameWindow);
			enemies.render(gameWindow);
			projectiles.render(gameWindow);
			texts.render(gameWindow);

			// Render UI
			gameWindow.setView(uiView);

			ui.render(gameWindow);

			audio.update();
			break;
	}
}

void GameManager::handleEvents()
{
	while ( const std::optional event = gameWindow.pollEvent() )
	{
		if ( event->is<sf::Event::Closed>() ) gameWindow.close();

		switch (currentState)
		{
			case GameState::PLAY:
				gameWindow.setView(playerView);

				if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
				{
					if (mouseButtonPressed->button == sf::Mouse::Button::Left)
					{
						if (player.startAiming()) ui.activateBottleBar();
					}
					else if (mouseButtonPressed->button == sf::Mouse::Button::Right) player.startBlocking();
				}
				else if (const auto* mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>())
				{
					if (mouseButtonReleased->button == sf::Mouse::Button::Left)
					{
						if (player.shootBottle(gameWindow.mapPixelToCoords(mouseButtonReleased->position), ui.getBottleTime())) ui.resetBottleTime();
					}
				}
				else if (const auto* mouseWheelScrolled = event->getIf<sf::Event::MouseWheelScrolled>())
				{
					windowZoom += mouseWheelScrolled->delta * -0.1f;
					windowZoom = std::max(0.1f, windowZoom);
				}
				else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
				{
					if (keyPressed->code == sf::Keyboard::Key::Space)
					{
						currentState = GameState::PAUSE;
						audio.pauseAllSounds();
						ui.resetBottleTime();
						player.cancelShooting();

					}
				}

				break;
			
			case GameState::PAUSE:
				gameWindow.setView(uiView);

				if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
				{
					if (keyPressed->code == sf::Keyboard::Key::Space)
					{
						currentState = GameState::PLAY;
						audio.resumeAllSounds();
					}
				}
				else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
				{
					if (mouseButtonPressed->button == sf::Mouse::Button::Left) ui.mouseClicked(sf::Vector2f(mouseButtonPressed->position));
				}
				else if (const auto* mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>())
				{
					if (mouseButtonReleased->button == sf::Mouse::Button::Left) ui.mouseReleased(sf::Vector2f(mouseButtonReleased->position));
				}

				break;
			
			case GameState::LOSE_SCREEN:
				if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
				{
					if (mouseButtonPressed->button == sf::Mouse::Button::Left) ui.mouseClicked(sf::Vector2f(mouseButtonPressed->position));
				}
				else if (const auto* mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>())
				{
					if (mouseButtonReleased->button == sf::Mouse::Button::Left) ui.mouseReleased(sf::Vector2f(mouseButtonReleased->position));
				}

				break;
		}
	}
}

void GameManager::updateViews()
{
	// Unless player is during death animation, he should be in the center of the window
	if (!player.isDying()) playerView.setCenter(player.getBounds().position + player.getBounds().size / 2.0f);

	// Update window size
	playerView.setSize(sf::Vector2f(gameWindow.getSize()));
	playerView.zoom(windowZoom);

	uiView.setSize(sf::Vector2f(gameWindow.getSize()));
	uiView.setCenter(sf::Vector2f(gameWindow.getSize() / 2u));
}