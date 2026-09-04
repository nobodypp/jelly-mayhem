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
	ui(assets, perks, audio, currentState), 
	currentState(GameState::Play), 
	windowZoom(1.f)
{}

void GameManager::GameLoop()
{
	while ( gameWindow.isOpen() && !ui.getQuit())
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
	switch (currentState)
	{
		case GameState::Play:
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
			ui.updateKillCount(enemies.getKillCount());

			// State transition
			if (!player.isAlive()) changeState(GameState::LoseScreen);

			break;
		
		case GameState::LoseScreen:
			// State transition
			if (ui.getRetry()) changeState(GameState::Play);
			break;
		
		case GameState::Pause:
			// State transition
			if (ui.getResume()) changeState(GameState::Play);
			ui.update(deltaTime);
			break;
	}
}

void GameManager::render()
{
	switch (currentState)
	{
		case GameState::Play:
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
		
		case GameState::LoseScreen:
			gameWindow.setView(uiView);
			ui.render(gameWindow);
			break;
		
		case GameState::Pause:
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
			case GameState::Play:
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
					if (keyPressed->code == sf::Keyboard::Key::Space) changeState(GameState::Pause);
				}

				break;
			
			case GameState::Pause:
				gameWindow.setView(uiView);

				if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
				{
					if (keyPressed->code == sf::Keyboard::Key::Space) changeState(GameState::Play);
				}
				else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
				{
					if (mouseButtonPressed->button == sf::Mouse::Button::Left) ui.mouseClicked(sf::Vector2f(mouseButtonPressed->position));
				}
				else if (const auto* mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>())
				{
					if (mouseButtonReleased->button == sf::Mouse::Button::Left) ui.mouseReleased(sf::Vector2f(mouseButtonReleased->position));
				}
				else if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
				{
					ui.mouseMoved(sf::Vector2f(mouseMoved->position));
				}

				break;
			
			case GameState::LoseScreen:
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

void GameManager::changeState(GameState state)
{
	// State transitions
	if (currentState == GameState::Play && state == GameState::Pause)
	{
		audio.pauseAllSounds();
		ui.resetBottleTime();
		player.cancelShooting();		
	}
	else if (currentState == GameState::Pause && state == GameState::Play)
	{
		audio.resumeAllSounds();
	}
	else if (currentState == GameState::LoseScreen && state == GameState::Play)
	{
		player.reset();
		enemies.reset();
		projectiles.reset();
		audio.stopAllSounds();
		perks.reset();
		texts.reset();
	}
	ui.changeGameState(state);
	currentState = state;
}
