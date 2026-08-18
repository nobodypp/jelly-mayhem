#include "game.hpp"

GameManager::GameManager()
  : gameWindow(sf::VideoMode({winWidth, winHeight}), ""), 
	projectiles(assets),
    player(gameWindow.getSize(), assets, projectiles),
	ground(gameWindow.getSize(), assets), 
	playerView(gameWindow.getDefaultView()), 
	uiView(playerView),
	enemies(assets, player, projectiles, randomizer), 
	texts(assets),
	collisions(enemies, projectiles, texts), 
	ui(assets), 
	currentState(GameState::PLAY)
{}

void GameManager::GameLoop()
{
	while ( gameWindow.isOpen() )
	{
		sf::Time deltaTime = clock.restart();

		ui.setGameState(currentState);

		gameWindow.clear();
		
		if (currentState == GameState::PLAY) handleLevel(deltaTime);

		while ( const std::optional event = gameWindow.pollEvent() )
		{
			if ( event->is<sf::Event::Closed>() ) gameWindow.close();
			else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
			{
				if (player.mousePressed(mouseButtonPressed)) ui.activateBottleBar();
			}
			else if (const auto* mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>())
			{
				if (player.mouseReleased(mouseButtonReleased, gameWindow.mapPixelToCoords(mouseButtonReleased->position), ui.getBottleTime())) ui.resetBottleTime();
			}
		}
		handleUI(deltaTime);

		gameWindow.display();

		if (!player.isAlive()) currentState = GameState::LOSE_SCREEN;

	}
}

void GameManager::handleLevel(sf::Time deltaTime)
{
	player.update(deltaTime);
	if (!player.isDying())
	{
		projectiles.update(deltaTime);
		enemies.update(deltaTime);
		texts.update(deltaTime);
		collisions.handleCollisions(player);
	}

	if (!player.isDying()) playerView.setCenter(player.getBounds().position + player.getBounds().size / 2.0f);
	playerView.setSize(sf::Vector2f(gameWindow.getSize()));
	gameWindow.setView(playerView);

	ground.render(gameWindow);
	player.render(gameWindow);
	enemies.render(gameWindow);
	projectiles.render(gameWindow);
	texts.render(gameWindow);
}

void GameManager::handleUI(sf::Time deltaTime)
{
	ui.update(deltaTime);
	ui.updateKillCount(enemies.getKillCount());

	uiView.setSize(sf::Vector2f(gameWindow.getSize()));
	uiView.setCenter(sf::Vector2f(gameWindow.getSize() / 2u));
	gameWindow.setView(uiView);

	ui.render(gameWindow);
}