#include "game.hpp"

GameManager::GameManager()
  : gameWindow(sf::VideoMode({winWidth, winHeight}), "", sf::Style::Close), 
	projectiles(textures),
    player(gameWindow.getSize(), textures, projectiles),
	ground(gameWindow.getSize(), textures), 
	view(gameWindow.getDefaultView()), 
	enemies(textures, player, projectiles), 
	texts(textures),
	collisions(enemies, projectiles, texts)
{
    srand(time(0));
}

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

	view.setCenter(player.getBounds().position + player.getBounds().size / 2.0f);
	gameWindow.setView(view);

	ground.render(gameWindow);
	player.render(gameWindow);
	enemies.render(gameWindow);
	projectiles.render(gameWindow);
	texts.render(gameWindow);
}

void GameManager::handleUI(sf::Time deltaTime)
{
	ui.update(deltaTime);

	gameWindow.setView(gameWindow.getDefaultView());

	ui.render(gameWindow);
}