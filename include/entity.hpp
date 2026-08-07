#pragma once

#include "drawable.hpp"


class Entity : public Drawable
{
    protected:
        virtual sf::FloatRect getBounds() = 0;
};