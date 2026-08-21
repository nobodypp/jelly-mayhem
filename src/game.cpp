#include "game.hpp"

GameManager::GameManager()
  : gameWindow(sf::VideoMode({winWidth, winHeight}), ""), 
	projectiles(assets, perks),
    player(gameWindow.getSize(), assets, projectiles, perks),
	ground(gameWindow.getSize(), assets), 
	playerView(gameWindow.getDefaultView()), 
	uiView(playerView),
	enemies(assets, player, projectiles, randomizer, perks), 
	texts(assets),
	collisions(enemies, projectiles, texts, perks), 
	ui(assets), 
	currentState(GameState::PLAY)
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

	uiView.setSize(sf::Vector2f(gameWindow.getSize()));
	uiView.setCenter(sf::Vector2f(gameWindow.getSize() / 2u));
}