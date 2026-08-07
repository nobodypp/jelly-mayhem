#pragma once

#include "entity.hpp"


class Projectile : public Entity
{
    protected:
        virtual int getDamage() = 0;
        virtual bool isColliding() = 0;
        virtual bool isAlive() = 0;
        virtual void registerHit() = 0;
};