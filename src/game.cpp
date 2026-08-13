#include "game.hpp"

GameManager::GameManager()
  : gameWindow(sf::VideoMode({winWidth, winHeight}), ""), 
	projectiles(textures),
    player(gameWindow.getSize(), textures, projectiles),
	ground(gameWindow.getSize(), textures), 
	playerView(gameWindow.getDefaultView()), 
	uiView(playerView),
	enemies(textures, player, projectiles, randomizer), 
	texts(textures),
	collisions(enemies, projectiles, texts)
{}

void GameManager::GameLoop()
{
	while ( gameWindow.isOpen() )
	{
		sf::Time deltaTime = clock.restart();

		gameWindow.clear();
		
		handleLevel(deltaTime);

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
	}
	std::cout <<  "Zabójstwa: " << enemies.killCount << "\n";
}

void GameManager::handleLevel(sf::Time deltaTime)
{
	player.update(deltaTime);
	projectiles.update(deltaTime);
	enemies.update(deltaTime);
	texts.update(deltaTime);
	collisions.handleCollisions(player);

	playerView.setCenter(player.getBounds().position + player.getBounds().size / 2.0f);
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

	uiView.setSize(sf::Vector2f(gameWindow.getSize()));
	uiView.setCenter(sf::Vector2f(gameWindow.getSize() / 2u));
	gameWindow.setView(uiView);

	ui.render(gameWindow);
}