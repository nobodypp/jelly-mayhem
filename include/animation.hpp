#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>


class Animation
{
    private:
        std::vector<sf::Texture>* frames;
        float fps;
        sf::Clock clock;

    public:
        Animation(std::vector<sf::Texture>* frames, float fps);
        void restart();
        sf::Texture& getCurrentFrame();
        unsigned int getCurrentCycle();
        unsigned int getCurrentFrameNumber();
};