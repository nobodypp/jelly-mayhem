#include "animation.hpp"


Animation::Animation(std::vector<sf::Texture>* frames, float fps)
    : fps(fps), 
      frames(frames)
{}

void Animation::restart()
{
    clock.restart();
}

sf::Texture& Animation::getCurrentFrame() { return (*frames)[int(fps * clock.getElapsedTime().asSeconds()) % frames->size()]; }

unsigned int Animation::getCurrentCycle() { return int(fps * clock.getElapsedTime().asSeconds()) / frames->size(); }

unsigned int Animation::getCurrentFrameNumber() { return int (fps * clock.getElapsedTime().asSeconds()) % frames->size();}