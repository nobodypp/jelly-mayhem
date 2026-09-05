#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>


class Animation
{
    private:
        sf::Time timeFromRestart = sf::Time::Zero;
        std::vector<sf::Texture>* frames;
        float fps;

    public:
        Animation(std::vector<sf::Texture>* frames, float fps);
        void restart();
        sf::Texture& getCurrentFrame();
        unsigned int getCurrentCycle();
        unsigned int getCurrentFrameNumber();
        void update(sf::Time deltaTime);
        void setFps(float fps);
};