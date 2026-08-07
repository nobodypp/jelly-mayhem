#include "star.hpp"


Star::Star(sf::Vector2f position, sf::Vector2f targetPos, sf::Color color, TextureManager& textures)
    : fly(&textures.starFly, 10), 
      explode(&textures.starExplode, 10), 
      sprite(fly.getCurrentFrame()), 
      shotTarget(false), 
      inView(true)
{
    sprite.setColor(sf::Color::Green);
    sprite.setOrigin({27.f, 17.f});
    velocity = (targetPos - position).normalized() * linearVelocity;
    sprite.setPosition(position);
    sprite.setRotation(velocity.angle());
    sprite.setColor(color);
}

void Star::update(sf::Time deltaTime)
{
    if (!shotTarget) sprite.move(velocity * deltaTime.asSeconds());
}

void Star::render(sf::RenderWindow& window)
{
    if (!sprite.getGlobalBounds().findIntersection({
    window.getView().getCenter() - window.getView().getSize() / 2.f,
    window.getView().getSize()})) inView = false;


    if (!shotTarget) sprite.setTexture(fly.getCurrentFrame());
    else if (explode.getCurrentCycle() < 1) sprite.setTexture(explode.getCurrentFrame());

    window.draw(sprite);
}

sf::FloatRect Star::getBounds() { return sprite.getGlobalBounds(); }

int Star::getDamage() { return 10; }

void Star::registerHit()
{
    shotTarget = true;
    explode.restart();
}

bool Star::isAlive() { return ((!shotTarget || explode.getCurrentCycle() < 1) && inView); }

bool Star::isColliding() { return !shotTarget; }