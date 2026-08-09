#pragma once

#include "drawable.hpp"


class Entity : public Drawable
{
    public:
        virtual sf::FloatRect getBounds() = 0;
        virtual bool isAlive() { return true; }
};