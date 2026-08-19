#include "ground.hpp"


Ground::Ground(sf::Vector2u windowSize, AssetManager& textureManager)
    : sprite(textureManager.groundTexture, sf::IntRect({0, 0}, {(int) windowSize.x, (int) windowSize.y}))
{}

void Ground::render(sf::RenderWindow& window)
{
    // Fill the window with ground tiles
    sf::View view = window.getView();
    sf::Vector2f topLeft = view.getCenter() - view.getSize() / 2.f;

    int tileSize = sprite.getTexture().getSize().x;

    int worldX = static_cast<int>(std::floor(topLeft.x));
    int worldY = static_cast<int>(std::floor(topLeft.y));

    int offsetX = ((worldX % tileSize) + tileSize) % tileSize;
    int offsetY = ((worldY % tileSize) + tileSize) % tileSize;

    sprite.setPosition(topLeft);

    sprite.setTextureRect({
        {offsetX, offsetY},
        {
            static_cast<int>(view.getSize().x),
            static_cast<int>(view.getSize().y)
        }
    });
    
    window.draw(sprite);
}