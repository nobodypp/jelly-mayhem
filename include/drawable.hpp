#pragma once

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>


class Drawable
{
    protected:
        virtual void update(sf::Time deltaTime);
        virtual void render(sf::RenderWindow& window) = 0;
};